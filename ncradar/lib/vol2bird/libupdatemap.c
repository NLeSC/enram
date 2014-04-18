
#include <jni.h>
#include <stdlib.h>
#include "libvol2bird.h"

JNIEXPORT jint JNICALL
Java_nl_esciencecenter_ncradar_JNIMethodsVol2Bird_updateMap(
JNIEnv *env,
jobject obj,
jintArray cellImage,
jintArray cellPropIRangOfMax,
jintArray cellPropIAzimOfMax,
jfloatArray cellPropDbzAvg,
jfloatArray cellPropTexAvg,
jfloatArray cellPropCv,
jfloatArray cellPropArea,
jfloatArray cellPropClutterArea,
jfloatArray cellPropDbzMax,
jintArray cellPropIndex,
jcharArray cellPropDrop,
jint nCells,
jint nGlobal,
jint minCellArea)
{

    // do some Java Native interface tricks:
    // arrays with nGlobal elements:
    jint *cellImageBody = (*env)->GetIntArrayElements(env, cellImage, NULL);
    // arrays with nCells elements:
    jint *cellPropIRangOfMaxBody = (*env)->GetIntArrayElements(env, cellPropIRangOfMax, NULL);
    jint *cellPropIAzimOfMaxBody = (*env)->GetIntArrayElements(env, cellPropIAzimOfMax, NULL);
    jfloat *cellPropDbzAvgBody = (*env)->GetFloatArrayElements(env, cellPropDbzAvg, NULL);
    jfloat *cellPropTexAvgBody = (*env)->GetFloatArrayElements(env, cellPropTexAvg, NULL);
    jfloat *cellPropCvBody = (*env)->GetFloatArrayElements(env, cellPropCv, NULL);
    jfloat *cellPropAreaBody = (*env)->GetFloatArrayElements(env, cellPropArea, NULL);
    jfloat *cellPropClutterAreaBody = (*env)->GetFloatArrayElements(env, cellPropClutterArea, NULL);
    jfloat *cellPropDbzMaxBody = (*env)->GetFloatArrayElements(env, cellPropDbzMax, NULL);
    jint *cellPropIndexBody = (*env)->GetIntArrayElements(env, cellPropIndex, NULL);
    jchar *cellPropDropBody = (*env)->GetCharArrayElements(env, cellPropDrop, NULL);
    // end of Java Native Interface tricks

    int iCell;
    int nCellsValid;

    CELLPROP *cellProp;
    /*Allocating and initializing memory for cell properties.*/
    cellProp = (CELLPROP *)malloc(nCells*sizeof(CELLPROP));

    // construct the CELLPROP struct
    for (iCell = 0; iCell<nCells;iCell++) {
        cellProp[iCell].iRangOfMax = cellPropIRangOfMaxBody[iCell];
        cellProp[iCell].iAzimOfMax = cellPropIAzimOfMaxBody[iCell];
        cellProp[iCell].dbzAvg = cellPropDbzAvgBody[iCell];
        cellProp[iCell].texAvg = cellPropTexAvgBody[iCell];
        cellProp[iCell].cv = cellPropCvBody[iCell];
        cellProp[iCell].area = cellPropAreaBody[iCell];
        cellProp[iCell].clutterArea = cellPropClutterAreaBody[iCell];
        cellProp[iCell].dbzMax = cellPropDbzMaxBody[iCell];
        cellProp[iCell].index = cellPropIndexBody[iCell];
        cellProp[iCell].drop = cellPropDropBody[iCell];
    }

    nCellsValid = updatemap(cellImageBody,cellProp,nCells,nGlobal,minCellArea);


    // deconstruct the CELLPROP struct
    for (iCell = 0; iCell<nCells;iCell++) {
        cellPropIRangOfMaxBody[iCell] = cellProp[iCell].iRangOfMax;
        cellPropIAzimOfMaxBody[iCell] = cellProp[iCell].iAzimOfMax;
        cellPropDbzAvgBody[iCell] = cellProp[iCell].dbzAvg;
        cellPropTexAvgBody[iCell] = cellProp[iCell].texAvg;
        cellPropCvBody[iCell] = cellProp[iCell].cv;
        cellPropAreaBody[iCell] = cellProp[iCell].area;
        cellPropClutterAreaBody[iCell] = cellProp[iCell].clutterArea;
        cellPropDbzMaxBody[iCell] = cellProp[iCell].dbzMax;
        cellPropIndexBody[iCell] = cellProp[iCell].index;
        cellPropDropBody[iCell] = cellProp[iCell].drop;
    }


    // do some Java Native interface tricks:
    // arrays with nGlobal elements:
    (*env)->ReleaseIntArrayElements(env, cellImage, cellImageBody, 0);
    // arrays with nCells elements:
    (*env)->ReleaseIntArrayElements(env, cellPropIRangOfMax, cellPropIRangOfMaxBody, 0);
    (*env)->ReleaseIntArrayElements(env, cellPropIAzimOfMax, cellPropIAzimOfMaxBody, 0);
    (*env)->ReleaseFloatArrayElements(env, cellPropDbzAvg, cellPropDbzAvgBody, 0);
    (*env)->ReleaseFloatArrayElements(env, cellPropTexAvg, cellPropTexAvgBody, 0);
    (*env)->ReleaseFloatArrayElements(env, cellPropCv, cellPropCvBody, 0);
    (*env)->ReleaseFloatArrayElements(env, cellPropArea, cellPropAreaBody, 0);
    (*env)->ReleaseFloatArrayElements(env, cellPropClutterArea, cellPropClutterAreaBody, 0);
    (*env)->ReleaseFloatArrayElements(env, cellPropDbzMax, cellPropDbzMaxBody, 0);
    (*env)->ReleaseIntArrayElements(env, cellPropIndex, cellPropIndexBody, 0);
    (*env)->ReleaseCharArrayElements(env, cellPropDrop, cellPropDropBody, 0);
    // end of Java Native Interface tricks

    return nCellsValid;


}


int updatemap(int *cellImage,CELLPROP *cellProp,int nCells,int nGlobal,int minCellArea)
{
    /******************************************************************************/
    /*This function updates the cellImage by dropping cells and reindexing the map  */
    /*Leaving index 0 unused, will be used for assigning cell fringes             */
    /******************************************************************************/
    int iGlobal;
    int iCellValid;
    int nCellsValid;
    int cellImageValue;


    nCellsValid = nCells;

    for (iGlobal=0; iGlobal<nGlobal; iGlobal++) {

        if (cellImage[iGlobal] == -1) {
            continue;
        }
        cellImageValue = cellImage[iGlobal];
        if (cellProp[cellImageValue].drop) {
            cellImage[iGlobal] = -1;
        }
    }

    /*Sort the cells by minCellArea and determine number of valid cells*/
    sortcells(cellProp,nCells,BYAREA);

    while (nCellsValid>0 && cellProp[nCellsValid-1].area<minCellArea){
        nCellsValid--;
    }

    /*reindex the map*/
    for (iCellValid = 0; iCellValid<nCellsValid; iCellValid++){
        for (iGlobal = 0 ; iGlobal<nGlobal ; iGlobal++) {
            if (cellImage[iGlobal]==cellProp[iCellValid].index) {
                cellImage[iGlobal] = iCellValid+1;
            }
        }
    }
    /*reindex the cellproperties object*/
    for (iCellValid = 0; iCellValid<nCells; iCellValid++) {
        cellProp[iCellValid].index = iCellValid+1;
    }

    return nCellsValid;
} //updatemap


