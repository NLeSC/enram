/*
 * libfindcells.c
 *
 *  Created on: Apr 2, 2014
 *      Author: Jurriaan H. Spaaks, Netherlands eScience Center
 */

#include <jni.h>
#include <stdlib.h>
#include "libvol2bird.h"




JNIEXPORT jint JNICALL
Java_nl_esciencecenter_ncradar_JNIMethodsVol2Bird_findCells(
        JNIEnv *env,
        jobject obj,
        jbyteArray texImage,
        jbyteArray rhoImage,
        jbyteArray zdrImage,
        jintArray cellImage,
        jint texMissing,
        jint texnAzim,
        jint texnRang,
        jfloat texValueOffset,
        jfloat texRangeScale,
        jfloat texValueScale,
        jfloat texThresMin,
        jint rhoMissing,
        jint rhonAzim,
        jint rhonRang,
        jfloat rhoValueOffset,
        jfloat rhoValueScale,
        jfloat rhoThresMin,
        jint zdrMissing,
        jint zdrnAzim,
        jint zdrnRang,
        jfloat zdrValueOffset,
        jfloat zdrValueScale,
        jfloat zdrThresMin,
        jfloat reflThresMin,
        jfloat rCellMax,
        jbyte sign)
{

    // do some Java Native interface tricks:
    jbyte *texImageBody = (*env)->GetByteArrayElements(env, texImage, NULL);
    jbyte *rhoImageBody = (*env)->GetByteArrayElements(env, rhoImage, NULL);
    jbyte *zdrImageBody = (*env)->GetByteArrayElements(env, zdrImage, NULL);
    jint *cellImageBody = (*env)->GetIntArrayElements(env, cellImage, NULL);
    // end of Java Native Interface tricks

    SCANMETA texMeta;
    SCANMETA rhoMeta;
    SCANMETA zdrMeta;

    texMeta.missing = texMissing;
    texMeta.nAzim = texnAzim;
    texMeta.nRang = texnRang;
    texMeta.valueOffset = texValueOffset;
    texMeta.valueScale = texValueScale;
    texMeta.rangeScale = texRangeScale;

    rhoMeta.missing = rhoMissing;
    rhoMeta.nAzim = rhonAzim;
    rhoMeta.nRang = rhonRang;
    rhoMeta.valueOffset = rhoValueOffset;
    rhoMeta.valueScale = rhoValueScale;

    zdrMeta.missing = zdrMissing;
    zdrMeta.nAzim = zdrnAzim;
    zdrMeta.nRang = zdrnRang;
    zdrMeta.valueOffset = zdrValueOffset;
    zdrMeta.valueScale = zdrValueScale;

    int nCells;


    nCells = findcells(texImageBody, rhoImageBody, zdrImageBody, cellImageBody,
                       &texMeta,     &rhoMeta,     &zdrMeta,
                       texThresMin,  rhoThresMin,  zdrThresMin,
                       reflThresMin, rCellMax, sign);


    // do some Java Native interface tricks:
    (*env)->ReleaseIntArrayElements(env, cellImage, cellImageBody, 0);
    (*env)->ReleaseByteArrayElements(env, texImage, texImageBody, JNI_ABORT);
    (*env)->ReleaseByteArrayElements(env, rhoImage, rhoImageBody, JNI_ABORT);
    (*env)->ReleaseByteArrayElements(env, zdrImage, zdrImageBody, JNI_ABORT);

    // end of Java Native Interface tricks

    return nCells;

}




int findcells(unsigned char *texImage,
              unsigned char *rhoImage,
              unsigned char *zdrImage,
              int *cellImage,
              SCANMETA *texMeta,
              SCANMETA *rhoMeta,
              SCANMETA *zdrMeta,
              float texThresMin,
              float rhoThresMin,
              float zdrThresMin,
              float reflThresMin,
              float rCellMax,
              char sign)
{

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


    nGlobal = nAzim*nRang;

    texThres = ROUND((texThresMin-texValueOffset)/texValueScale);  // FIXME why type is int?

    if (rhoImage!=NULL) {
        rhoThres = ROUND((rhoThresMin-rhoValueOffset)/rhoValueScale);  // FIXME why type is int?
    }
    if (zdrImage!=NULL) {
        zdrThres = ROUND((zdrThresMin-zdrValueOffset)/zdrValueScale);  // FIXME why type is int?
    }

    /*Initializing of connection cellImage.*/
    cellImageInitialValue = -1;
    for (iGlobal=0; iGlobal<nGlobal; iGlobal++) {
        cellImage[iGlobal] = cellImageInitialValue;
    }

    /*If threshold is missing, return.*/
    if (texThres==texMissing) {
        return 0;  // FIXME return zero is traditionally used to signal "everything is great!"
    }

    /*Labeling of groups of connected pixels using horizontal, vertical, and */
    /*diagonal connections. The algorithm is described in 'Digital Image */
    /*Processing' by Gonzales and Woods published by Addison-Wesley.*/

    iCellIdentifier = 0;

    for (iAzim=0; iAzim<nAzim; iAzim++) {
        for (iRang=0; iRang<nRang; iRang++) {

            iGlobal = iRang+iAzim*nRang;

            if ((iRang+1)*texRangeScale>rCellMax) {
                continue;
            }
            if (texImage[iGlobal]==texMissing) {
                continue;
            }

            /* count number of direct neighbors above threshold */
            count = 0;

            if (rhoImage==NULL){
                if (sign*texImage[iGlobal]>sign*texThres) {  // FIXME why sign x2? ... sort of an ABS?
                    continue;
                }
                for (iNeighborhood=0; iNeighborhood<9; iNeighborhood++) {
                    iRangLocal = (iRang-1+iNeighborhood%3);
                    iAzimLocal = (nAzim+(iAzim-1+iNeighborhood/3))%nAzim;
                    iLocal = iRangLocal+iAzimLocal*nRang;  // FIXME see issue #32
                    if (iLocal >= nGlobal || iLocal < 0) {
                        continue;
                    }
                    if (sign*texImage[iLocal]<=sign*texThres) { // FIXME sign 2x ? // FIXME why <= instead of >
                        count++;
                    }
                }
            }
            else {
                if (rhoImage[iGlobal]==rhoMissing) {
                    continue;
                }
                if (zdrImage[iGlobal]==zdrMissing) {
                    continue;
                }
                if (texImage[iGlobal]<reflThresMin) {  // FIXME tex v refl why?
                    continue;
                }
                if (!(zdrImage[iGlobal]>zdrThres || rhoImage[iGlobal]>rhoThres)) {
                    continue;
                }

                for (iNeighborhood=0; iNeighborhood<9; iNeighborhood++) {
                    iRangLocal = (iRang-1+iNeighborhood%3);
                    iAzimLocal = (nAzim+(iAzim-1+iNeighborhood/3))%nAzim;
                    iLocal = iRangLocal+iAzimLocal*nRang;     // FIXME see issue #32
                    if (rhoImage[iLocal]>rhoThres || zdrImage[iLocal]>zdrThres) {
                        count++;
                    }
                }
            } // if (rhoImage==NULL)

            /* when not enough qualified neighbors, continue */
            if (count-1 < NEIGHBOURS) {
                continue;
            }

            /*Looking for horizontal, vertical, forward diagonal, and backward diagonal */
            /*connections.*/

            for (iNeighborhood=0; iNeighborhood<4; iNeighborhood++) {

                iRangLocal = (iRang-1+iNeighborhood%3);
                iAzimLocal = (nAzim+(iAzim-1+iNeighborhood/3))%nAzim;
                iLocal = iRangLocal+iAzimLocal*nRang;

                /* index out of range, go to next pixel within neighborhood */
                if (iRangLocal<0 || iRangLocal>=nRang || iAzimLocal<0 || iAzimLocal>=nAzim) {   // FIXME possibly affected by issue #32
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
                    if (cellImage[iGlobal]!=cellImage[iLocal]) {
                        /* merging cells detected: replace all other occurences by value of connection: */
                        for (iGlobalOther=0; iGlobalOther<nGlobal; iGlobalOther++) {
                            if (cellImage[iGlobalOther]==cellImage[iGlobal]) {
                                cellImage[iGlobalOther] = cellImage[iLocal];
                            }
                            /*note: not all iCellIdentifier need to be used eventually */
                        }
                    }
                }
            }

            /*When no connections are found, give a new number.*/
            if (cellImage[iGlobal] == cellImageInitialValue) {
                cellImage[iGlobal] = iCellIdentifier;
                iCellIdentifier++;
            }

        } // (iRang=0; iRang<nRang; iRang++)
    } // for (iAzim=0; iAzim<nAzim; iAzim++)

    /*Returning number of detected cells (including fringe/clutter) .*/

    nCells = iCellIdentifier;

    return nCells;
}//findcells
