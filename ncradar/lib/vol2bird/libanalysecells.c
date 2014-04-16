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
jdoubleArray cellPropDbz,
jdoubleArray cellPropTex,
jdoubleArray cellPropCv,
jdoubleArray cellPropArea,
jdoubleArray cellPropClutterArea,
jdoubleArray cellPropMax,
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
jdouble area,
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
    jdouble *cellPropDbzBody = (*env)->GetDoubleArrayElements(env, cellPropDbz, NULL);
    jdouble *cellPropTexBody = (*env)->GetDoubleArrayElements(env, cellPropTex, NULL);
    jdouble *cellPropCvBody = (*env)->GetDoubleArrayElements(env, cellPropCv, NULL);
    jdouble *cellPropAreaBody = (*env)->GetDoubleArrayElements(env, cellPropArea, NULL);
    jdouble *cellPropClutterAreaBody = (*env)->GetDoubleArrayElements(env, cellPropClutterArea, NULL);
    jdouble *cellPropMaxBody = (*env)->GetDoubleArrayElements(env, cellPropMax, NULL);
    jint *cellPropIndexBody = (*env)->GetIntArrayElements(env, cellPropIndex, NULL);
    jint *cellPropDropBody = (*env)->GetIntArrayElements(env, cellPropDrop, NULL);
    // end of Java Native Interface tricks

    int iCell;
    int iGlobal;
    int iAzim;
    int iRang;
    int nCellValid;
    int iCellIdentifier;
    float validArea;
    float dbzValue;
    float vradValue;
    float texValue;
    float cmapValue;

    nCellValid = nCells;

    if (nGlobal != nAzim*nRang) {
        fprintf(stderr,"Inconsistent array sizes.");
    }

    iCellIdentifier = cellImageBody[iGlobal];

    for (iCell=0; iCell<nCells; iCell++) {
        cellPropIRangOfMaxBody[iCell] = 0;
        cellPropIAzimOfMaxBody[iCell] = 0;
        cellPropAreaBody[iCell] = 0;
        cellPropClutterAreaBody[iCell] = 0;
        cellPropDbzBody[iCell] = 0;
        cellPropTexBody[iCell] = 0;
        cellPropMaxBody[iCell] = dbzOffset;
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

            if (dbzValue>cellPropMaxBody[iCellIdentifier]) {
                cellPropMaxBody[iCellIdentifier] = dbzValue;
                cellPropIRangOfMaxBody[iCellIdentifier] = iGlobal%nAzim;
                cellPropIAzimOfMaxBody[iCellIdentifier] = iGlobal/nAzim;
            }
            cellPropDbzBody[iCellIdentifier] += dbzValue;
            cellPropTexBody[iCellIdentifier] += texValue;
        }
    }

    for (iCell = 0; iCell<nCells; iCell++) {
        validArea = cellPropAreaBody[iCell] - cellPropClutterAreaBody[iCell];
        if (validArea>0){
            cellPropDbzBody[iCell] /= validArea;
            cellPropTexBody[iCell] /= validArea;
            cellPropCvBody[iCell] = 10*log10(cellPropTexBody[iCell])-cellPropDbzBody[iCell];  // FIXME this is immediately overwritten in the next line
            cellPropCvBody[iCell] = cellPropTexBody[iCell]/cellPropDbzBody[iCell];
        }
    }

    /*Determine which cells to drop from map based on low mean dBZ / high stdev / small area / high percentage clutter*/
    for (iCell = 0; iCell<nCells; iCell++) {
        if (dualpolflag == 1){
            if (cellPropAreaBody[iCell]<area){
                cellPropDropBody[iCell] = 1;
            }
        }
        else {
            if (cellPropAreaBody[iCell]<area || (cellPropDbzBody[iCell]<dbzThresCell && cellPropTexBody[iCell]>stdevThresCell && cellPropClutterAreaBody[iCell]/cellPropAreaBody[iCell]< clutcell )) {
                cellPropDropBody[iCell] = 1;
            }
        }
    }

    /*Sorting cell properties according to cell area. Drop small cells from map*/
    nCellValid = updatemap(cellImageBody,c,nCells,nGlobal,area);

    //Printing of cell properties to stdout.
    if (verbose==1){
        fprintf(stdout,"#Cell analysis for elevation %f:\n",elev);
        fprintf(stdout,"#Minimum cell area in pixels   : %i\n",area);
        fprintf(stdout,"#Threshold for mean dBZ cell   : %g dBZ\n",dbzThresCell);
        fprintf(stdout,"#Threshold for mean stdev cel  : %g dBZ\n",stdevThresCell);
        fprintf(stdout,"#Valid cells                   : %i/%i\n",nCellValid,nCells);
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
                    cellPropDbzBody[iCell],
                    cellPropTexBody[iCell],
                    cellPropCvBody[iCell],
                    cellPropMaxBody[iCell],
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



    return nCellValid;
} //analysecells
