/*
 * Copyright 2013 Netherlands eScience Center
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "libvol2bird.h"


// #define FPRINTFON (1)



int analyzeCells(unsigned char *dbzImage,unsigned char *vradImage,
                 unsigned char *texImage, unsigned char *clutterImage, int *cellImage,
                 SCANMETA *dbzMeta, SCANMETA *vradMeta, SCANMETA *texMeta, SCANMETA *clutterMeta,
                 int nCells, int areaMin, float cellDbzMin, float cellStdDevMax, float cellClutterFraction,
                 float vradMinValue,float dbzClutterMin, unsigned char clutterFlag,
                 unsigned char dualPolFlag, unsigned char verbose) {

    //  *********************************************************************************
    //  This function analyses the cellImage array found by the 'findcells' procedure.
    //  Smalls cells are rejected and the cells are re-numbered according to size.
    //  The final number of cells in cellImage is returned as an integer.
    //  *********************************************************************************

#ifdef FPRINTFON
    fprintf(stderr,"Begin of analyzeCells in C.\n");
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
        return(10);
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
            if (clutterFlag == 1){
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
                cellProp[iCell].iRangOfMax = iGlobal%nRang;
                cellProp[iCell].iAzimOfMax = iGlobal/nRang;
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
            cellProp[iCell].cv = cellProp[iCell].texAvg / cellProp[iCell].dbzAvg;
        }
    }

    /*Determine which cells to drop from map based on low mean dBZ / high stdev /
     * small area / high percentage clutter*/
    for (iCell = 0; iCell < nCells; iCell++) {
        if (dualPolFlag == 1){  // FIXME can I delete all dual polarity related stuff from
                                // FIXME vol2bird...seems like we're not using any of that at the moment
            if (cellProp[iCell].area < areaMin) {
                cellProp[iCell].drop = 1;
            }
        }
        else {
            if (cellProp[iCell].area < areaMin ||
                (cellProp[iCell].dbzAvg < cellDbzMin &&
                 cellProp[iCell].texAvg > cellStdDevMax &&
                 (cellProp[iCell].clutterArea / cellProp[iCell].area) < cellClutterFraction )) {
                // Terms 2,3 and 4 are combined with && to be conservative in labeling stuff as
                // bird migration --see discussion of issue #37 on GitHub.
                cellProp[iCell].drop = 1;
            }
        }
    }

    /*Sorting cell properties according to cell area. Drop small cells from map*/
    nCellsValid = updateMap(cellImage,cellProp,nCells,nAzim*nRang,areaMin);

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
    fprintf(stderr,"End of analyzeCells in C.\n");
#endif


    return nCellsValid;
} // analyzeCells







float calcDist(int iRang1, int iAzim1, int iRang2, int iAzim2, float rangScale, float azimScaleDeg) {

    //  ******************************************************************************
    //  This function calculates the distance in km between two gates
    //  ******************************************************************************

    float range1;
    float range2;
    float azimuth1;
    float azimuth2;

    range1 = iRang1 * rangScale;
    range2 = iRang2 * rangScale;

    azimuth1 = iAzim1 * azimScaleDeg * DEG2RAD;
    azimuth2 = iAzim2 * azimScaleDeg * DEG2RAD;

    return sqrt(pow(range1,2) +
                pow(range2,2) -
                2 * (range1 * range2) * cos(azimuth1-azimuth2));

} // calcDist






void calcTexture(unsigned char *texImage, unsigned char *vradImage,
        unsigned char *dbzImage, SCANMETA *texMeta, SCANMETA *vradMeta,
        SCANMETA *dbzMeta, unsigned char nRangNeighborhood,
        unsigned char nAzimNeighborhood, unsigned char nCountMin,
        unsigned char texType) {


    //  ****************************************************************************************
    //  This function computes a texture parameter based on a block of (nRangNeighborhood x
    //  nAzimNeighborhood) pixels. The texture parameter equals the local standard deviation
    //  in the velocity field (when texType==STDEV).
    //  ****************************************************************************************


    int iRang;
    int iAzim;
    int nRang;
    int nAzim;
    int iNeighborhood;
    int nNeighborhood;
    int count;
    int missingValue;
    int value;
    int index;
    double vmoment1;
    double vmoment2;
    double dbz;
    double tex;
    int iGlobal;
    int nGlobal;
    int iLocal;
    float texOffset;
    float texScale;
    float dbzOffset;
    float dbzScale;
    float vradOffset;
    float vradScale;
    float vRadDiff;

    nRang = vradMeta->nRang;
    nAzim = vradMeta->nAzim;
    nGlobal = nAzim * nRang;
    missingValue = vradMeta->missing; // FIXME this missingValue is used indiscriminately in vRad, tex and dbz alike

    dbzOffset = dbzMeta->valueOffset;
    dbzScale = dbzMeta->valueScale;

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

            dbz = dbzOffset + dbzScale * dbzImage[iGlobal]; // FIXME issue #44

            for (iNeighborhood = 0; iNeighborhood < nNeighborhood; iNeighborhood++) {

                iLocal = findNearbyGateIndex(nAzim,nRang,iGlobal,nAzimNeighborhood,nRangNeighborhood,iNeighborhood);

#ifdef FPRINTFON
                fprintf(stderr, "iLocal = %d; ",iLocal);
#endif

                if (iLocal < 0) {
                    // iLocal less than zero are error codes
                    continue;
                }

                if (vradImage[iLocal] == missingValue || dbzImage[iLocal] == missingValue) {
                    continue;
                }

                vRadDiff = vradOffset + vradScale * (vradImage[iGlobal] - vradImage[iLocal]);
                vmoment1 += vRadDiff;
                vmoment2 += SQUARE(vRadDiff);

                // FIXME double counted dbz when iLocal == iGlobal
                dbz += dbzOffset + dbzScale * dbzImage[iLocal];

                count++;

            }

            vmoment1 /= count;
            vmoment2 /= count;
            dbz /= count;

            /* when not enough neighbours, continue */
            if (count < nCountMin) {
                texImage[iGlobal] = missingValue;
            }
            else {
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

#ifdef FPRINTFON
                fprintf(stderr,
                        "\n(C) count = %d; nCountMin = %d; texType = %d; vmoment1 = %f; vmoment2 = %f; tex = %f; texBody[%d] = %d\n",
                        count, nCountMin, texType, vmoment1, vmoment2, tex,
                        iGlobal, texImage[iGlobal]);
#endif

            } //else
        } //for
    } //for
} // calcTexture



void calcVvp(SCANMETA vradMeta, unsigned char *vradImage, float *points, float *yObs,
        int *c, int *cellImage, int nDims, int *nPointsMaxPtr, int NGAPBIN,
        float rangeMin, float rangeMax, float HLAYER, float heightInputPar,
        float vradMin, int iData, int layer, int id, int *nPoints)
{

    // FIXME this function's name suggest that the vvp analysis takes place in its body, but
    // that is not the case -- the function merely handles the selection of valid gates, which
    // are then supposedly passed on to svdfit()
    // FIXME heightInputPar could be the height for which you want to calculate the VVP
    // FIXME NGAPBIN not used
    // FIXME id not used
    // FIXME HLAYER suggests preprocessor but is not
    // FIXME NGAPBIN suggest preprocessor but is not

    /******************************************************************************/
    /* This function computes the wind velocity components                        */
    /******************************************************************************/

    int iAzim;
    int llayer;
    int iRang;
    int iPoint;
    int nPointsMax;    // FIXME nothing really happens to either nPointsMax or nPointsMaxPtr.. maybe delete altogether?
    float gateHeight;
    float gateRange;
    float gateAzim;
    int nRang;
    int nAzim;
    float rangeScale;
    float azimuthScale;
    float elevAngle;
    int missing;
    float radarHeight;
    int iGlobal;
    float valueOffset;
    float valueScale;

    iPoint = *nPoints;
    nPointsMax = *nPointsMaxPtr;

    llayer = layer * NDATA;  // FIXME variable remains unused

    nRang = vradMeta.nRang;
    nAzim = vradMeta.nAzim;
    rangeScale = vradMeta.rangeScale;
    azimuthScale = vradMeta.azimScale;
    elevAngle = vradMeta.elev;
    missing = vradMeta.missing;
    radarHeight = vradMeta.heig;
    valueOffset = vradMeta.valueOffset;
    valueScale = vradMeta.valueScale;


    for (iRang = 0; iRang < nRang; iRang++) {

        // so gateRange represents a distance along the view direction (not necessarily horizontal)
        gateRange = (iRang + 0.5) * rangeScale;

        // note that "sin(elevAngle*DEG2RAD)" is equivalent to = "cos((90 - elevAngle)*DEG2RAD)":
        gateHeight = gateRange * sin(elevAngle*DEG2RAD) + radarHeight;

        if (gateRange < rangeMin || gateRange > rangeMax) {
            continue;
        }
        if (fabs(heightInputPar-gateHeight) > 0.5*HLAYER) {
            // FIXME what does this if statement mean?  Maybe "if the height of the current gate
            // is too far away from the requested height, continue with the next gate"?
            continue;
        }


        for (iAzim = 0; iAzim < nAzim; iAzim++) {

            iGlobal = iRang + iAzim * nRang;
            gateAzim = (iAzim + 0.5) * azimuthScale;

            if (vradImage[iGlobal] == missing) {
                continue;
            }

            switch (iData) {
            case 0:
                if (cellImage[iGlobal]>0) {
                    continue; // outside rain clutter map only
                }
                break;
            case 1:
                if (cellImage[iGlobal]<2) {
                    continue; // inside rain clutter map without fringe only
                }
                break;
            }

            // so at this point we've checked a couple of things and we see no reason
            // why vRadImage[iGlobal] shouldn't be part of the points array

            points[iPoint * nDims + 0] = gateAzim;
            points[iPoint * nDims + 1] = elevAngle;
            if (nDims > 2) {
                points[iPoint * nDims + 2] = gateRange;
            }
            if (nDims > 3) {
                points[iPoint * nDims + 3] = heightInputPar-radarHeight;
            }
            yObs[iPoint] = valueScale * vradImage[iGlobal] + valueOffset;
            c[iPoint] = cellImage[iGlobal];

            if (fabs(yObs[iPoint]) >= vradMin) {
                // FIXME why fabs?
                // FIXME at this point, you've already added the point. What if the condition is false, but there
                // are no more gates to overwrite points[iPoint+whatever], yObs[iPoint] and c[iPoint] later?
                iPoint++;
            }
        }  //for iAzim
    } //for iRang

    *nPoints = iPoint;
    *nPointsMaxPtr = nPointsMax;

#ifdef FPRINTFON
            //for (iPoint = 0; iPoint < nPointsMax; iPoint++) {
            fprintf(stderr, "points[0] = %f\n",points[0]);
            //}
#endif


} //calcVvp




void classify(SCANMETA dbzMeta, SCANMETA vradMeta, SCANMETA rawReflMeta,
        SCANMETA clutterMeta, int *cellImage,
        unsigned char *dbzImage, unsigned char *vradImage,
        unsigned char *rawReflImage, unsigned char *clutterImage,
        float *zdata,
        float rangeMin, float rangeMax, float HLAYER, float XOFFSET,
        float XSCALE, float XMEAN, float height,
        float azimMin, float azimMax, float vradMin, float dbzClutter, float dbzMin,
        float dBZx, float DBZNOISE, int NGAPMIN, int NGAPBIN, int NDBZMIN,
        int layer, int *np, int *nPointsPtr, int *nPointsAllPtr, int *nPointsClutterPtr,
        int *nPointsRainPtr, int *nPointsRainNoFringePtr,
        unsigned char clutterFlag, unsigned char rawReflFlag, unsigned char xflag) {


    //  *****************************************************************************
    //  This function classifies the range gates, distinguishing between clutter,
    //  rain, fringes, empty and valid gates. It returns the classification
    //  and layer counters
    //  *****************************************************************************

    // FIXME regarding the 'classify' method name...classify what?

    // FIXME HLAYER suggests preprocessor but isn't
    // FIXME XOFFSET suggests preprocessor but isn't
    // FIXME XSCALE suggests preprocessor but isn't
    // FIXME XMEAN suggests preprocessor but isn't
    // FIXME DBZNOISE suggests preprocessor but isn't
    // FIXME NGAPMIN suggests preprocessor but isn't
    // FIXME NGAPBIN suggests preprocessor but isn't
    // FIXME NDBZMIN suggests preprocessor but isn't
    // FIXME id unused
    // FIXME why not "SCANMETA*" (x4) instead of "SCANMETA" (x4)?

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

#ifdef FPRINTFON
    fprintf(stderr, "nPointsRainNoFringe = %d\n",nPointsRainNoFringe);
#endif

    llayer = layer * NDATA;

    nRang = dbzMeta.nRang;
    nAzim = dbzMeta.nAzim;

    for (iRang = 0; iRang < nRang; iRang++) {

        for (iAzim = 0; iAzim < nAzim; iAzim++) {

            range = (iRang+0.5) * dbzMeta.rangeScale;
            // FIXME equivalent line in ~/enram/doc/vol2bird-adriaans-version-20140716/vol2birdprof_h5.c is 491
            azim = iAzim * dbzMeta.azimScale;                                    // FIXME why not iAzim+0.5?
            heightBeam = range * sin(dbzMeta.elev*DEG2RAD) + dbzMeta.heig;

#ifdef FPRINTFON
            fprintf(stderr,"range = %f; azim = %f; heightBeam = %f\n",range, azim, heightBeam);
#endif

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

#ifdef FPRINTFON
            fprintf(stderr,"dbzValue = %f; vradValue = %f; clutterValue = %f\n",dbzValue, vradValue, clutterValue);
#endif

            n++;

            nPointsAll++;

            //cluttermap points:
            if (clutterFlag == 1){
                if (clutterValue > dbzClutter){
                    nPointsClutter++;
#ifdef FPRINTFON
                    fprintf(stderr,"nPointsClutter = %d\n",nPointsClutter);
#endif
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
                    // FIXME what does "1+llayer" represent? ANSWER: pseudocolumn 1 in zdata
                    if (isnan(zdata[1+llayer])) {
                        zdata[1+llayer] = 0;
                    }
                    zdata[1+llayer] += exp(0.1*log(10)*dbzValue);
                    nPointsRainNoFringe++;
                }

                if (isnan(zdata[2+llayer])) {
                    // FIXME what does "2+llayer" represent?  ANSWER: pseudocolumn 2 in zdata
                    zdata[2+llayer] = 0;
                }
                zdata[2+llayer] += exp(0.1*log(10)*dbzValue);
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
                zdata[0+llayer] += exp(0.1*log(10)*dbzValue)*XMEAN/(XOFFSET+XSCALE/range);
            }
            else {
                zdata[0+llayer] += exp(0.1*log(10)*dbzValue);
            }
            zdata[2+llayer] += exp(0.1*log(10)*dbzValue);
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
} // classify






int findCells(unsigned char *texImage, unsigned char *rhoImage,
        unsigned char *zdrImage, int *cellImage, SCANMETA *texMeta,
        SCANMETA *rhoMeta, SCANMETA *zdrMeta, float texThresMin,
        float rhoThresMin, float zdrThresMin, float dbzThresMin,
        float rCellMax, char sign) {

    // FIXME see issue #48


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
    int nNeighborhood;
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

    int nAzimNeighborhood;
    int nRangNeighborhood;

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
    nAzimNeighborhood = 3;
    nRangNeighborhood = 3;
    nNeighborhood = nAzimNeighborhood * nRangNeighborhood;


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
                for (iNeighborhood = 0; iNeighborhood < nNeighborhood; iNeighborhood++) {

                    iLocal = findNearbyGateIndex(nAzim,nRang,iGlobal,nAzimNeighborhood,nRangNeighborhood,iNeighborhood);

                    if (iLocal < 0) {
                        // iLocal below zero are error codes
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
                if (texImage[iGlobal] < dbzThresMin) { // FIXME tex v refl why?   // FIXME see issue #48, also: might be dbzThres
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

                for (iNeighborhood = 0; iNeighborhood < nNeighborhood; iNeighborhood++) {

                    iLocal = findNearbyGateIndex(nAzim,nRang,iGlobal,nAzimNeighborhood,nRangNeighborhood,iNeighborhood);

                    if (iLocal < 0) {
                        // iLocal less than zero are error codes
                        continue;
                    }

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

                iLocal = findNearbyGateIndex(nAzim,nRang,iGlobal,nAzimNeighborhood,nRangNeighborhood,iNeighborhood);

                if (iLocal < 0) {
                    // iLocal less than zero are error codes
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
} // findCells






int findNearbyGateIndex(const int nAzimParent, const int nRangParent, const int iParent,
                        const int nAzimChild,  const int nRangChild,  const int iChild) {



    if (nRangChild%2 != 1) {
#ifdef FPRINTFON
        fprintf(stderr, "nRangChild must be an odd integer number.\n");
#endif
        return -1;
    }

    if (nAzimChild%2 != 1) {
#ifdef FPRINTFON
        fprintf(stderr, "nAzimChild must be an odd integer number.\n");
#endif
        return -2;
    }

    if (iChild > nAzimChild * nRangChild - 1) {
#ifdef FPRINTFON
        fprintf(stderr, "iChild is outside the child window.\n");
#endif
        return -3;
    }


    const int iAzimParent = iParent / nRangParent;
    const int iRangParent = iParent % nRangParent;

    const int iAzimChild = iChild / nRangChild;
    const int iRangChild = iChild % nRangChild;

    // the azimuth dimension is wrapped (polar plot); iAzim = 0 is adjacent to iAzim = nAzim-1:
    const int iAzimReturn = (iAzimParent - nAzimChild/2 + iAzimChild + nAzimParent) % nAzimParent;
    const int iRangReturn = iRangParent - nRangChild/2 + iRangChild;


    if (iRangReturn > nRangParent - 1) {
#ifdef FPRINTFON
        fprintf(stderr, "iChild is outside the parent array on the right-hand side.\n");
#endif
        return -4;
    }
    if (iRangReturn < 0) {
#ifdef FPRINTFON
        fprintf(stderr, "iChild is outside the parent array on the left-hand side.\n");
#endif
        return -5;
    }

    return iAzimReturn * nRangParent + iRangReturn;

} // findNearbyGateIndex








void fringeCells(int *cellImage, int nRang, int nAzim, float aScale,
        float rScale, float fringe) {

    //  ******************************************************************************
    //  This function enlarges cellImage by additional fringe.
    //  First a block around each pixel is searched for pixels within a distance
    //  equal to 'fringe'.
    //  ******************************************************************************


    int iRang;
    int iAzim;
    int iNeighborhood;
    int nNeighborhood;
    int iRangLocal;
    int iAzimLocal;
    int iLocal;
    int rBlock;
    int aBlock;
    int edge;
    int iGlobal;
    float tmp;
    float theDist;
    int nAzimChild;
    int nRangChild;


    rBlock = ROUND(fringe / rScale);
    for (iAzim = 0; iAzim < nAzim; iAzim++) {
        for (iRang = 0; iRang < nRang; iRang++) {

            iGlobal = iRang + iAzim * nRang;

            if (cellImage[iGlobal] <= 1) {
#ifdef FPRINTFON
                fprintf(stderr, "iGlobal = %d; cellImage[iGlobal] = %d: skip\n", iGlobal, cellImage[iGlobal]);
#endif
                continue; //already fringe or not in cellImage
            }
#ifdef FPRINTFON
            fprintf(stderr, "iGlobal = %d; cellImage[iGlobal] = %d: pass\n", iGlobal, cellImage[iGlobal]);
#endif

            //determine whether current pixel is a pixel on the edge of a cell
            edge = 0;
            for (iNeighborhood = 0; iNeighborhood < 9 && !edge;
                    iNeighborhood++) {

                iLocal = findNearbyGateIndex(nAzim,nRang,iGlobal,3,3,iNeighborhood);

                if (iLocal < 0) {
                    // iLocal less than zero are error codes
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

#ifdef FPRINTFON
            fprintf(stderr, "aBlock = %d; rBlock = %d\n", aBlock, rBlock);
#endif

            nAzimChild = 2 * aBlock + 1;
            nRangChild = 2 * rBlock + 1;
            nNeighborhood = nAzimChild * nRangChild;

            for (iNeighborhood = 0; iNeighborhood < nNeighborhood; iNeighborhood++) {

                iLocal = findNearbyGateIndex(nAzim,nRang,iGlobal,nAzimChild,nRangChild,iNeighborhood);

                if (iLocal < 0) {
                    // iLocal less than zero are error codes
                    continue;
                }

                iAzimLocal = iLocal / nRang;
                iRangLocal = iLocal % nRang;

                //if not within range or already in cellImage or already a fringe, do nothing
                theDist = calcDist(iRang, iAzim, iRangLocal, iAzimLocal, rScale, aScale);
                if (theDist > fringe || cellImage[iLocal] >= 1) {
                    continue;
                }
                //include pixel (iRangLocal,iAzimLocal) in fringe
                cellImage[iLocal] = 1; //assign index 1 to fringes
            }
        }
    } //for
    return;
} // fringeCells






void sortCells(CELLPROP *cellProp, int nCells, int method) {


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

            while (iCellOther >= 0 && cellProp[iCellOther].area * XABS(cellProp[iCellOther].drop - 1) < tmp.area * XABS(tmp.drop - 1)) {

                cellProp[iCellOther + 1] = cellProp[iCellOther];

                iCellOther--;
            }

            cellProp[iCellOther + 1] = tmp;

        } //for iCell
    } else if (method == BYMEAN) {
        for (iCell = 1; iCell < nCells; iCell++) {

            tmp = cellProp[iCell];

            iCellOther = iCell - 1;

            while (iCellOther >= 0 && cellProp[iCellOther].dbzAvg * XABS(cellProp[iCellOther].drop - 1) < tmp.dbzAvg * XABS(tmp.drop - 1)) {

                cellProp[iCellOther + 1] = cellProp[iCellOther];

                iCellOther--;
            }

            cellProp[iCellOther + 1] = tmp;

        } //for iCell
    }

    return;
} // sortCells





int updateMap(int *cellImage, CELLPROP *cellProp, int nCells, int nGlobal, int minCellArea) {

    //  *****************************************************************************
    //  This function updates the cellImage by dropping cells and reindexing the map
    //  Leaving index 0 unused, will be used for assigning cell fringes
    //  *****************************************************************************


    int iGlobal;
    int iCell;
    int iCellNew;
    int nCellsValid;
    int cellImageValue;
    int cellImageOld[nGlobal];

    nCellsValid = nCells;

    for (iGlobal = 0; iGlobal < nGlobal; iGlobal++) {

        if (cellImage[iGlobal] == -1) {
            continue;
        }
        // FIXME this may be a bug: the line below reads an identifier number
        // from cellImage, which is then used as an index into cellProp[].
        // I imagine the cellProp array starts at index 0, which should contain
        // the properties of the cell which is identifiable in cellImage by
        // its value...2!  (0 and 1 are reserved for other stuff it seems)

        cellImageValue = cellImage[iGlobal];

        if (cellImageValue > nCells - 1) {

#ifdef FPRINTFON
            fprintf(stderr, "You just asked for the properties of cell %d, which does not exist.\n", cellImageValue);
#endif
            continue;
        }

        if (cellProp[cellImageValue].drop == 1) {

            cellImage[iGlobal] = -1;
        }
    }

    /*Sort the cells by area and determine number of valid cells*/
    sortCells(cellProp, nCells, BYAREA);

    while (nCellsValid > 0 && cellProp[nCellsValid - 1].area < minCellArea) {

        // FIXME possible error: the condition above does not
        // take into account the value of cellProp[].drop
        nCellsValid--;
    }

    // make a copy of the cellImage map
    for (iGlobal = 0; iGlobal < nGlobal; iGlobal++) {
        cellImageOld[iGlobal] = cellImage[iGlobal];
    }
    // re-index the map
    for (iCell = 0; iCell < nCells; iCell++) {

        if (iCell < nCellsValid) {
            iCellNew = iCell + 1;
        }
        else {
            iCellNew = -1;
        }

        for (iGlobal = 0; iGlobal < nGlobal; iGlobal++) {
            if (cellImageOld[iGlobal] == cellProp[iCell].index) {
                cellImage[iGlobal] = iCellNew;
            }
        }
    }

    for (iCell = 0; iCell < nCells; iCell++) {

        if (iCell < nCellsValid) {
            iCellNew = iCell + 1;
        }
        else {
            iCellNew = -1;
        }

        // re-index the cellproperties object
        cellProp[iCell].index = iCellNew;
    }

    return nCellsValid;
} //updateMap






