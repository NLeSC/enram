/******************************************************************************/
/*This function analyses the cellmap found by the 'findcells' procedure.      */
/*Smalls cells are rejected and the cells are re-numbered according to size   */
/*The final number of cells in the cellmap is returned as an integer          */
/******************************************************************************/

/*
 * libvol2bird.c
 *
 *  Created on: Apr 2, 2014
 *      Author: Jurriaan H. Spaaks, Netherlands eScience Center
 */

#include <jni.h>
//#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "libvol2bird.h"
//#include "nl_esciencecenter_ncradar_JNIMethodsVol2Bird.h" // maybe only used when calling java from c?

JNIEXPORT jint JNICALL
Java_nl_esciencecenter_ncradar_JNIMethodsVol2Bird_analyseCells(
      JNIEnv *env,
     jobject obj,
        jint nAzim,
        jint nRang,
   jintArray dbzImage,
   jintArray texImage,
   jintArray vradImage,
   jintArray cmapImage,
   jintArray cellImage,
   jintArray cellPropIRangOfMax,
   jintArray cellPropIAzimOfMax,
jdoubleArray cellPropDbzAvg,
jdoubleArray cellPropTexAvg,
jdoubleArray cellPropCv,
jdoubleArray cellPropArea,
jdoubleArray cellPropClutterArea,
jdoubleArray cellPropDbzMax,
   jintArray cellPropIndex,
   jintArray cellPropDrop,
     jdouble elev,
     jdouble dbzScale,
     jdouble dbzOffset,
        jint vradScale,
     jdouble vradOffset,
     jdouble texScale,
     jdouble texOffset,
     jdouble cmapScale,
     jdouble cmapOffset,
        jint nCells,
     jdouble minCellArea,
     jdouble dbzThresCell,
     jdouble stdevThresCell,
     jdouble clutcell,
     jdouble vmin,
     jdouble dbzclutter,
        jint cmapFlag,
        jint dualpolflag,
        jint verbose)
{

    // do some Java Native interface tricks:

    jsize nGlobal = (*env)->GetArrayLength(env, dbzImage);

    // these arrays have nGlobal elements
    jint *dbzImageBody = (*env)->GetIntArrayElements(env, dbzImage, NULL);
    jint *texImageBody = (*env)->GetIntArrayElements(env, texImage, NULL);
    jint *vradImageBody = (*env)->GetIntArrayElements(env, vradImage, NULL);
    jint *cmapImageBody = (*env)->GetIntArrayElements(env, cmapImage, NULL);
    jint *cellImageBody = (*env)->GetIntArrayElements(env, cellImage, NULL);

    // these arrays have nCells elements:
    jint *cellPropIRangOfMaxBody = (*env)->GetIntArrayElements(env, cellPropIRangOfMax, NULL);
    jint *cellPropIAzimOfMaxBody = (*env)->GetIntArrayElements(env, cellPropIAzimOfMax, NULL);
    jdouble *cellPropDbzAvgBody = (*env)->GetDoubleArrayElements(env, cellPropDbzAvg, NULL);
    jdouble *cellPropTexAvgBody = (*env)->GetDoubleArrayElements(env, cellPropTexAvg, NULL);
    jdouble *cellPropCvBody = (*env)->GetDoubleArrayElements(env, cellPropCv, NULL);
    jdouble *cellPropAreaBody = (*env)->GetDoubleArrayElements(env, cellPropArea, NULL);
    jdouble *cellPropClutterAreaBody = (*env)->GetDoubleArrayElements(env, cellPropClutterArea, NULL);
    jdouble *cellPropDbzMaxBody = (*env)->GetDoubleArrayElements(env, cellPropDbzMax, NULL);
    jint *cellPropIndexBody = (*env)->GetIntArrayElements(env, cellPropIndex, NULL);
    jint *cellPropDropBody = (*env)->GetIntArrayElements(env, cellPropDrop, NULL);
    // end of Java Native Interface tricks

    int iCell;
    int iGlobal;
    int iAzim;
    int iRang;
    int nCellsValid;
    int iCellIdentifier;
    float validArea;
    float dbzValue;
    float vradValue;
    float texValue;
    float cmapValue;

    nCellsValid = nCells;

    if (nGlobal != nAzim*nRang) {
        fprintf(stderr,"Inconsistent array sizes.");
    }

    iCellIdentifier = cellImageBody[iGlobal];

    for (iCell=0; iCell<nCells; iCell++) {
        cellPropIRangOfMaxBody[iCell] = 0;
        cellPropIAzimOfMaxBody[iCell] = 0;
        cellPropAreaBody[iCell] = 0;
        cellPropClutterAreaBody[iCell] = 0;
        cellPropDbzAvgBody[iCell] = 0;
        cellPropTexAvgBody[iCell] = 0;
        cellPropDbzMaxBody[iCell] = dbzOffset;
        cellPropIndexBody[iCell] = iCell;
        cellPropDropBody[iCell] = 0;
        cellPropCvBody[iCell] = 0;
    }

    /*Calculation of cell properties.*/
    for (iAzim = 0; iAzim<nAzim; iAzim++) {
        for (iRang = 0; iRang<nRang; iRang++) {

            iGlobal = iRang+iAzim*nRang;

            dbzValue = dbzScale*dbzImageBody[iGlobal] + dbzOffset;
            cmapValue = cmapScale*cmapImageBody[iGlobal] + cmapOffset;
            texValue = texScale*texImageBody[iGlobal] + texOffset;
            vradValue = vradScale*vradImageBody[iGlobal] + vradOffset;

            if (iCellIdentifier<0){  //FIXME cellImageBody == -1? if you're testing for identity
                continue;
            }

            //low radial velocities are treated as clutter, not included in calculation cell properties
            if (fabs(vradValue) < vmin){
                cellPropClutterAreaBody[iCellIdentifier] += 1;
                cellPropAreaBody[iCellIdentifier] += 1;
                continue;
            }

            //pixels in clutter map not included in calculation cell properties
            if (cmapFlag == 1){
                if (cmapValue > dbzclutter) {
                    cellPropClutterAreaBody[iCellIdentifier] += 1;
                    cellPropAreaBody[iCellIdentifier] += 1;
                    continue;
                }
            }
            cellPropAreaBody[iCellIdentifier] += 1;

            if (dbzValue>cellPropDbzMaxBody[iCellIdentifier]) {
                cellPropDbzMaxBody[iCellIdentifier] = dbzValue;
                cellPropIRangOfMaxBody[iCellIdentifier] = iGlobal%nAzim;
                cellPropIAzimOfMaxBody[iCellIdentifier] = iGlobal/nAzim;
            }
            cellPropDbzAvgBody[iCellIdentifier] += dbzValue;
            cellPropTexAvgBody[iCellIdentifier] += texValue;
        } // for (iRang = 0; iRang<nRang; iRang++)
    } //  (iAzim = 0; iAzim<nAzim; iAzim++)

    for (iCell = 0; iCell<nCells; iCell++) {
        validArea = cellPropAreaBody[iCell] - cellPropClutterAreaBody[iCell];
        if (validArea>0){
            cellPropDbzAvgBody[iCell] /= validArea;
            cellPropTexAvgBody[iCell] /= validArea;
            cellPropCvBody[iCell] = 10*log10(cellPropTexAvgBody[iCell])-cellPropDbzAvgBody[iCell];  // FIXME this is immediately overwritten in the next line
            cellPropCvBody[iCell] = cellPropTexAvgBody[iCell]/cellPropDbzAvgBody[iCell];
        }
    }

    /*Determine which cells to drop from map based on low mean dBZ / high stdev / small minCellArea / high percentage clutter*/
    for (iCell = 0; iCell<nCells; iCell++) {
        if (dualpolflag == 1){
            if (cellPropAreaBody[iCell]<minCellArea){
                cellPropDropBody[iCell] = 1;
            }
        }
        else {
            if (cellPropAreaBody[iCell]<minCellArea || (cellPropDbzAvgBody[iCell]<dbzThresCell && cellPropTexAvgBody[iCell]>stdevThresCell && cellPropClutterAreaBody[iCell]/cellPropAreaBody[iCell]< clutcell )) {
                cellPropDropBody[iCell] = 1;
            }
        }
    }

    /*Sorting cell properties according to cell minCellArea. Drop small cells from map*/
    //nCellsValid = updatemap(cellImageBody,c,nCells,nGlobal,minCellArea);

    nCellsValid = Java_nl_esciencecenter_ncradar_JNIMethodsVol2Bird_updateMap(*env, obj, cellImage,
            cellPropArea, cellPropIndex, cellPropDrop, nCells, nGlobal, minCellArea);


    //Printing of cell properties to stdout.
    if (verbose==1){
        fprintf(stdout,"#Cell analysis for elevation %f:\n",elev);
        fprintf(stdout,"#Minimum cell area in pixels   : %i\n",minCellArea);
        fprintf(stdout,"#Threshold for mean dBZ cell   : %g dBZ\n",dbzThresCell);
        fprintf(stdout,"#Threshold for mean stdev cel  : %g dBZ\n",stdevThresCell);
        fprintf(stdout,"#Valid cells                   : %i/%i\n",nCellsValid,nCells);
        fprintf(stdout,"#\n");
        fprintf(stdout,"cellinfo: NUM CellArea ClutArea AvgDbz AvgStdev CV MaxVal MaxIII MaxJJJ Dropped\n");

        for (iCell=0; iCell<nCells; iCell++) {
            if (cellPropAreaBody[iCell]==0) {
                continue;
            }
            fprintf(stdout,"cellinfo: %3d %8.1f %8.1f %6.2f %6.2f %6.2f %6.2f %6d %6d %6d\n",
                    iCell+2,   //FIXME plus 2 why?
                    cellPropAreaBody[iCell],
                    cellPropClutterAreaBody[iCell],
                    cellPropDbzAvgBody[iCell],
                    cellPropTexAvgBody[iCell],
                    cellPropCvBody[iCell],
                    cellPropDbzMaxBody[iCell],
                    cellPropIRangOfMaxBody[iCell],
                    cellPropIAzimOfMaxBody[iCell],
                    cellPropDropBody[iCell]);
        }
    }

    free(c);

    // do some more Java Native Interface tricks:
    (*env)->ReleaseIntArrayElements(env, dbzImage, dbzImageBody, 0);
    (*env)->ReleaseIntArrayElements(env, texImage, texImageBody, 0);
    (*env)->ReleaseIntArrayElements(env, vradImage, vradImageBody, 0);
    (*env)->ReleaseIntArrayElements(env, cmapImage, cmapImageBody, 0);
    (*env)->ReleaseIntArrayElements(env, cellImage, cellImageBody, 0);
    // end of Java Native Interface tricks



    return nCellsValid;
} //analysecells
