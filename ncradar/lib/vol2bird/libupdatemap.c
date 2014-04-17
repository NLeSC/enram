
#include <jni.h>
//#include <stdio.h>
//#include <stdlib.h>
#include "libvol2bird.h"
//#include "nl_esciencecenter_ncradar_JNIMethodsVol2Bird.h" // maybe only used when calling java from c?

JNIEXPORT jint JNICALL
Java_nl_esciencecenter_ncradar_JNIMethodsVol2Bird_updateMap(
JNIEnv *env,
jobject obj,
jintArray cellImage,
jdoubleArray cellPropArea,
jintArray cellPropIndex,
jintArray cellPropDrop,
jint nCells,
jint nGlobal,
jint minCellArea)
{

/******************************************************************************/
/*This function updates the cellImage by dropping cells and reindexing the map  */
/*Leaving index 0 unused, will be used for assigning cell fringes             */
/******************************************************************************/
    int iGlobal;
    int iCellValid;
    int nCellsValid;


    nCellsValid = nCells;

    for (iGlobal=0; iGlobal<nGlobal; iGlobal++) {

        if (cellImage[iGlobal] == -1) {
            continue;
        }
        cellImageValue = cellImage[iGlobal];
        if (cellPropDrop[cellImageValue]) {
            cellImage[iGlobal] = -1;
        }
    }

    /*Sort the cells by minCellArea and determine number of valid cells*/
    sortcells(cellPropIRangOfMax, cellPropIAzimOfMax, cellPropDbz,
              cellPropTex, cellPropCv, cellPropArea, cellPropClutterArea,
              cellPropMax, cellPropIndex, cellPropDrop,nCells,BYAREA);

    while (nCellsValid>0 && cellPropArea[nCellsValid-1]<minCellArea){
        nCellsValid--;
    }

    /*reindex the map*/
    for (iCellValid=0; iCellValid<nCellsValid; iCellValid++){
        for (iGlobal=0 ; iGlobal<nGlobal ; iGlobal++) {
            if (cellImage[iGlobal]==cellPropIndex[iCellValid]) {
                cellImage[iGlobal] = iCellValid+1;
            }
        }
    }
    /*reindex the cellproperties object*/
    for (iCellValid = 0; iCellValid<nCells; iCellValid++) {
        cellPropIndex[iCellValid] = iCellValid+1;
    }

    return nCellsValid;
} //updatemap


