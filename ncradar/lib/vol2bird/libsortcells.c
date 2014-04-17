
#include <jni.h>
//#include <stdio.h>
//#include <stdlib.h>
#include "libvol2bird.h"
//#include "nl_esciencecenter_ncradar_JNIMethodsVol2Bird.h" // maybe only used when calling java from c?

JNIEXPORT jint JNICALL
Java_nl_esciencecenter_ncradar_JNIMethodsVol2Bird_sortCells(
JNIEnv *env,
jobject obj,
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
    int iCellOther;
    int tmpIRangOfMax;
    int tmpIAzimOfMax;
    double tmpDbzAvg;
    double tmpTexAvg;
    double tmpCv;
    double tmpArea;
    double tmpClutterArea;
    int tmpDbzMax;
    int tmpIndex;
    int tmpDrop;
    int condition;

    /*Sorting of data elements using straight insertion method.*/
    if (method==BYAREA) {
        for (iCell = 1; iCell<nCells; iCell++) {

            tmpIRangOfMax = cellPropIRangOfMaxBody[iCell];
            tmpIAzimOfMax = cellPropIAzimOfMaxBody[iCell];
            tmpDbzAvg = cellPropDbzAvgBody[iCell];
            tmpTexAvg = cellPropTexAvgBody[iCell];
            tmpCv = cellPropCvBody[iCell];
            tmpArea =  cellPropAreaBody[iCell];
            tmpClutterArea = cellPropClutterAreaBody[iCell];
            tmpDbzMax = cellPropDbzMaxBody[iCell];
            tmpIndex = cellPropIndexBody[iCell];
            tmpDrop = cellPropDropBody[iCell];

            iCellOther = iCell-1;

            while (iCellOther>=0 && cellPropAreaBody[iCellOther] * XABS(cellPropDropBody[iCellOther]-1) < tmpArea * XABS(tmpDrop-1)) {

                cellPropIRangOfMaxBody[iCellOther+1] = cellPropIRangOfMaxBody[iCellOther];
                cellPropIAzimOfMaxBody[iCellOther+1] = cellPropIAzimOfMaxBody[iCellOther];
                cellPropDbzAvgBody[iCellOther+1] = cellPropDbzAvgBody[iCellOther];
                cellPropTexAvgBody[iCellOther+1] = cellPropTexAvgBody[iCellOther];
                cellPropCvBody[iCellOther+1] = cellPropCvBody[iCellOther];
                cellPropAreaBody[iCellOther+1] = cellPropAreaBody[iCellOther];
                cellPropClutterAreaBody[iCellOther+1] = cellPropClutterAreaBody[iCellOther];
                cellPropDbzMaxBody[iCellOther+1] = cellPropDbzMaxBody[iCellOther];
                cellPropIndexBody[iCellOther+1] = cellPropIndexBody[iCellOther];
                cellPropDropBody[iCellOther+1] = cellPropDropBody[iCellOther];

                iCellOther--;
            }

            cellPropIRangOfMaxBody[iCellOther+1] = tmpIRangOfMax;
            cellPropIAzimOfMaxBody[iCellOther+1] = tmpIAzimOfMax;
            cellPropDbzAvgBody[iCellOther+1] = tmpDbzAvg;
            cellPropTexAvgBody[iCellOther+1] = tmpTexAvg;
            cellPropCvBody[iCellOther+1] = tmpCv;
            cellPropAreaBody[iCellOther+1] =  tmpArea;
            cellPropClutterAreaBody[iCellOther+1] = tmpClutterArea;
            cellPropDbzMaxBody[iCellOther+1] = tmpDbzMax;
            cellPropIndexBody[iCellOther+1] = tmpIndex;
            cellPropDropBody[iCellOther+1] = tmpDrop;

       } //for iCell
    } else if (method==BYMEAN){
        for (iCell = 1; iCell<nCells; iCell++) {

            tmpIRangOfMax = cellPropIRangOfMaxBody[iCell];
            tmpIAzimOfMax = cellPropIAzimOfMaxBody[iCell];
            tmpDbzAvg = cellPropDbzAvgBody[iCell];
            tmpTexAvg = cellPropTexAvgBody[iCell];
            tmpCv = cellPropCvBody[iCell];
            tmpArea =  cellPropAreaBody[iCell];
            tmpClutterArea = cellPropClutterAreaBody[iCell];
            tmpDbzMax = cellPropDbzMaxBody[iCell];
            tmpIndex = cellPropIndexBody[iCell];
            tmpDrop = cellPropDropBody[iCell];

            iCellOther = iCell-1;

            while (iCellOther>=0 && cellPropDbzAvgBody[iCellOther] * XABS(cellPropDropBody[iCellOther]-1) < tmpDbzAvg * XABS(tmpDrop-1)) {

                cellPropIRangOfMaxBody[iCellOther+1] = cellPropIRangOfMaxBody[iCellOther];
                cellPropIAzimOfMaxBody[iCellOther+1] = cellPropIAzimOfMaxBody[iCellOther];
                cellPropDbzAvgBody[iCellOther+1] = cellPropDbzAvgBody[iCellOther];
                cellPropTexAvgBody[iCellOther+1] = cellPropTexAvgBody[iCellOther];
                cellPropCvBody[iCellOther+1] = cellPropCvBody[iCellOther];
                cellPropAreaBody[iCellOther+1] = cellPropAreaBody[iCellOther];
                cellPropClutterAreaBody[iCellOther+1] = cellPropClutterAreaBody[iCellOther];
                cellPropDbzMaxBody[iCellOther+1] = cellPropDbzMaxBody[iCellOther];
                cellPropIndexBody[iCellOther+1] = cellPropIndexBody[iCellOther];
                cellPropDropBody[iCellOther+1] = cellPropDropBody[iCellOther];

                iCellOther--;
            }

            cellPropIRangOfMaxBody[iCellOther+1] = tmpIRangOfMax;
            cellPropIAzimOfMaxBody[iCellOther+1] = tmpIAzimOfMax;
            cellPropDbzAvgBody[iCellOther+1] = tmpDbzAvg;
            cellPropTexAvgBody[iCellOther+1] = tmpTexAvg;
            cellPropCvBody[iCellOther+1] = tmpCv;
            cellPropAreaBody[iCellOther+1] =  tmpArea;
            cellPropClutterAreaBody[iCellOther+1] = tmpClutterArea;
            cellPropDbzMaxBody[iCellOther+1] = tmpDbzMax;
            cellPropIndexBody[iCellOther+1] = tmpIndex;
            cellPropDropBody[iCellOther+1] = tmpDrop;

       } //for iCell
    }

    // do some more Java Native Interface tricks:
    (*env)->ReleaseIntArrayElements(env, cellPropIRangOfMax, cellPropIRangOfMaxBody, 0);
    (*env)->ReleaseIntArrayElements(env, cellPropIAzimOfMax, cellPropIAzimOfMaxBody, 0);
    (*env)->ReleaseDoubleArrayElements(env, cellPropDbzAvg, cellPropDbzAvgBody, 0);
    (*env)->ReleaseDoubleArrayElements(env, cellPropTexAvg, cellPropTexAvgBody, 0);
    (*env)->ReleaseDoubleArrayElements(env, cellPropCv, cellPropCvBody, 0);
    (*env)->ReleaseDoubleArrayElements(env, cellPropArea, cellPropAreaBody, 0);
    (*env)->ReleaseDoubleArrayElements(env, cellPropClutterArea, cellPropClutterAreaBody, 0);
    (*env)->ReleaseDoubleArrayElements(env, cellPropDbzMax, cellPropDbzMaxBody, 0);
    (*env)->ReleaseIntArrayElements(env, cellPropIndex, cellPropIndexBody, 0);
    (*env)->ReleaseIntArrayElements(env, cellPropDrop, cellPropDropBody, 0);
    // end of Java Native Interface tricks


   return;
}//sortcells

