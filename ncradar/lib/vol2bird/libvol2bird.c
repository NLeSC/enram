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


#define FPRINTFON (1)



int analyzeCells(const unsigned char *dbzImage, const unsigned char *vradImage,
        const unsigned char *texImage, const unsigned char *clutterImage, int *cellImage,
        const SCANMETA *dbzMeta, const SCANMETA *vradMeta, const SCANMETA *texMeta, const SCANMETA *clutterMeta,
        const int nCells, const int areaMin, const float cellDbzMin, const float cellStdDevMax, const float cellClutterFraction,
        const float vradMinValue, const float clutterValueMax, const unsigned char clutterFlag,
        const unsigned char verbose) {

    //  *********************************************************************************
    //  This function analyzes the cellImage array found by the 'findCells' procedure.
    //  Small cells are rejected and the cells are re-numbered according to size.
    //  The final number of cells in cellImage is returned as an integer.
    //  *********************************************************************************

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

    nCellsValid = 0;

    if (nCells == 0) {
        iGlobal = 0;
        for (iAzim = 0; iAzim < nAzim; iAzim++) {
            for (iRang = 0; iRang < nRang; iRang++) {
                cellImage[iGlobal] = -1;
                iGlobal++;
            }
        }
        return nCellsValid;
    }

    /*Allocating and initializing memory for cell properties.*/
    cellProp = (CELLPROP *)malloc(nCells*sizeof(CELLPROP));
    if (!cellProp) {
        printf("Requested memory could not be allocated!\n");
        return(-10);
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

            iCell = cellImage[iGlobal];

            if (iCell<0) {
                continue;
            }

            #ifdef FPRINTFON
            fprintf(stderr,"dbzValue = %f; vradValue = %f; clutterValue = %f; texValue = %f\n",dbzValue,vradValue,clutterValue,texValue);
            fprintf(stderr,"iGlobal = %d, iCell = %d\n",iGlobal,iCell);
            #endif

            cellProp[iCell].area += 1;

            //low radial velocities are treated as clutter, not included in calculation cell properties
            if (fabs(vradValue) < vradMinValue){
                // FIXME why fabs()?
                cellProp[iCell].clutterArea += 1;

                #ifdef FPRINTFON
                fprintf(stderr,"iGlobal = %d: vrad too low...treating as clutter\n",iGlobal);
                #endif

                continue;
            }

            //pixels in clutter map not included in calculation cell properties
            if (clutterFlag == 1){
                if (clutterValue > clutterValueMax){
                    cellProp[iCell].clutterArea += 1;
                    continue;
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
        } // for (iRang = 0; iRang < nRang; iRang++)
    } // for (iAzim = 0; iAzim < nAzim; iAzim++)


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
        if (cellProp[iCell].area < areaMin ||
            (cellProp[iCell].dbzAvg < cellDbzMin &&
             cellProp[iCell].texAvg > cellStdDevMax &&
             (cellProp[iCell].clutterArea / cellProp[iCell].area) < cellClutterFraction )) {
            // Terms 2,3 and 4 are combined with && to be conservative in labeling stuff as
            // bird migration --see discussion of issue #37 on GitHub.
            cellProp[iCell].drop = 1;
        }
    }

    /*Sorting cell properties according to cell area. Drop small cells from map*/
    // TODO FIXME updateMap() causes SEGFAULTs
    #ifdef FPRINTFON
    fprintf(stderr,"before the call to updateMap().\n");
    #endif

    nCellsValid = updateMap(cellImage,cellProp,nCells,nAzim*nRang,areaMin);

    #ifdef FPRINTFON
    fprintf(stderr,"after the call to updateMap().\n");
    #endif


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






void calcTexture(unsigned char *texImage, const unsigned char *vradImage,
        const unsigned char *dbzImage, const SCANMETA *texMeta, const SCANMETA *vradMeta,
        const SCANMETA *dbzMeta, const unsigned char nRangNeighborhood,
        const unsigned char nAzimNeighborhood, const unsigned char nCountMin) {


    //  ****************************************************************************************
    //  This function computes a texture parameter based on a block of (nRangNeighborhood x
    //  nAzimNeighborhood) pixels. The texture parameter equals the local standard deviation
    //  in the velocity field
    //  ****************************************************************************************


    int iRang;
    int iAzim;
    int nRang;
    int nAzim;
    int iNeighborhood;
    int nNeighborhood;
    int count;
    int missingValue;
    double vmoment1;
    double vmoment2;
    double dbz;
    double tex;
    int iGlobal;
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
    missingValue = vradMeta->missing; // FIXME this missingValue is used indiscriminately in vRad, tex and dbz alike

    dbzOffset = dbzMeta->valueOffset;
    dbzScale = dbzMeta->valueScale;

    vradOffset = vradMeta->valueOffset;
    vradScale = vradMeta->valueScale;

    texOffset = texMeta->valueOffset;
    texScale = texMeta->valueScale;

    nNeighborhood = nRangNeighborhood * nAzimNeighborhood;

    for (iAzim = 0; iAzim < nAzim; iAzim++) {
        for (iRang = 0; iRang < nRang; iRang++) {

            iGlobal = iRang + iAzim * nRang;

            /* count number of direct neighbours above threshold */
            count = 0;
            vmoment1 = 0;
            vmoment2 = 0;

            dbz = 0;

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

                tex = sqrt(XABS(vmoment2-SQUARE(vmoment1)));

                // FIXME maybe add safeguard against negative outcomes of ROUND((tex - texOffset) / texScale);
                texImage[iGlobal] = ROUND((tex - texOffset) / texScale);

                #ifdef FPRINTFON
                fprintf(stderr,
                        "\n(C) count = %d; nCountMin = %d; vmoment1 = %f; vmoment2 = %f; tex = %f; texBody[%d] = %d\n",
                        count, nCountMin, vmoment1, vmoment2, tex,
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
    float dbzValue;
    float vradValue;
    float clutterValue;

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






int findCells(const unsigned char *dbzImage, int *cellImage,
        const SCANMETA *dbzMeta,
        const float dbzThresMin,
        const float rCellMax) {

    //  *****************************************************************************
    //  This function detects the cells in 'dbzImage' using a threshold value of
    //  'dbzThresMin' and a non-recursive algorithm which looks for neighboring
    //  pixels above threshold. On return the marked cells are contained by
    //  'cellImage'. The number of detected cells/highest index value is returned.
    //  *****************************************************************************


    int iCellIdentifier;
    int nCells;
    int iRang;
    int nRang;
    int iAzim;
    int nAzim;
    int iNeighborhood;
    int nNeighborhood;
    int count;
    int cellImageInitialValue;

    int dbzThres;

    int iGlobal;
    int iGlobalOther;
    int nGlobal;
    int iLocal;

    int dbzMissing;
    int dbznAzim;
    int dbznRang;
    float dbzValueOffset;
    float dbzValueScale;
    float dbzRangeScale;

    int nAzimNeighborhood;
    int nRangNeighborhood;
    int nHalfNeighborhood;

    int cellIdentifierGlobal;
    int cellIdentifierGlobalOther;
    int iGlobalInner;


    #ifdef FPRINTFON
    int dbg = 0;
    #endif

    dbzMissing = dbzMeta->missing;
    dbznAzim = dbzMeta->nAzim;
    dbznRang = dbzMeta->nRang;
    dbzValueOffset = dbzMeta->valueOffset;
    dbzValueScale = dbzMeta->valueScale;
    dbzRangeScale = dbzMeta->rangeScale;

    nAzim = dbznAzim;
    nRang = dbznRang;

    nGlobal = nAzim * nRang;
    nAzimNeighborhood = 3;
    nRangNeighborhood = 3;
    nNeighborhood = nAzimNeighborhood * nRangNeighborhood;
    nHalfNeighborhood = (nNeighborhood - 1)/2;


    if (dbzImage != NULL) {
        dbzThres = ROUND((dbzThresMin - dbzValueOffset) / dbzValueScale);
    }

    /*Initializing of connection cellImage.*/
    cellImageInitialValue = -1;
    for (iGlobal = 0; iGlobal < nGlobal; iGlobal++) {
        cellImage[iGlobal] = cellImageInitialValue;
    }

    /*If threshold value is equal to missing value, return.*/
    if (dbzThres == dbzMissing) {
        return -1;
    }

    /*Labeling of groups of connected pixels using horizontal, vertical, and */
    /*diagonal connections. The algorithm is described in 'Digital Image */
    /*Processing' by Gonzales and Woods published by Addison-Wesley.*/

    iCellIdentifier = 0;

    for (iAzim = 0; iAzim < nAzim; iAzim++) {
        for (iRang = 0; iRang < nRang; iRang++) {

            iGlobal = iRang + iAzim * nRang;

            if ((iRang + 1) * dbzRangeScale > rCellMax) {
                // FIXME the left hand side of the condition above is a distance; the
                // right hand side's data type suggests a number of array elements
                continue;
            }
            else {
                #ifdef FPRINTFON
                fprintf(stderr, "iGlobal = %d\niRang + 1 = %d\ndbzRangeScale = %f\nrCellMax = %f\n(iRang + 1) * dbzRangeScale = %f\n((iRang + 1) * dbzRangeScale > rCellMax) = %d\ndbg=%d\n",iGlobal,iRang + 1,dbzRangeScale,rCellMax,(iRang + 1) * dbzRangeScale,((iRang + 1) * dbzRangeScale > rCellMax),dbg);
                dbg++;
                #endif
            }

            #ifdef FPRINTFON
            fprintf(stderr,"iGlobal = %d\n",iGlobal);
            #endif

            if (dbzImage[iGlobal] == dbzMissing) {

                #ifdef FPRINTFON
                fprintf(stderr,"dbzImage[%d] == dbzMissing\n",iGlobal);
                #endif

                continue;
            }

            if (dbzImage[iGlobal] < dbzThres) {
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
                if (dbzImage[iLocal] > dbzThres) {
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


            /*Looking for horizontal, vertical, forward diagonal, and backward diagonal */
            /*connections.*/

            for (iNeighborhood = 0; iNeighborhood < nHalfNeighborhood; iNeighborhood++) {

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


    // check whether a cell crosses the border of the array (remember that iAzim=0 is
    // adjacent to iAzim=nAzim-1):
    iAzim = 0;

    for (iRang = 0; iRang < nRang; iRang++) {

        iGlobal = iRang + iAzim * nRang;
        // index 1 in a 3x3 child array refers to the cell that is a direct neighbor of
        // iGlobal, but on the other side of the array (because the polar plot is wrapped
        // in the azimuth dimension):
        iGlobalOther = findNearbyGateIndex(nAzim,nRang,iGlobal,3,3,1);

        #ifdef FPRINTFON
        fprintf(stderr,"iGlobal = %d, iGlobalOther = %d\n",iGlobal,iGlobalOther);
        #endif

        cellIdentifierGlobal = cellImage[iGlobal];
        cellIdentifierGlobalOther = cellImage[iGlobalOther];
        if (cellIdentifierGlobal != cellImageInitialValue && cellIdentifierGlobalOther != cellImageInitialValue ) {
            // adjacent gates, both part of a cell -> assign them the same identifier, i.e. assign
            // all elements of cellImage that are equal to cellImage[iGlobalOther] the value of
            // cellImage[iGlobal]

            for (iGlobalInner = 0; iGlobalInner < nGlobal; iGlobalInner++) {
                if (cellImage[iGlobalInner] == cellIdentifierGlobalOther) {
                    cellImage[iGlobalInner] = cellIdentifierGlobal;
                }
            }
        }


    }



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








void fringeCells(int *cellImage, int nRang, int nAzim, float aScale, float rScale, float fringeDist) {

    //  ******************************************************************************
    //  This function enlarges cellImage by additional fringe.
    //  First a block around each pixel is searched for pixels within a distance
    //  equal to 'fringeDist'.
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
    int edge;  // FIXME isEdge would be better, also define TRUE and FALSE in preprocessor
    int iGlobal;
    float theDist;
    int nAzimChild;
    int nRangChild;

    float actualRange;
    float circumferenceAtActualRange;


    rBlock = ROUND(fringeDist / rScale);
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
            for (iNeighborhood = 0; iNeighborhood < 9 && !edge; iNeighborhood++) {

                iLocal = findNearbyGateIndex(nAzim,nRang,iGlobal,3,3,iNeighborhood);

                if (iLocal < 0) {
                    // iLocal less than zero are error codes
                    continue;
                }

                if (cellImage[iLocal] < 1) {
                    edge = 1; //FIXME THIS SHOULD BE <=1, BUT THEN TAKES MUCH MUCH LONGER
                }

            } //NOW ONLY CELL PIXELS WITHOUT ANY BORDERING FRINGE ARE 'FRINGED'

            if (!edge) {  // FIXME  edge == 0 ?
                continue;
            }

            actualRange = (iRang+0.5) * rScale;
            circumferenceAtActualRange = 2 * PI * actualRange;
            aBlock = (fringeDist / circumferenceAtActualRange) * nAzim;


            #ifdef FPRINTFON
            fprintf(stderr, "actualRange = %f\n",actualRange);
            fprintf(stderr, "circumferenceAtActualRange = %f\n",circumferenceAtActualRange);
            fprintf(stderr, "fringeDist / circumferenceAtActualRange = %f\n",fringeDist / circumferenceAtActualRange);
            fprintf(stderr, "aBlock = %d\n", aBlock);
            fprintf(stderr, "rBlock = %d\n", rBlock);
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
                if (theDist > fringeDist || cellImage[iLocal] >= 1) {
                    continue;
                }
                //include pixel (iRangLocal,iAzimLocal) in fringe
                cellImage[iLocal] = 1; //assign index 1 to fringes
            }
        }
    } //for
    return;
} // fringeCells






void sortCells(CELLPROP *cellProp, int nCells) {


    //  *****************************************************************************
    //  Sorting of the cell properties using cell area.
    //  Assume an area equal to zero for cells that are marked 'dropped'
    //  *****************************************************************************

    int iCell;
    int iCellOther;
    CELLPROP tmp;

    /*Sorting of data elements using straight insertion method.*/
    for (iCell = 1; iCell < nCells; iCell++) {

        tmp = cellProp[iCell];

        iCellOther = iCell - 1;

        while (iCellOther >= 0 && cellProp[iCellOther].area * XABS(cellProp[iCellOther].drop - 1) < tmp.area * XABS(tmp.drop - 1)) {

            cellProp[iCellOther + 1] = cellProp[iCellOther];

            iCellOther--;
        }

        cellProp[iCellOther + 1] = tmp;

    } //for iCell

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


    #ifdef FPRINTFON
    fprintf(stderr,"before the call to sortCells().\n");
    #endif

    /*Sort the cells by area and determine number of valid cells*/
    sortCells(cellProp, nCells);

    #ifdef FPRINTFON
    fprintf(stderr,"after the call to sortCells().\n");
    #endif


    #ifdef FPRINTFON
    fprintf(stderr,"nCellsValid = %d.\n",nCellsValid);
    if (nCellsValid > 0) {
        fprintf(stderr,"cellProp[nCellsValid - 1].area = %f.\n",cellProp[nCellsValid - 1].area);
        fprintf(stderr,"cellProp[nCellsValid - 1].drop = %d.\n",cellProp[nCellsValid - 1].drop);
        fprintf(stderr,"minCellArea = %d.\n",minCellArea);
    }
    #endif


    while (nCellsValid > 0 && cellProp[nCellsValid - 1].area < minCellArea) {

        // FIXME possible error: the condition above does not
        // take into account the value of cellProp[].drop
        nCellsValid--;

        #ifdef FPRINTFON
        fprintf(stderr,"nCellsValid = %d.\n",nCellsValid);
        #endif
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
        // re-index the cellproperties object
        cellProp[iCell].index = iCellNew;
    }

    return nCellsValid;
} //updateMap






