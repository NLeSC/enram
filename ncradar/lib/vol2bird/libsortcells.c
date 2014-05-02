
#include <jni.h>
#include <stdlib.h>
#include "libvol2bird.h"

JNIEXPORT void JNICALL
Java_nl_esciencecenter_ncradar_JNIMethodsVol2Bird_sortCells(
JNIEnv *env,
jobject obj,
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
jint method
)
{
/******************************************************************************/
/*Sorting of the cell properties using cell area or mean.                     */
/*Assume an area or mean equal to zero for cells that are marked 'dropped'    */
/******************************************************************************/

    // do some Java Native interface tricks:

    // these arrays have nCells elements:
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

    sortcells(cellProp,nCells,method);


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



    // do some more Java Native Interface tricks:
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
}



void sortcells(CELLPROP *cellProp, int nCells, int method)
{


    int iCell;
    int iCellOther;
    CELLPROP tmp;

    /*Sorting of data elements using straight insertion method.*/
    if (method==BYAREA) {
        for (iCell = 1; iCell<nCells; iCell++) {

            tmp = cellProp[iCell];

            iCellOther = iCell-1;

            while (iCellOther>=0 && cellProp[iCellOther].area * XABS(cellProp[iCellOther].drop-1) < tmp.area * XABS(tmp.drop-1)) {

                cellProp[iCellOther+1] = cellProp[iCellOther];

                iCellOther--;
            }

            cellProp[iCellOther+1] = tmp;

       } //for iCell
    } else if (method==BYMEAN){
        for (iCell = 1; iCell<nCells; iCell++) {

            tmp = cellProp[iCell];

            iCellOther = iCell-1;

            while (iCellOther>=0 && cellProp[iCellOther].dbzAvg * XABS(cellProp[iCellOther].drop-1) < tmp.dbzAvg * XABS(tmp.drop-1)) {

                cellProp[iCellOther+1] = cellProp[iCellOther];

                iCellOther--;
            }

            cellProp[iCellOther+1] = tmp;

       } //for iCell
    }


   return;
}//sortcells

