
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "libvol2bird.h"


#define FPRINTFON



int analysecells(unsigned char *dbzImage,unsigned char *vradImage,
                 unsigned char *texImage, unsigned char *clutterImage, int *cellImage,
                 SCANMETA *dbzMeta, SCANMETA *vradMeta, SCANMETA *texMeta, SCANMETA *clutterMeta,
                 int nCells, int areaMin, float cellDbzMin, float cellStdDevMax, float cellClutterFraction,
                 float vradMinValue,float dbzClutterMin, unsigned char cmFlag,
                 unsigned char dualPolFlag, unsigned char verbose) {

    //  *********************************************************************************
    //  This function analyses the cellImage array found by the 'findcells' procedure.
    //  Smalls cells are rejected and the cells are re-numbered according to size.
    //  The final number of cells in cellImage is returned as an integer.
    //  *********************************************************************************

#ifdef FPRINTFON
    fprintf(stderr,"Begin of analysecells in C.\n");
#endif


    CELLPROP *cellProp;
    int iCell;
    int iGlobal;
    int iRang;
    int iAzim;
    int nCellsValid;
    int nAzim;
    int nRang;
    float validArea;
    float dbzValue;
    float texValue;
    float vradValue;
    float clutterValue;

    nCellsValid = nCells;
    nRang = dbzMeta->nRang;
    nAzim = dbzMeta->nAzim;

    /*Allocating and initializing memory for cell properties.*/
    cellProp = (CELLPROP *)malloc(nCells*sizeof(CELLPROP));
    if (!cellProp) {
        printf("Requested memory could not be allocated!\n");
        exit(10);
    }
    for (iCell = 0; iCell < nCells; iCell++) {
        cellProp[iCell].iRangOfMax = 0;
        cellProp[iCell].iAzimOfMax = 0;
        cellProp[iCell].area = 0;
        cellProp[iCell].clutterArea = 0;
        cellProp[iCell].dbzAvg = 0;
        cellProp[iCell].texAvg = 0;
        cellProp[iCell].dbzMax = dbzMeta->valueOffset;
        cellProp[iCell].index = iCell;
        cellProp[iCell].drop = 0;
        cellProp[iCell].cv = 0;
    }

    /*Calculation of cell properties.*/
    for (iAzim = 0; iAzim < nAzim; iAzim++) {
        for (iRang = 0; iRang < nRang; iRang++) {

            iGlobal = iRang + iAzim * nRang;

            dbzValue = dbzMeta->valueScale * dbzImage[iGlobal] + dbzMeta->valueOffset;
            vradValue = vradMeta->valueScale * vradImage[iGlobal] + vradMeta->valueOffset;
            clutterValue = clutterMeta->valueScale * clutterImage[iGlobal] + clutterMeta->valueOffset;
            texValue = texMeta->valueScale * texImage[iGlobal] + texMeta->valueOffset;

#ifdef FPRINTFON
            fprintf(stderr,"dbzValue = %f; vradValue = %f; clutterValue = %f; texValue = %f\n",dbzValue,vradValue,clutterValue,texValue);
#endif

            iCell = cellImage[iGlobal];


#ifdef FPRINTFON
            fprintf(stderr,"iGlobal = %d, iCell = %d\n",iGlobal,iCell);
#endif

            if (iCell<0) {
                continue;
            }

            cellProp[iCell].area += 1;

            //low radial velocities are treated as clutter, not included in calculation cell properties
            if (fabs(vradValue) < vradMinValue){
                cellProp[iCell].clutterArea += 1;
#ifdef FPRINTFON
                fprintf(stderr,"iGlobal = %d: vrad too low...treating as clutter\n",iGlobal);
#endif
                continue;
            }

            //pixels in clutter map not included in calculation cell properties
            if (cmFlag == 1){
                if (iAzim < nAzim && iRang < nRang){
                    if (clutterValue > dbzClutterMin){
                        cellProp[iCell].clutterArea += 1;
                        continue;
                    }
                }
            }



            if (dbzValue > cellProp[iCell].dbzMax) {
#ifdef FPRINTFON
                fprintf(stderr,"%d: new dbzMax value of %f found for this cell (%d).\n",iGlobal,dbzValue,iCell);
#endif
                cellProp[iCell].dbzMax = dbzValue;
                cellProp[iCell].iRangOfMax = iGlobal%nAzim; // FIXME mod nAzim? I expected nRang
                cellProp[iCell].iAzimOfMax = iGlobal/nAzim; // FIXME div nAzim? I expected nRang
            }
            cellProp[iCell].dbzAvg += dbzValue;
            cellProp[iCell].texAvg += texValue;
        }
    }


    for (iCell = 0; iCell < nCells; iCell++) {
        validArea = cellProp[iCell].area - cellProp[iCell].clutterArea;
        if (validArea > 0){
            cellProp[iCell].dbzAvg /= validArea;
            cellProp[iCell].texAvg /= validArea;
            // FIXME why have this next calculation two times, different methods?
            cellProp[iCell].cv = 10 * log10(cellProp[iCell].texAvg) - cellProp[iCell].dbzAvg;
            cellProp[iCell].cv = cellProp[iCell].texAvg / cellProp[iCell].dbzAvg;
        }
    }

    /*Determine which cells to drop from map based on low mean dBZ / high stdev /
     * small area / high percentage clutter*/
    for (iCell = 0; iCell < nCells; iCell++) {
        if (dualPolFlag == 1){
            if (cellProp[iCell].area < areaMin) {
                cellProp[iCell].drop = 1;
            }
        }
        else {
            if (cellProp[iCell].area < areaMin ||
                (cellProp[iCell].dbzAvg < cellDbzMin &&
                 cellProp[iCell].texAvg > cellStdDevMax &&
                 (cellProp[iCell].clutterArea / cellProp[iCell].area) < cellClutterFraction )) {
                // FIXME I don't see how this condition is correct -- why are terms 2,3 and
                // 4 combined with &&
                cellProp[iCell].drop = 1;
            }
        }
    }

    /*Sorting cell properties according to cell area. Drop small cells from map*/
    nCellsValid = updatemap(cellImage,cellProp,nCells,nAzim*nRang,areaMin);

    //Printing of cell properties to stdout.
    if (verbose==1){
        printf("#Cell analysis for elevation %f:\n",dbzMeta->elev);
        printf("#Minimum cell area in pixels   : %i\n",areaMin);
        printf("#Threshold for mean dBZ cell   : %g dBZ\n",cellDbzMin);
        printf("#Threshold for mean stdev cell : %g dBZ\n",cellStdDevMax);
        printf("#Valid cells                   : %i/%i\n#\n",nCellsValid,nCells);
        printf("cellinfo: NUM CellArea ClutArea AvgDbz AvgStdev CV MaxVal MaxIII MaxJJJ Dropped\n");
        for (iCell = 0; iCell < nCells; iCell++) {
            if (cellProp[iCell].area==0) {
                continue;
            }
            printf("cellinfo: %3d %8.1f %8.1f %6.2f %6.2f %6.2f %6.2f %6d %6d %6d\n",
                    iCell+2,
                    cellProp[iCell].area,
                    cellProp[iCell].clutterArea,
                    cellProp[iCell].dbzAvg,
                    cellProp[iCell].texAvg,
                    cellProp[iCell].cv,
                    cellProp[iCell].dbzMax,
                    cellProp[iCell].iRangOfMax,
                    cellProp[iCell].iAzimOfMax,
                    cellProp[iCell].drop);
        }
    }

    free(cellProp);

#ifdef FPRINTFON
    fprintf(stderr,"End of analysecells in C.\n");
#endif


    return nCellsValid;
} //analysecells









float dist(int range1, int azim1, int range2, int azim2, float rscale,
        float ascale) {

    //  ******************************************************************************
    //  This function calculates the distance in km between two gates
    //  ******************************************************************************


    // FIXME looks like this calculation is wrong (issue #31)
    float x1;
    float x2;
    float y1;
    float y2;
    x1 = range1 * cos(azim1 * ascale * DEG2RAD);
    x2 = range2 * cos(azim2 * ascale * DEG2RAD);
    y1 = range1 * sin(azim1 * ascale * DEG2RAD);
    y2 = range2 * sin(azim2 * ascale * DEG2RAD);
    return sqrt(SQUARE(x1-x2) + SQUARE(y1 - y2));
} //dist






int findcells(unsigned char *texImage, unsigned char *rhoImage,
        unsigned char *zdrImage, int *cellImage, SCANMETA *texMeta,
        SCANMETA *rhoMeta, SCANMETA *zdrMeta, float texThresMin,
        float rhoThresMin, float zdrThresMin, float dbzThresMin,
        float rCellMax, char sign) {


    //  *****************************************************************************
    //  This function detects the cells in '[tex|rho|zdr]Image' using an integer
    //  threshold value of '[tex|rho|zdr]Thres' and a non-recursive algorithm which
    //  looks for neighboring pixels above threshold. On return the marked cells are
    //  contained by 'cellImage'. The number of detected cells/highest index value is
    //  returned.
    //  *****************************************************************************



    // FIXME it seems the algorithm only works as intended if sign is equal
    // to -1 (or minus anything for that matter). Why is it an input argument then?

    int iCellIdentifier;
    int nCells;
    int iRang;
    int nRang;
    int iAzim;
    int nAzim;
    int iRangLocal;
    int iAzimLocal;
    int iNeighborhood;
    int count;
    int cellImageInitialValue;

    float texThres;
    float rhoThres;
    float zdrThres;

    int iGlobal;
    int iGlobalOther;
    int nGlobal;
    int iLocal;

    int texMissing;
    int texnAzim;
    int texnRang;
    float texValueOffset;
    float texValueScale;
    float texRangeScale;

    int rhoMissing;
    int rhonAzim;
    int rhonRang;
    float rhoValueOffset;
    float rhoValueScale;

    int zdrMissing;
    int zdrnAzim;
    float zdrnRang;
    float zdrValueOffset;
    float zdrValueScale;

    texMissing = texMeta->missing;
    texnAzim = texMeta->nAzim;
    texnRang = texMeta->nRang;
    texValueOffset = texMeta->valueOffset;
    texValueScale = texMeta->valueScale;
    texRangeScale = texMeta->rangeScale;

    rhoMissing = rhoMeta->missing;
    rhonAzim = rhoMeta->nAzim;
    rhonRang = rhoMeta->nRang;
    rhoValueOffset = rhoMeta->valueOffset;
    rhoValueScale = rhoMeta->valueScale;

    zdrMissing = zdrMeta->missing;
    zdrnAzim = zdrMeta->nAzim;
    zdrnRang = zdrMeta->nRang;
    zdrValueOffset = zdrMeta->valueOffset;
    zdrValueScale = zdrMeta->valueScale;


    nAzim = texnAzim;
    nRang = texnRang;

    nGlobal = nAzim * nRang;

    texThres = ROUND((texThresMin - texValueOffset) / texValueScale);

    if (rhoImage != NULL) {
        rhoThres = ROUND((rhoThresMin - rhoValueOffset) / rhoValueScale);
    }
    if (zdrImage != NULL) {
        zdrThres = ROUND((zdrThresMin - zdrValueOffset) / zdrValueScale);
    }

    /*Initializing of connection cellImage.*/
    cellImageInitialValue = -1;
    for (iGlobal = 0; iGlobal < nGlobal; iGlobal++) {
        cellImage[iGlobal] = cellImageInitialValue;
    }

    /*If threshold value is equal to missing value, return.*/
    if (texThres == texMissing) {
        return -1;
    }

    /*Labeling of groups of connected pixels using horizontal, vertical, and */
    /*diagonal connections. The algorithm is described in 'Digital Image */
    /*Processing' by Gonzales and Woods published by Addison-Wesley.*/

    iCellIdentifier = 0;

    for (iAzim = 0; iAzim < nAzim; iAzim++) {
        for (iRang = 0; iRang < nRang; iRang++) {

            iGlobal = iRang + iAzim * nRang;

            if ((iRang + 1) * texRangeScale > rCellMax) {
                // FIXME the left hand side of the condition above is a distance; the
                // right hand side's data type suggests a number of array elements
                continue;
            }

#ifdef FPRINTFON
            fprintf(stderr,"iGlobal = %d\n",iGlobal);
#endif
            if (rhoImage == NULL) {

                if (texImage[iGlobal] == texMissing) {
#ifdef FPRINTFON
                    fprintf(stderr,"texImage[%d] == texMissing\n",iGlobal);
#endif
                    continue;
                }

                if (sign * texImage[iGlobal] > sign * texThres) { // FIXME why sign x2? ... sort of an ABS?
#ifdef FPRINTFON
                    fprintf(stderr,"sign * texImage[%d] > sign * texThres\n",iGlobal);
                    fprintf(stderr,"sign = %d; texImage[%d] = %d; texThres = %f\n",sign,iGlobal,texImage[iGlobal],texThres);
#endif
                    continue;
                }

                /* count number of direct neighbors above threshold */
                count = 0;
                for (iNeighborhood = 0; iNeighborhood < 9; iNeighborhood++) {
                    iRangLocal = (iRang - 1 + iNeighborhood % 3);
                    iAzimLocal = (nAzim + (iAzim - 1 + iNeighborhood / 3)) % nAzim;
                    iLocal = iRangLocal + iAzimLocal * nRang; // FIXME see issue #32
                    if (iLocal >= nGlobal || iLocal < 0) {
                        continue;
                    }
                    if (sign * texImage[iLocal] <= sign * texThres) {
                        // FIXME sign 2x ?
                        // FIXME shouldn't it be '>' instead of '<='?
                        count++;
                    }

                }
                /* when not enough qualified neighbors, continue */
                if (count - 1 < NEIGHBOURS) {
                    continue;
                }
#ifdef FPRINTFON
                fprintf(stderr,"iGlobal = %d, count = %d\n",iGlobal,count);
#endif

            }
            else {
                if (rhoImage[iGlobal] == rhoMissing) {
#ifdef FPRINTFON
                    fprintf(stderr,"rhoImage[%d] == rhoMissing\n",iGlobal);
#endif
                    continue;
                }
                if (zdrImage[iGlobal] == zdrMissing) {
#ifdef FPRINTFON
                    fprintf(stderr,"zdrImage[%d] == zdrMissing\n",iGlobal);
#endif
                    continue;
                }
                if (texImage[iGlobal] == texMissing) {
#ifdef FPRINTFON
                    fprintf(stderr,"texImage[%d] == texMissing\n",iGlobal);
#endif
                    continue;
                }
                if (texImage[iGlobal] < dbzThresMin) { // FIXME tex v refl why?
#ifdef FPRINTFON
                    fprintf(stderr,"texImage[%d] < dbzThresMin\n",iGlobal);
#endif
                    continue;
                }
                if (!(zdrImage[iGlobal] > zdrThres || rhoImage[iGlobal] > rhoThres)) {
#ifdef FPRINTFON
                    fprintf(stderr,"!(zdrImage[%d] > zdrThres || rhoImage[%d] > rhoThres)\n",iGlobal,iGlobal);
#endif
                    continue;
                }

                for (iNeighborhood = 0; iNeighborhood < 9; iNeighborhood++) {
                    iRangLocal = (iRang - 1 + iNeighborhood % 3);
                    iAzimLocal = (nAzim + (iAzim - 1 + iNeighborhood / 3)) % nAzim;
                    iLocal = iRangLocal + iAzimLocal * nRang; // FIXME see issue #32
                    if (zdrImage[iLocal] > zdrThres || rhoImage[iLocal] > rhoThres ) {
                        count++;
                    }
                }
                /* when not enough qualified neighbors, continue */
                if (count - 1 < NEIGHBOURS) {
                    continue;
                }

            } // if (rhoImage==NULL)


            /*Looking for horizontal, vertical, forward diagonal, and backward diagonal */
            /*connections.*/

            for (iNeighborhood = 0; iNeighborhood < 4; iNeighborhood++) {

                iRangLocal = (iRang - 1 + iNeighborhood % 3);
                iAzimLocal = (nAzim + (iAzim - 1 + iNeighborhood / 3)) % nAzim;
                iLocal = iRangLocal + iAzimLocal * nRang;

                /* index out of range, go to next pixel within neighborhood */
                if (iRangLocal < 0 || iRangLocal >= nRang ||
                    iAzimLocal < 0 || iAzimLocal >= nAzim) {
                    // FIXME possibly affected by issue #32
                    continue;
                }

                /* no connection found, go to next pixel within neighborhood */
                if (cellImage[iLocal] == cellImageInitialValue) {
                    continue;
                }

                /* if pixel still unassigned, assign same iCellIdentifier as connection */
                if (cellImage[iGlobal] == cellImageInitialValue) {
                    cellImage[iGlobal] = cellImage[iLocal];
                }
                else {
                    /* if connection found but pixel is already assigned a different iCellIdentifier: */
                    if (cellImage[iGlobal] != cellImage[iLocal]) {
                        /* merging cells detected: replace all other occurences by value of connection: */
                        for (iGlobalOther = 0; iGlobalOther < nGlobal; iGlobalOther++) {
                            if (cellImage[iGlobalOther] == cellImage[iGlobal]) {
                                cellImage[iGlobalOther] = cellImage[iLocal];
                            }
                            /*note: not all iCellIdentifier need to be used eventually */
                        }
                    }
                }
            }

            /*When no connections are found, give a new number.*/
            if (cellImage[iGlobal] == cellImageInitialValue) {
#ifdef FPRINTFON
                fprintf(stderr, "new cell found...assigning number %d\n",iCellIdentifier);
#endif
                cellImage[iGlobal] = iCellIdentifier;
                iCellIdentifier++;
            }

        } // (iRang=0; iRang<nRang; iRang++)
    } // for (iAzim=0; iAzim<nAzim; iAzim++)

    /*Returning number of detected cells (including fringe/clutter) .*/

    nCells = iCellIdentifier;

    return nCells;
} //findcells







void fringecells(int *cellImage, int nRang, int nAzim, float aScale,
        float rScale, float fringe) {

    //  ******************************************************************************
    //  This function enlarges cellImage by additional fringe.
    //  First a block around each pixel is searched for pixels within a distance
    //  equal to 'fringe'.
    //  ******************************************************************************


    int iRang;
    int iAzim;
    int iNeighborhood;
    int iRangLocal;
    int iAzimLocal;
    int iLocal;
    int rBlock;
    int aBlock;
    int edge;
    int iGlobal;
    float tmp;

    rBlock = ROUND(fringe / rScale);
    for (iAzim = 0; iAzim < nAzim; iAzim++) {
        for (iRang = 0; iRang < nRang; iRang++) {

            iGlobal = iRang + iAzim * nRang;

            if (cellImage[iGlobal] <= 1) {
                fprintf(stderr, "iGlobal = %d; cellImage[iGlobal] = %d: skip\n",
                        iGlobal, cellImage[iGlobal]);
                continue; //already fringe or not in cellImage
            }
            fprintf(stderr, "iGlobal = %d; cellImage[iGlobal] = %d: pass\n",
                    iGlobal, cellImage[iGlobal]);

            //determine whether current pixel is a pixel on the edge of a cell
            edge = 0;
            for (iNeighborhood = 0; iNeighborhood < 9 && !edge;
                    iNeighborhood++) {
                iRangLocal = iRang - 1 + iNeighborhood % 3;
                iAzimLocal = (nAzim + (iAzim - 1 + iNeighborhood / 3)) % nAzim;
                iLocal = iRangLocal + iAzimLocal * nRang;

                fprintf(stderr,
                        "iAzimLocal = %d; iRangLocal = %d; iLocal = %d\n",
                        iAzimLocal, iRangLocal, iLocal);

                if (iRangLocal < 0 || iRangLocal >= nRang) {
                    // In the condition above, check only the range dimension,
                    // because the azimuth dimension is wrapped (polar plot);
                    // iAzim = 0  is adjacent to iAzim = nAzim-1.
                    continue;
                }
                if (cellImage[iLocal] < 1) {
                    edge = 1; //FIXME THIS SHOULD BE <=1, BUT THEN TAKES MUCH MUCH LONGER
                }
            } //NOW ONLY CELL PIXELS WITHOUT ANY BORDERING FRINGE ARE 'FRINGED'

            if (!edge) {
                continue;
            }

            //search a limited block around pixel (iRang,iAzim) only
            tmp = (XYMAX(0,iRang-rBlock) * rScale * aScale * DEG2RAD); // FIXME Unclear that this is correct
            if (tmp < fringe / nAzim) {
                aBlock = nAzim;
            } else {
                aBlock = ROUND(fringe / tmp);
            }

            fprintf(stderr, "aBlock = %d; rBlock = %d\n", aBlock, rBlock);

            for (iNeighborhood = 0;
                    iNeighborhood < (2 * rBlock + 1) * (2 * aBlock + 1);
                    iNeighborhood++) {

                iRangLocal = iRang - rBlock + iNeighborhood % (2 * rBlock + 1);
                iAzimLocal = (nAzim
                        + (iAzim - aBlock + iNeighborhood / (2 * rBlock + 1)))
                        % nAzim;
                iLocal = iRangLocal + iAzimLocal * nRang;
                if (iRangLocal < 0 || iRangLocal >= nRang) {
                    // In the condition above, check only the range dimension,
                    // because the azimuth dimension is wrapped (polar plot);
                    // iAzim = 0  is adjacent to iAzim = nAzim-1.
                    continue;
                }
                //if not within range or already in cellImage or already a fringe, do nothing
                if (dist(iRang, iAzim, iRangLocal, iAzimLocal, rScale, aScale)
                        > fringe || cellImage[iLocal] >= 1) {
                    // FIXME note that this condition contains a call to dist(), which
                    // is known to be wrong (issue #31)
                    continue;
                }
                //include pixel (iRangLocal,iAzimLocal) in fringe
                cellImage[iLocal] = 1; //assign index 1 to fringes
            }
        }
    } //for
    return;
} //fringecells






void sortcells(CELLPROP *cellProp, int nCells, int method) {


    //  *****************************************************************************
    //  Sorting of the cell properties using cell area or mean.
    //  Assume an area or mean equal to zero for cells that are marked 'dropped'
    //  *****************************************************************************


    int iCell;
    int iCellOther;
    CELLPROP tmp;

    /*Sorting of data elements using straight insertion method.*/
    if (method == BYAREA) {
        for (iCell = 1; iCell < nCells; iCell++) {

            tmp = cellProp[iCell];

            iCellOther = iCell - 1;

            while (iCellOther >= 0
                    && cellProp[iCellOther].area
                            * XABS(cellProp[iCellOther].drop - 1)
                            < tmp.area * XABS(tmp.drop - 1)) {

                cellProp[iCellOther + 1] = cellProp[iCellOther];

                iCellOther--;
            }

            cellProp[iCellOther + 1] = tmp;

        } //for iCell
    } else if (method == BYMEAN) {
        for (iCell = 1; iCell < nCells; iCell++) {

            tmp = cellProp[iCell];

            iCellOther = iCell - 1;

            while (iCellOther >= 0
                    && cellProp[iCellOther].dbzAvg
                            * XABS(cellProp[iCellOther].drop - 1)
                            < tmp.dbzAvg * XABS(tmp.drop - 1)) {

                cellProp[iCellOther + 1] = cellProp[iCellOther];

                iCellOther--;
            }

            cellProp[iCellOther + 1] = tmp;

        } //for iCell
    }

    return;
} //sortcells





void texture(unsigned char *texImage, unsigned char *vradImage,
        unsigned char *reflImage, SCANMETA *texMeta, SCANMETA *vradMeta,
        SCANMETA *reflMeta, unsigned char nRangNeighborhood,
        unsigned char nAzimNeighborhood, unsigned char nCountMin,
        unsigned char texType) {


    //  ****************************************************************************************
    //  This function computes a texture parameter based on a block of (nRangNeighborhood x
    //  nAzimNeighborhood) pixels. The texture parameter equals the local standard deviation
    //  in the velocity field (when texType==STDEV).
    //  ****************************************************************************************


    int iRang;
    int iAzim;
    int iRangLocal;
    int iAzimLocal;
    int nRang;
    int nAzim;
    int iNeighborhood;
    int count;
    unsigned char missingValue;
    int value;
    int index;
    double vmoment1;
    double vmoment2;
    double dbz;
    double tex;
    int iGlobal;
    int iLocal;
    int nGlobal;
    int nNeighborhood;
    float texOffset;
    float texScale;
    float reflOffset;
    float reflScale;
    float vradOffset;
    float vradScale;
    float vRadDiff;

    nRang = vradMeta->nRang;
    nAzim = vradMeta->nAzim;
    nGlobal = nAzim * nRang;
    missingValue = vradMeta->missing; // FIXME this missingValue is used indiscriminately in vRad, tex and dbz alike

    reflOffset = reflMeta->valueOffset;
    reflScale = reflMeta->valueScale;

    vradOffset = vradMeta->valueOffset;
    vradScale = vradMeta->valueScale;

    texOffset = texMeta->valueOffset;
    texScale = texMeta->valueScale;

    tex = texOffset; // FIXME why does this variable have a value at this point?

    nNeighborhood = nRangNeighborhood * nAzimNeighborhood;

    for (iAzim = 0; iAzim < nAzim; iAzim++) {
        for (iRang = 0; iRang < nRang; iRang++) {

            iGlobal = iRang + iAzim * nRang;

            /* count number of direct neighbours above threshold */
            count = 0;
            vmoment1 = 0;
            vmoment2 = 0;

            dbz = reflOffset + reflScale * reflImage[iGlobal];

            for (iNeighborhood = 0; iNeighborhood < nNeighborhood; iNeighborhood++) {

                iRangLocal = iRang - nRangNeighborhood / 2 + iNeighborhood % nRangNeighborhood;
                iAzimLocal = (nAzim + (iAzim - nAzimNeighborhood / 2 + iNeighborhood / nRangNeighborhood)) % nAzim;

                iLocal = iRangLocal + iAzimLocal * nRang;

                //fprintf(stderr,"    iAzim=%d,iRang=%d,iNeighborhood=%d;iAzimLocal=%d,iRangLocal=%d:iLocal=%d\n",iAzim, iRang, iNeighborhood, iAzimLocal,iRangLocal,iLocal);

                // FIXME iLocal is wrong at the iRang = 0 and iRang = nRang-1 edges: issue #32
                if (iLocal >= nGlobal || iLocal < 0) {
                    continue;
                }
                if (vradImage[iLocal] == missingValue || reflImage[iLocal] == missingValue) {
                    continue;
                }

                // FIXME why difference between local and global?
                //fprintf(stderr,"vradOffset=%f, vradScale = %f, vradImage[iGlobal]=%d, vradImage[iLocal]=%d\n",
                //         vradOffset, vradScale, vradImage[iGlobal], vradImage[iLocal]);

                vRadDiff = vradOffset + vradScale * (vradImage[iGlobal] - vradImage[iLocal]);
                vmoment1 += vRadDiff;
                vmoment2 += SQUARE(vRadDiff);

                dbz += reflOffset + reflScale * reflImage[iLocal];

                count++;

            }

            vmoment1 /= count;
            vmoment2 /= count;
            dbz /= count;

            /* when not enough neighbours, continue */
            if (count < nCountMin) {
                texImage[iGlobal] = missingValue;
            } else {
                if (texType == TEXCV) {

                    tex = 10 * log10(sqrt(XABS(vmoment2-SQUARE(vmoment1)))) - dbz;

                    if (tex < texOffset + texScale) { // FIXME tex < texOffset would make more sense
                        tex = texOffset + texScale;
                    }

                }

                if (texType == TEXSTDEV) {
                    tex = sqrt(XABS(vmoment2-SQUARE(vmoment1)));
                }

                texImage[iGlobal] = ROUND((tex - texOffset) / texScale);

                fprintf(stderr,
                        "(C) count = %d; nCountMin = %d; texType = %d; vmoment1 = %f; vmoment2 = %f; tex = %f; texBody[%d] = %d\n",
                        count, nCountMin, texType, vmoment1, vmoment2, tex,
                        iGlobal, texImage[iGlobal]);

            } //else
        } //for
    } //for
} //texture





int updatemap(int *cellImage, CELLPROP *cellProp, int nCells, int nGlobal,
        int minCellArea) {

    //  *****************************************************************************
    //  This function updates the cellImage by dropping cells and reindexing the map
    //  Leaving index 0 unused, will be used for assigning cell fringes
    //  *****************************************************************************


    int iGlobal;
    int iCellValid;
    int nCellsValid;
    int cellImageValue;

    nCellsValid = nCells;

    for (iGlobal = 0; iGlobal < nGlobal; iGlobal++) {

        if (cellImage[iGlobal] == -1) {
            continue;
        }
        // FIXME this may be a bug: the line below reads an identifier number
        // from cellImage, which is then used as an index into cellProp[].
        // I image the cellProp array starts at index 0, which should contain
        // the properties of the cell which is identifiable in cellImage by
        // its value...2!  (0 and 1 are reserved for other stuff it seems)

        cellImageValue = cellImage[iGlobal];
        if (cellProp[cellImageValue].drop) {
            cellImage[iGlobal] = -1;
        }
    }

    /*Sort the cells by minCellArea and determine number of valid cells*/
    sortcells(cellProp, nCells, BYAREA);

    while (nCellsValid > 0 && cellProp[nCellsValid - 1].area < minCellArea) {

        // FIXME possible error: the condition above does not
        // take into account the value of cellProp[].drop
        nCellsValid--;
    }

    /*reindex the map*/
    for (iCellValid = 0; iCellValid < nCellsValid; iCellValid++) {
        for (iGlobal = 0; iGlobal < nGlobal; iGlobal++) {
            // FIXME: inline changing of array elements...very flaky
            if (cellImage[iGlobal] == cellProp[iCellValid].index) {
                cellImage[iGlobal] = iCellValid + 1;
            }
        }
    }
    /*reindex the cellproperties object*/
    for (iCellValid = 0; iCellValid < nCells; iCellValid++) {
        cellProp[iCellValid].index = iCellValid + 1;
    }

    return nCellsValid;
} //updatemap






void classification(SCANMETA dbzMeta, SCANMETA vradMeta, SCANMETA rawReflMeta,
        SCANMETA clutterMeta, int *cellImage,
        unsigned char *dbzImage, unsigned char *vradImage,
        unsigned char *rawReflImage, unsigned char *clutterImage,
        float *zdata, int *nzdata,
        float *fracclut, float *fracrain, float *fracbird, float *fracfringe,
        float rangeMin, float rangeMax, float HLAYER, float XOFFSET,
        float XSCALE, float XMEAN, float height,
        float azimMin, float azimMax, float vradMin, float dbzClutter, float dbzMin,
        float dBZx, float DBZNOISE, int NGAPMIN, int NGAPBIN, int NDBZMIN,
        int layer, int id, int *np, int *nPointsPtr, int *nPointsAllPtr, int *nPointsClutterPtr,
        int *nPointsRainPtr, int *nPointsRainNoFringePtr,
        unsigned char clutterFlag, unsigned char rawReflFlag, unsigned char xflag) {


    //  *****************************************************************************
    //  This function classifies the range gates, distinguishing between clutter,
    //  rain, fringes, empty and valid gates. It returns the classification
    //  and layer counters
    //  *****************************************************************************

    // FIXME *nzdata unused
    // FIXME *fracclut  unused
    // FIXME *fracrain  unused
    // FIXME *fracbird  unused
    // FIXME *fracfringe  unused
    // FIXME HLAYER suggests preprocessor but isn't
    // FIXME XOFFSET suggests preprocessor but isn't
    // FIXME XSCALE suggests preprocessor but isn't
    // FIXME XMEAN suggests preprocessor but isn't
    // FIXME DBZNOISE suggests preprocessor but isn't
    // FIXME NGAPMIN suggests preprocessor but isn't
    // FIXME NGAPBIN suggests preprocessor but isn't
    // FIXME NDBZMIN suggests preprocessor but isn't
    // FIXME id unused
    // FIXME why not "SCANMETA*" (x4) instead of "SCANMETA'?

    int iAzim;
    int nAzim;
    int iRang;
    int nRang;
    int llayer;
    int n;
    int nPoints;
    int nPointsAll;
    int nPointsClutter;
    int nPointsRain;
    int nPointsRainNoFringe;
    int iGlobal;
    float range;
    float heightBeam;
    float azim;
    float FNAN;
    float dbzValue;
    float vradValue;
    float clutterValue;

    FNAN = 0.0/0.0;

    n = *np;                      // FIXME I think this information is already contained in iGlobal
    nPoints = *nPointsPtr;        // FIXME I think this information is already contained in iGlobal
    nPointsAll = *nPointsAllPtr;  // FIXME I think this information is already contained in iGlobal
    nPointsClutter = *nPointsClutterPtr;
    nPointsRain = *nPointsRainPtr;
    nPointsRainNoFringe = *nPointsRainNoFringePtr;


    fprintf(stderr, "nPointsRainNoFringe = %d\n",nPointsRainNoFringe);

    llayer = layer * NDATA;

    nRang = dbzMeta.nRang;
    nAzim = dbzMeta.nAzim;

    for (iRang = 0; iRang < nRang; iRang++) {

        for (iAzim = 0; iAzim < nAzim; iAzim++) {

            range = (iRang+0.5) * dbzMeta.rangeScale;
            azim = iAzim * dbzMeta.azimScale;    // FIXME why not iAzim+0.5?
            heightBeam = range * sin(dbzMeta.elev*DEG2RAD) + dbzMeta.heig;

            fprintf(stderr,"range = %f; azim = %f; heightBeam = %f\n",range, azim, heightBeam);

            if (range < rangeMin || range > rangeMax) {
                continue;
            }

            if (azim <= azimMin || azim > azimMax) {
                // FIXME what is this clause for?
                continue;
            }

            if (fabs(height-heightBeam) > 0.5*HLAYER) {
                continue;
            }


            iGlobal = iRang + iAzim * nRang;

            dbzValue = dbzMeta.valueScale*dbzImage[iGlobal] + dbzMeta.valueOffset;
            vradValue = vradMeta.valueScale*vradImage[iGlobal] + vradMeta.valueOffset;
            clutterValue = clutterMeta.valueScale*clutterImage[iGlobal] + clutterMeta.valueOffset;


            fprintf(stderr,"dbzValue = %f; vradValue = %f; clutterValue = %f\n",dbzValue, vradValue, clutterValue);

            n++;

            nPointsAll++;

            //cluttermap points:
            if (clutterFlag == 1){
                if (clutterValue > dbzClutter){
                    nPointsClutter++;
                    fprintf(stderr,"nPointsClutter = %d\n",nPointsClutter);
                    continue;
                }
            }

            //points without valid reflectivity data, but WITH raw reflectivity data are points
            //dropped by the signal preprocessor. These will be treated as clutter.
            if (rawReflFlag == 1){
                if (dbzImage[iGlobal] == dbzMeta.missing && rawReflImage[iGlobal] != rawReflMeta.missing){
                    nPointsClutter++;
                    continue;
                }
            }
            //if non-zero reflectivity but doppler data missing, treat as clutter:
            if (dbzImage[iGlobal] != dbzMeta.missing && vradImage[iGlobal] == vradMeta.missing){
                nPointsClutter++;
                continue;
            }

            if (dbzValue < dbzMin) {
                dbzValue = DBZNOISE;
            }

            //treat zero doppler speed as clutter:
            if (vradImage[iGlobal] != vradMeta.missing && fabs(vradValue) < vradMin){
                nPointsClutter++;
                continue;
            }

            // FIXME what does dBZx represent?
            if (cellImage[iGlobal] > 0 || dbzValue > dBZx) {
                if (cellImage[iGlobal] > 1) { //cluttermap without added fringes  // FIXME "cluttermap"? I think you mean cellmap
                    // FIXME what does "1+llayer" represent?
                    if (isnan(zdata[1+llayer])) {
                        zdata[1+llayer] = 0;
                    }
                    zdata[1+llayer] += exp(0.1*log(10)*dbzValue); // FIXME "log(10)" or "log10()"?
                    nPointsRainNoFringe++;
                }

                if (isnan(zdata[2+llayer])) {
                    // FIXME what does "2+llayer" represent?
                    zdata[2+llayer] = 0;
                }
                zdata[2+llayer] += exp(0.1*log(10)*dbzValue);  // FIXME "log(10)" or "log10()"?
                nPointsRain++;
                continue;
            }

            if (isnan(zdata[0+llayer])) {
                zdata[0+llayer] = 0;
            }

            if (isnan(zdata[2+llayer])) {
                zdata[2+llayer] = 0;
            }

            if (xflag==1) {
                zdata[0+llayer] += exp(0.1*log(10)*dbzValue)*XMEAN/(XOFFSET+XSCALE/range); // FIXME "log(10)" or "log10()"?
            }
            else {
                zdata[0+llayer] += exp(0.1*log(10)*dbzValue); // FIXME "log(10)" or "log10()"?
            }
            zdata[2+llayer] += exp(0.1*log(10)*dbzValue); // FIXME "log(10)" or "log10()"?
            nPoints++;
        }//for iAzim
    }//for iRang

    *np = n;
    *nPointsPtr = nPoints;
    *nPointsAllPtr = nPointsAll;
    *nPointsClutterPtr = nPointsClutter;
    *nPointsRainPtr = nPointsRain;
    *nPointsRainNoFringePtr = nPointsRainNoFringe;

    return;
}//classification





