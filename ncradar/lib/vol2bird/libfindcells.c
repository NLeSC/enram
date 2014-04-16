/*
 * libfindcells.c
 *
 *  Created on: Apr 2, 2014
 *      Author: Jurriaan H. Spaaks, Netherlands eScience Center
 */

#include <jni.h>
//#include <stdio.h>
#include <stdlib.h>
//#include <math.h>
#include "libvol2bird.h"
//#include "nl_esciencecenter_ncradar_JNIMethodsVol2Bird.h" // maybe only used when calling java from c?



JNIEXPORT jintArray JNICALL
Java_nl_esciencecenter_ncradar_JNIMethodsVol2Bird_findCells(JNIEnv *env, jobject obj, jintArray cellImage, jintArray texImage, jint texnRang, jint texnAzim, jdouble texOffset, jdouble texScale, jint texMissing, jdouble texThresMin, jintArray rhoImage, jint rhonRang, jint rhonAzim, jdouble rhoOffset, jdouble rhoScale, jint rhoMissing, jdouble rhoThresMin, jintArray zdrImage, jint zdrnRang, jint zdrnAzim, jdouble zdrOffset, jdouble zdrScale, jint zdrMissing, jdouble zdrThresMin, jdouble dbzmin, jdouble rcellmax, jint sign, jdouble texrScale)
{

    // do some Java Native interface tricks:
    jsize texnElems = (*env)->GetArrayLength(env, texImage);
    jsize rhonElems = (*env)->GetArrayLength(env, rhoImage);
    jsize zdrnElems = (*env)->GetArrayLength(env, zdrImage);

    jint *texImageBody = (*env)->GetIntArrayElements(env, texImage, NULL);
    jint *rhoImageBody = (*env)->GetIntArrayElements(env, rhoImage, NULL);
    jint *zdrImageBody = (*env)->GetIntArrayElements(env, zdrImage, NULL);
    jint *cellImageBody = (jint*)malloc(texnElems*sizeof(jint));
    // end of Java Native Interface tricks


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
    int cellImageBodyInitialValue;

    double texThres;
    double rhoThres;
    double zdrThres;

    int iGlobal;
    int iGlobalOther;
    int nGlobal;
    int iLocal;


    // check whether the sizes of the input arrays are the same:
    if (rhonElems!=texnElems || zdrnElems!=texnElems) {
        fprintf(stderr,"error: different sized input arguments.");
        return;
    } else {
        nGlobal = texnElems;
    }

    // check if the number of rows in each image is consistent
    if (rhonAzim != texnAzim || zdrnAzim != texnAzim) {
        fprintf(stderr,"error: nAzim arguments have different values");
        return;
    }
    else {
        nAzim = texnAzim;
    }

    // check if the number of columns in each image is consistent
    if (rhonRang != texnRang || zdrnRang != texnRang) {
        fprintf(stderr,"error: nRang arguments have different values");
        return;
    }
    else {
        nRang = texnRang;
    }

    // check if there are exactly enough elements in vImageBody and zImageBody;
    if (nGlobal != nAzim*nRang) {
        fprintf(stderr,"error: number of elements in arrays must match nRang*nAzim");
        return;
    }


    texThres = ROUND((texThresMin-texOffset)/texScale);  // FIXME why type is int?

    if (rhoImageBody!=NULL) {
        rhoThres = ROUND((rhoThresMin-rhoOffset)/rhoScale);  // FIXME why type is int?
    }
    if (zdrImageBody!=NULL) {
        zdrThres = ROUND((zdrThresMin-zdrOffset)/zdrScale);  // FIXME why type is int?
    }

    /*Initializing of connection cellImageBody.*/
    cellImageBodyInitialValue = -1;
    for (iGlobal=0; iGlobal<nGlobal; iGlobal++) {
        cellImageBody[iGlobal] = cellImageBodyInitialValue;
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

            if ((iRang+1)*texrScale>rcellmax) {  // FIXME what is the difference between zscale and rscale // FIXME why iRang +1 ?
                continue;
            }
            if (texImageBody[iGlobal]==texMissing) {
                continue;
            }

            /* count number of direct neighbors above threshold */
            count = 0;

            if (rhoImageBody==NULL){
                if (sign*texImageBody[iGlobal]>sign*texThres) {  // FIXME why sign x2? ... sort of an ABS?
                    continue;
                }
                for (iNeighborhood=0; iNeighborhood<9; iNeighborhood++) {
                    iRangLocal = (iRang-1+iNeighborhood%3);
                    iAzimLocal = (nAzim+(iAzim-1+iNeighborhood/3))%nAzim;
                    iLocal = iRangLocal+iAzimLocal*nRang;
                    if (iLocal >= nGlobal || iLocal < 0) {
                        continue;
                    }
                    if (sign*texImageBody[iLocal]<=sign*texThres) { // FIXME sign 2x ? // FIXME why <= instead of >
                        count++;
                    }
                }
            }
            else {
                if (rhoImageBody[iGlobal]==rhoMissing) {
                    continue;
                }
                if (zdrImageBody[iGlobal]==zdrMissing) {
                    continue;
                }
                if (texImageBody[iGlobal]<dbzmin) {  // FIXME tex v dbz why?
                    continue;
                }
                if (!(zdrImageBody[iGlobal]>zdrThres || rhoImageBody[iGlobal]>rhoThres)) {
                    continue;
                }

                for (iNeighborhood=0; iNeighborhood<9; iNeighborhood++) {
                    iRangLocal = (iRang-1+iNeighborhood%3);
                    iAzimLocal = (nAzim+(iAzim-1+iNeighborhood/3))%nAzim;
                    iLocal = iRangLocal+iAzimLocal*nRang;
                    if (rhoImageBody[iLocal]>rhoThres || zdrImageBody[iLocal]>zdrThres) {
                        count++;
                    }
                }
            } // if (rhoImageBody==NULL)

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
                if (iRangLocal<0 || iRangLocal>=nRang || iAzimLocal<0 || iAzimLocal>=nAzim) {
                    continue;
                }

                /* no connection found, go to next pixel within neighborhood */
                if (cellImageBody[iLocal] == cellImageBodyInitialValue) {
                    continue;
                }

                /* if pixel still unassigned, assign same iCellIdentifier as connection */
                if (cellImageBody[iGlobal] == cellImageBodyInitialValue) {
                    cellImageBody[iGlobal] = cellImageBody[iLocal];
                }
                else {
                    /* if connection found but pixel is already assigned a different iCellIdentifier: */
                    if (cellImageBody[iGlobal]!=cellImageBody[iLocal]) {
                        /* merging cells detected: replace all other occurences by value of connection: */
                        for (iGlobalOther=0; iGlobalOther<nGlobal; iGlobalOther++) {
                            if (cellImageBody[iGlobalOther]==cellImageBody[iGlobal]) {
                                cellImageBody[iGlobalOther] = cellImageBody[iLocal];
                            }
                            /*note: not all iCellIdentifier need to be used eventually */
                        }
                    }
                }
            }

            /*When no connections are found, give a new number.*/
            if (cellImageBody[iGlobal] == cellImageBodyInitialValue) {
                cellImageBody[iGlobal] = iCellIdentifier;
                iCellIdentifier++;
            }

        } // (iRang=0; iRang<nRang; iRang++)
    } // for (iAzim=0; iAzim<nAzim; iAzim++)

    /*Returning number of detected cells (including fringe/clutter) .*/

    nCells = iCellIdentifier;


    // do some more Java Native Interface tricks:
    (*env)->ReleaseIntArrayElements(env, texImage, texImageBody, 0);
    (*env)->ReleaseIntArrayElements(env, rhoImage, rhoImageBody, 0);
    (*env)->ReleaseIntArrayElements(env, zdrImage, zdrImageBody, 0);

    jintArray tImage = (*env)->NewIntArray(env,nGlobal);
    (*env)->SetIntArrayRegion(env,cellImage,0,nGlobal,cellImageBody);
    // end of Java Native Interface tricks

    return cellImage;
}//findcells
