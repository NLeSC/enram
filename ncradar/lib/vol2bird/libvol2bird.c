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

//#define FPRINTFON (1)
#include "libvol2bird.h"




int analyzeCells(const unsigned char *dbzImage, const unsigned char *vradImage,
        const unsigned char *texImage, const unsigned char *clutterImage, int *cellImage,
        const SCANMETA *dbzMeta, const SCANMETA *vradMeta, const SCANMETA *texMeta, const SCANMETA *clutterMeta,
        const int nCells, const int areaMin, const float cellDbzMin, const float cellStdDevMax, const float cellClutterFraction,
        const float absVradMin, const float clutterValueMax, const unsigned char clutterFlag,
        const unsigned char verbose) {

    //  *********************************************************************************
    //  This function analyzes the cellImage array found by the 'findCells' procedure.
    //  Small cells are rejected and the cells are re-numbered according to size.
    //  The final number of cells in cellImage is returned as an integer.
    //  *********************************************************************************


    CELLPROP *cellProp;
    int iCell;
    int iGlobal;
    int nGlobal;
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
    nGlobal = nAzim*nRang;
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
        return -10;
    }
    for (iCell = 0; iCell < nCells; iCell++) {
        cellProp[iCell].iRangOfMax = -1;
        cellProp[iCell].iAzimOfMax = -1;
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

            dbzValue = dbzMeta->valueScale * (float) dbzImage[iGlobal] + dbzMeta->valueOffset;
            vradValue = vradMeta->valueScale * (float) vradImage[iGlobal] + vradMeta->valueOffset;
            clutterValue = clutterMeta->valueScale * (float) clutterImage[iGlobal] + clutterMeta->valueOffset;
            texValue = texMeta->valueScale * (float) texImage[iGlobal] + texMeta->valueOffset;

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
            if (fabs(vradValue) < absVradMin){

                cellProp[iCell].clutterArea += 1;

                #ifdef FPRINTFON
                fprintf(stderr,"iGlobal = %d: vrad too low...treating as clutter\n",iGlobal);
                #endif

                continue;
            }

            //pixels in clutter map not included in calculation cell properties
            if ((int) clutterFlag == 1){
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
             ((float) cellProp[iCell].clutterArea / cellProp[iCell].area) < cellClutterFraction )) {
            // Terms 2,3 and 4 are combined with && to be conservative in labeling stuff as
            // bird migration --see discussion of issue #37 on GitHub.
            cellProp[iCell].drop = 1;
            cellProp[iCell].area = 0;
        }
    }

    /*Sorting cell properties according to cell area. Drop small cells from map*/
    nCellsValid = updateMap(cellImage,nGlobal,cellProp,nCells,areaMin);



    //Printing of cell properties to stdout.
    if ((int) verbose==1){
        fprintf(stderr,"#Cell analysis for elevation %f:\n",dbzMeta->elev);
        fprintf(stderr,"#Minimum cell area in pixels   : %i\n",areaMin);
        fprintf(stderr,"#Threshold for mean dBZ cell   : %g dBZ\n",cellDbzMin);
        fprintf(stderr,"#Threshold for mean stdev cell : %g dBZ\n",cellStdDevMax);
        fprintf(stderr,"#Valid cells                   : %i/%i\n#\n",nCellsValid,nCells);
        fprintf(stderr,"cellProp: .index .area .clutterArea .dbzAvg .texAvg   .cv .dbzMax .iRangOfMax .iAzimOfMax .drop\n");
        for (iCell = 0; iCell < nCells; iCell++) {
            if (cellProp[iCell].area==0) {
                continue;
            }
            fprintf(stderr,"cellProp: %6d %5d %12d %7.2f %7.2f %3.2f %7.2f %11d %11d %5d\n",
                    cellProp[iCell].index,
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

    range1 = (float) iRang1 * rangScale;
    range2 = (float) iRang2 * rangScale;

    azimuth1 = (float) iAzim1 * azimScaleDeg * (float) DEG2RAD;
    azimuth2 = (float) iAzim2 * azimScaleDeg * (float) DEG2RAD;

    return (float) sqrt((range1 * range1) +
                (range2 * range2) -
                2 * (range1 * range2) * cos(azimuth1-azimuth2));


} // calcDist






void calcTexture(unsigned char *texImage, const unsigned char *vradImage,
        const unsigned char *dbzImage, const SCANMETA *texMeta, const SCANMETA *vradMeta,
        const SCANMETA *dbzMeta, const int nRangNeighborhood,
        const int nAzimNeighborhood, const int nCountMin) {


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
    unsigned char missingValue;
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

    nNeighborhood = (int)(nRangNeighborhood * nAzimNeighborhood);

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

                vRadDiff = vradOffset + vradScale * (float) (vradImage[iGlobal] - vradImage[iLocal]);
                vmoment1 += vRadDiff;
                vmoment2 += SQUARE(vRadDiff);

                dbz += dbzOffset + dbzScale * (float) dbzImage[iLocal];

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
                texImage[iGlobal] = (unsigned char) ROUND((tex - texOffset) / texScale);

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







void classifyGates(const SCANMETA dbzMeta, const SCANMETA vradMeta, const SCANMETA rawReflMeta, const SCANMETA clutterMeta,
        const int *cellImage, const unsigned char *dbzImage, const unsigned char *vradImage, unsigned char *rawReflImage, const unsigned char *clutterImage,
        float *zdata, int *nzdata, const float rangeMin, const float rangeMax, const float layerThickness, const float XOFFSET,
        const float XSCALE, const float XMEAN, const float heightOfInterest, const float azimMin, const float azimMax,
        const float absVradMin, const float dbzClutter, const float dbzMin, const float dBZx, const float DBZNOISE,
        const int iLayer, const unsigned char clutterFlag, const unsigned char rawReflFlag, const unsigned char xflag) {


    //  *****************************************************************************
    //  This function classifies the range gates, distinguishing between clutter,
    //  rain, fringes, empty and valid gates. It returns the classification
    //  and layer counters
    //  *****************************************************************************

    // FIXME XOFFSET suggests preprocessor but isn't
    // FIXME XSCALE suggests preprocessor but isn't
    // FIXME XMEAN suggests preprocessor but isn't
    // FIXME DBZNOISE suggests preprocessor but isn't
    // FIXME NDBZMIN suggests preprocessor but isn't

    int iAzim;
    int nAzim;
    int iRang;
    int nRang;
    int iCol0;
    int iCol1;
    int iCol2;
    int nCols;
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
    float unDbzValue;
    float vradValue;
    float clutterValue;

    nPoints = 0;
    nPointsAll = 0;
    nPointsClutter = 0;
    nPointsRain = 0;
    nPointsRainNoFringe = 0;

    // for each height layer (a.k.a each bin in the density profiles), keep track of nCols variables:
    nCols = 3;

    // These are the indices into 'zdata' for the 3 variables that we track for each height layer:
    iCol0 = (iLayer * nCols) + 0;  // nPointsAll - nPointsRain - nPointsClutter
    iCol1 = (iLayer * nCols) + 1;  // nPointsRainNoFringe
    iCol2 = (iLayer * nCols) + 2;  // nPointsAll - nPointsClutter

    nRang = dbzMeta.nRang;
    nAzim = dbzMeta.nAzim;

    for (iRang = 0; iRang < nRang; iRang++) {

        for (iAzim = 0; iAzim < nAzim; iAzim++) {

            range = (float) (iRang+0.5) * dbzMeta.rangeScale;
            azim = (float) iAzim * dbzMeta.azimScale;  // FIXME (iAzim + 0.5) would be better I think
            heightBeam = range * (float) sin(dbzMeta.elev*DEG2RAD) + dbzMeta.heig;

            #ifdef FPRINTFON
            fprintf(stderr,"range = %f; azim = %f; heightBeam = %f\n",range, azim, heightBeam);
            #endif

            if (range < rangeMin || range > rangeMax) {
                // ignore range gates that are either too close or too far away
                continue;
            }

            if (azim <= azimMin || azim > azimMax) {
                // This clause is for when the user wants to use only those range gates from a certain direction
                continue;
            }

            if (fabs(heightOfInterest-heightBeam) > 0.5*layerThickness) {
                // if the height of the current gate is not within half a layerThickness distance from
                // the height of interest, continue with the next gate.
                continue;
            }


            iGlobal = iRang + iAzim * nRang;

            dbzValue = dbzMeta.valueScale*(float) dbzImage[iGlobal] + dbzMeta.valueOffset;
            unDbzValue = (float) exp(0.1*log(10)*dbzValue);
            vradValue = vradMeta.valueScale*(float) vradImage[iGlobal] + vradMeta.valueOffset;
            clutterValue = clutterMeta.valueScale*(float) clutterImage[iGlobal] + clutterMeta.valueOffset;



            #ifdef FPRINTFON
            fprintf(stderr,"dbzValue = %f; vradValue = %f; clutterValue = %f\n",dbzValue, vradValue, clutterValue);
            #endif

            nPointsAll++;

            if ((int) clutterFlag == 1){
                // take clutter into account
                if (clutterValue > dbzClutter){

                    // the current gate is classified as clutter. Raise the corresponding counter:
                    nPointsClutter++;

                    #ifdef FPRINTFON
                    fprintf(stderr,"nPointsClutter = %d\n",nPointsClutter);
                    #endif

                    continue;
                }
            }

            if ((int) rawReflFlag == 1){

                // Take into account that points can be dropped by the signal processor

                if (dbzImage[iGlobal] == dbzMeta.missing && rawReflImage[iGlobal] != rawReflMeta.missing){

                    // points without valid reflectivity data, but WITH raw reflectivity data are points
                    // dropped by the signal preprocessor. These will be treated as clutter.

                    nPointsClutter++;
                    continue;
                }
            }

            if (dbzImage[iGlobal] != dbzMeta.missing && vradImage[iGlobal] == vradMeta.missing){

                // we have reflectivity but the corresponding doppler data is missing. The gate is classified as clutter:

                nPointsClutter++;
                continue;
            }

            if (dbzValue < dbzMin) {

                // the reflectivity value is too low to be considered.

                dbzValue = DBZNOISE;
                unDbzValue = (float) exp(0.1*log(10)*DBZNOISE);

                // FIXME why reset the dbzValue to DBZNOISE?
            }

            if (vradImage[iGlobal] != vradMeta.missing && fabs(vradValue) < absVradMin){

                // we have a valid value for vrad but the magnitude of the value is too
                // low. The gate is classified as clutter:

                nPointsClutter++;
                continue;
            }

            // FIXME what does dBZx represent?
            if (cellImage[iGlobal] > 0 || dbzValue > dBZx) {
                if (cellImage[iGlobal] > 1) {

                    // i.e. the cells from cellImage, without any added fringes
                    if (isnan(zdata[iCol1])) {
                        // FIXME what is this clause for
                        zdata[iCol1] = 0;
                    }

                    zdata[iCol1] += unDbzValue;
                    nPointsRainNoFringe++;

                }

                if (isnan(zdata[iCol2])) {
                    // FIXME what is this clause for
                    zdata[iCol2] = 0;
                }
                zdata[iCol2] += unDbzValue;
                nPointsRain++;
                continue;
            }

            if (isnan(zdata[iCol0])) {
                // FIXME what is this clause for
                zdata[iCol0] = 0;
            }

            if (isnan(zdata[iCol2])) {
                // FIXME what is this clause for
                zdata[iCol2] = 0;
            }

            if ((int) xflag==1) {
                zdata[iCol0] += unDbzValue*XMEAN/(XOFFSET+XSCALE/range);
            }
            else {
                zdata[iCol0] += unDbzValue;
            }
            zdata[iCol2] += unDbzValue;
            nPoints++;
        }//for iAzim
    }//for iRang

    nzdata[iCol0] = nPointsAll - nPointsRain - nPointsClutter;
    nzdata[iCol1] = nPointsRainNoFringe;
    nzdata[iCol2] = nPointsAll - nPointsClutter;

    // FIXME Adriaan's latest code has some additional calculations to calculate the various fractions


    return;
} // classifyGates



int detNumberOfGates(const int iLayer, const float layerThickness,
                     const float rangeMin, const float rangeMax,
                     const float rangeScale, const float elevAngle,
                     const int nRang, const int nAzim,
                     const float radarHeight) {

    // Determine the number of gates that are within the limits set
    // by (rangeMin,rangeMax) as well as by (iLayer*layerThickness,
    // (iLayer+1)*layerThickness).

    int nGates;
    float layerHeight;
    int iRang;
    float range;
    float beamHeight;

    nGates = 0;
    layerHeight = (iLayer + 0.5) * layerThickness;
    for (iRang = 0; iRang < nRang; iRang++) {
        range = (iRang + 0.5) * rangeScale;
        if (range < rangeMin || range > rangeMax) {
            // the gate is too close to the radar, or too far away
            continue;
        }
        beamHeight = range * sin(elevAngle * DEG2RAD) + radarHeight;
        if (fabs(layerHeight - beamHeight) > 0.5*layerThickness) {
            // the gate is not close enough to the altitude layer of interest
            continue;
        }

        #ifdef FPRINTFON
        fprintf(stderr, "iRang = %d; range = %f; beamHeight = %f\n",iRang,range,beamHeight);
        #endif

        nGates += nAzim;
    } // iRang

    return nGates;

} // detNumberOfGates()



int getListOfSelectedGates(const SCANMETA vradMeta, const unsigned char *vradImage, float *points, float *yObs,
        int *c, const int *cellImage,
        const float rangeMin, const float rangeMax, const float layerThickness, const float heightOfInterest,
        const float absVradMin, const int iData, int nPoints)
{

    int nDims;
    int iAzim;
    int iRang;
    int iPoint;
    float gateHeight;
    float gateRange;
    float gateAzim;
    int nRang;
    int nAzim;
    float rangeScale;
    float azimuthScale;
    float elevAngle;
    unsigned char missing;
    float radarHeight;
    int iGlobal;
    float valueOffset;
    float valueScale;

    iPoint = nPoints;

    nRang = vradMeta.nRang;
    nAzim = vradMeta.nAzim;
    rangeScale = vradMeta.rangeScale;
    azimuthScale = vradMeta.azimScale;
    elevAngle = vradMeta.elev;
    missing = vradMeta.missing;
    radarHeight = vradMeta.heig;
    valueOffset = vradMeta.valueOffset;
    valueScale = vradMeta.valueScale;

    nDims = 2;


    for (iRang = 0; iRang < nRang; iRang++) {

        // so gateRange represents a distance along the view direction (not necessarily horizontal)
        gateRange = ((float) iRang + 0.5f) * rangeScale;

        // note that "sin(elevAngle*DEG2RAD)" is equivalent to = "cos((90 - elevAngle)*DEG2RAD)":
        gateHeight = gateRange * (float) sin(elevAngle*DEG2RAD) + radarHeight;

        if (gateRange < rangeMin || gateRange > rangeMax) {
            // the current gate is either (1) too close to the radar
            // or (2) too far away.
            continue;
        }
        if (fabs(heightOfInterest-gateHeight) > 0.5*layerThickness) {
            // if the height of the middle of the current gate is too far away from
            // the requested height, continue with the next gate
            continue;
        }


        for (iAzim = 0; iAzim < nAzim; iAzim++) {

            iGlobal = iRang + iAzim * nRang;
            gateAzim = ((float) iAzim + 0.5f) * azimuthScale;

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

            if (fabs(yObs[iPoint]) >= absVradMin) {

                // so at this point we've checked a couple of things and we see no reason
                // why vRadImage[iGlobal] shouldn't be part of the 'points' array

                // store the location as an azimuth angle, elevation angle combination
                points[iPoint * nDims + 0] = gateAzim;
                points[iPoint * nDims + 1] = elevAngle;

                // store the corresponding observed vrad value
                yObs[iPoint] = valueScale * (float) vradImage[iGlobal] + valueOffset;

                // FIXME Adriaan's latest version also stores the dbz value --useful when estimating the bird density

                // store the corresponding cellImage value
                c[iPoint] = cellImage[iGlobal];

                // raise the counter by 1
                iPoint++;

            }
        }  //for iAzim
    } //for iRang

    nPoints = iPoint;

    return nPoints;


} //getListOfSelectedGates





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

    unsigned char dbzThres;

    int iGlobal;
    int iGlobalOther;
    int nGlobal;
    int iLocal;

    unsigned char dbzMissing;
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

    if (dbzImage==NULL) {
        fprintf(stderr,"Input argument dbzImage is NULL.\n");
        return -1;
    }

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
        dbzThres = (unsigned char) ROUND((dbzThresMin - dbzValueOffset) / dbzValueScale);
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

            if ((float)(iRang + 1) * dbzRangeScale > rCellMax) {
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

            if (dbzImage[iGlobal] == (unsigned char) dbzMissing) {

                #ifdef FPRINTFON
                fprintf(stderr,"dbzImage[%d] == dbzMissing\n",iGlobal);
                #endif

                continue;
            }

            if (dbzImage[iGlobal] < (unsigned char) dbzThres) {
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
    int isEdge;
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
                continue; // with the next iGlobal; already fringe or not in cellImage
            }

            // determine whether current pixel is a pixel on the edge of a cell
            isEdge = FALSE;
            for (iNeighborhood = 0; iNeighborhood < 9 && isEdge == FALSE; iNeighborhood++) {

                iLocal = findNearbyGateIndex(nAzim,nRang,iGlobal,3,3,iNeighborhood);

                if (iLocal < 0) {
                    // iLocal less than zero are error codes
                    continue; // with the next iGlobal
                }

                if (cellImage[iLocal] < 1) { //FIXME THIS SHOULD BE <=1, BUT THEN TAKES MUCH MUCH LONGER
                    isEdge = TRUE;
                }

            } //NOW ONLY CELL PIXELS WITHOUT ANY BORDERING FRINGE ARE 'FRINGED'

            if (isEdge == FALSE) {
                continue; // with the next iGlobal
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
                    continue; // with the next iGlobal
                }
                //include pixel (iRangLocal,iAzimLocal) in fringe
                cellImage[iLocal] = 1;
            } // (iNeighborhood = 0; iNeighborhood < nNeighborhood; iNeighborhood++)
        } // (iRang = 0; iRang < nRang; iRang++)
    } // (iAzim = 0; iAzim < nAzim; iAzim++)


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





int updateMap(int *cellImage, int nGlobal, CELLPROP *cellProp, int nCells, int minCellArea) {

    //  *****************************************************************************
    //  This function updates the cellImage by dropping cells and reindexing the map
    //  Leaving index 0 unused, will be used for assigning cell fringes
    //  *****************************************************************************


    int iGlobal;
    int iCell;
    int iCellNew;
    int nCellsValid;
    int cellImageValue;


    #ifdef FPRINTFON
    int minValue = cellImage[0];
    int maxValue = cellImage[0];
    for (iGlobal = 1;iGlobal < nGlobal;iGlobal++) {
        if (cellImage[iGlobal] < minValue) {
            minValue = cellImage[iGlobal];
        }
        if (cellImage[iGlobal] > maxValue) {
            maxValue = cellImage[iGlobal];
        }
    }
    fprintf(stderr,"minimum value in cellImage array = %d.\n", minValue);
    fprintf(stderr,"maximum value in cellImage array = %d.\n", maxValue);
    #endif

    nCellsValid = nCells;

    for (iGlobal = 0; iGlobal < nGlobal; iGlobal++) {

        if (cellImage[iGlobal] == -1) {
            continue;
        }

        cellImageValue = cellImage[iGlobal];

        if (cellImageValue > nCells - 1) {
            fprintf(stderr, "You just asked for the properties of cell %d, which does not exist.\n", cellImageValue);
            continue;
        }

        if (cellProp[cellImageValue].drop == 1) {
            cellImage[iGlobal] = -1;
        }

//        #ifdef FPRINTFON
//        fprintf(stderr,"cellImage[%d] = %d\n",iGlobal,cellImage[iGlobal]);
//        #endif
    }


    /*Sort the cells by area and determine number of valid cells*/
    sortCells(cellProp, nCells);

    while (nCellsValid > 0 && cellProp[nCellsValid - 1].area < minCellArea) {
        nCellsValid--;
    }

    #ifdef FPRINTFON
    fprintf(stderr,"nCellsValid = %d\n",nCellsValid);
    fprintf(stderr,"\n");
    #endif

    // replace the values in cellImage with newly calculated index values:
    for (iCell = 0; iCell < nCells; iCell++) {

        if (iCell < nCellsValid) {
            iCellNew = -1 * (iCell + 2 + 100);
        }
        else {
            iCellNew = -1;
        }

        #ifdef FPRINTFON
        fprintf(stderr,"before: cellProp[%d].index = %d.\n",iCell,cellProp[iCell].index);
        fprintf(stderr,"before: cellProp[%d].area = %d.\n",iCell,cellProp[iCell].area);
        fprintf(stderr,"before: iCell = %d.\n",iCell);
        fprintf(stderr,"before: iCellNew = %d.\n",iCellNew);
        fprintf(stderr,"\n");
        #endif


        for (iGlobal = 0; iGlobal < nGlobal; iGlobal++) {
            if (cellImage[iGlobal] == cellProp[iCell].index) {
                cellImage[iGlobal] = iCellNew;
            }
        }
        // have the indices in cellProp match the re-numbering
        cellProp[iCell].index = iCellNew;

    } // (iCell = 0; iCell < nCells; iCell++)

//    for (iGlobal = 0; iGlobal < nGlobal; iGlobal++) {
//        if (cellImage[iGlobal] > -1) {
//            cellImage[iGlobal] = -1;
//        }
//    }


    // once you've re-numbered everything, flip the sign back and
    // remove the offset of 100...
    for (iGlobal = 0; iGlobal < nGlobal; iGlobal++) {
        if (cellImage[iGlobal] == -1) {
            // do nothing
        }
        else {
            cellImage[iGlobal] = (-1 * cellImage[iGlobal]) - 100;
        }
    }
    // ...and make sure the indices in cellProp match that change
    for (iCell = 0; iCell < nCells; iCell++) {

        if (cellProp[iCell].index == -1) {
            // do nothing
        }
        else {
            cellProp[iCell].index = (-1 * cellProp[iCell].index) - 100;
        }

        #ifdef FPRINTFON
        fprintf(stderr,"after: cellProp[%d].index = %d.\n",iCell,cellProp[iCell].index);
        fprintf(stderr,"after: cellProp[%d].area = %d.\n",iCell,cellProp[iCell].area);
        fprintf(stderr,"\n");
        #endif
    }



    return nCellsValid;
} //updateMap






