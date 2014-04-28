/*
 * libvol2bird.c
 *
 *  Created on: Apr 2, 2014
 *      Author: Jurriaan H. Spaaks, Netherlands eScience Center
 */

#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "libvol2bird.h"
//#include "nl_esciencecenter_ncradar_JNIMethodsVol2Bird.h" // maybe only used when calling java from c?

JNIEXPORT jintArray JNICALL
Java_nl_esciencecenter_ncradar_JNIMethodsVol2Bird_calcTexture(
   JNIEnv *env,
  jobject obj,
     jint tMissing,
     jint tnAzim,
     jint tnRange,
  jdouble tOffset,
  jdouble tScale,
jintArray vImage,
     jint vMissing,
     jint vnAzim,
     jint vnRange,
  jdouble vOffset,
  jdouble vScale,
jintArray zImage,
     jint zMissing,
     jint znAzim,
     jint znRange,
  jdouble zOffset,
  jdouble zScale,
     jint nRangLocal,
     jint nAzimLocal,
     jint nCountMin,
     jint textype)
{

    // do some Java Native interface tricks:
    jsize nElems = (*env)->GetArrayLength(env, vImage);

    jsize nv = (*env)->GetArrayLength(env, vImage);
    jsize nz = (*env)->GetArrayLength(env, zImage);

    jint *vImageBody = (*env)->GetIntArrayElements(env, vImage, NULL);
    jint *zImageBody = (*env)->GetIntArrayElements(env, zImage, NULL);
    jint *tImageBody = (jint*)malloc(nElems*sizeof(jint));
    // end of Java Native Interface tricks

    int iRang;
    int nRang;
    int iAzim;
    int nAzim;
    int iGlobal;
    int nGlobal;
    int iNeighborhood;
    int nNeighborhood;
    int iRangLocal;
    int iAzimLocal;
    int iLocal;
    int count;
    double vmoment1;
    double vmoment2;
    double dbz;
    double tex;
    double vRad;

    // check whether the sizes of the input arrays are the same:
    if (nv!=nz) {
        fprintf(stderr,"error: different sized input arguments.");
        return;
    } else {
        nGlobal = nv;
    }

    // check if the number of rows in each image is consistent
    if (tnAzim != vnAzim || tnAzim != znAzim || vnAzim != znAzim) {
        fprintf(stderr,"error: nAzim arguments have different values");
        return;
    }

    // check if the number of columns in each image is consistent
    if (tnRange != vnRange || tnRange != znRange || vnRange != znRange) {
        fprintf(stderr,"error: nRange arguments have different values");
        return;
    }

    // check if there are exactly enough elements in vImageBody and zImageBody;
    if (nGlobal != tnAzim*tnRange) {
        fprintf(stderr,"error: number of elements in arrays must match nRange*nAzim");
        return;
    }

    // verify that the moving window can be centered on the current row
    if (nAzimLocal%2!=1) {
        fprintf(stderr,"error: nAzimLocal must be odd integer.");
        return;
    }

    // verify that the moving window can be centered on the current column
    if (nRangLocal%2!=1) {
        fprintf(stderr,"error: nRangeLocal must be odd integer.");
        return;
    }

    // start the calculation

    tex = tOffset; // FIXME why does this variable have a value?
    nRang = vnRange;
    nAzim = vnAzim;

    nNeighborhood = nRangLocal*nAzimLocal;

    for (iAzim=0; iAzim<nAzim; iAzim++) {
        for (iRang=0; iRang<nRang; iRang++) {

            iGlobal = iRang+iAzim*nRang;

            /* count number of direct neighbors above threshold */
            count = 0;
            vmoment1 = 0;
            vmoment2 = 0;
            dbz = zOffset + zScale * zImageBody[iGlobal];

            for (iNeighborhood=0; iNeighborhood<nNeighborhood; iNeighborhood++) {

                iRangLocal = (iRang-nRangLocal/2+iNeighborhood%nRangLocal);

                // The modulo below is because the data in vImageBody forms a polar coordinate plot,
                // therefore iAzim=0 and iAzim=nAzim-1 are neighbors
                iAzimLocal = (nAzim+(iAzim-nAzimLocal/2+iNeighborhood/nRangLocal))%nAzim;
                iLocal = iRangLocal+iAzimLocal*nRang;

                if (iLocal >= nGlobal || iLocal < 0) {
                    continue;
                }
                if (vImageBody[iLocal]==vMissing || zImageBody[iLocal]==zMissing) {
                    continue;
                }

                // FIXME why difference between local and global?
                vRad = vOffset + vScale * (vImageBody[iGlobal]-vImageBody[iLocal]);
                vmoment1 += vRad;
                vmoment2 += SQUARE(vRad);

                // FIXME dbz at iRang,iAzim is counted double?
                dbz += zOffset + zScale * zImageBody[iLocal];
                count++;

            }

            vmoment1 /= count;
            vmoment2 /= count;
            dbz /= count;

            /* when not enough neighbors, continue */
            if (count < nCountMin) {
                tImageBody[iGlobal] = (int)tMissing;
            }
            else {
                if (textype == TEXCV){
                    tex = 10*log10(sqrt(XABS(vmoment2-SQUARE(vmoment1))))-dbz;
                    if (tex < tOffset + tScale) {
                        tex = tOffset + tScale;  // FIXME not sure this is right
                    }
                }
                if (textype == TEXSTDEV){
                    tex = sqrt(XABS(vmoment2-SQUARE(vmoment1)));
                } else {
                    fprintf(stderr,"error: seems like you have an invalid 'textype'.");
                    return;
                }
                tImageBody[iGlobal] = (int)ROUND((tex-tOffset)/tScale);
            } // else
            fprintf(stderr,"count = %d; nCountMin = %d; textype = %d; vmoment1 = %f; vmoment2 = %f; tex = %f; tImageBody[%d] = %d\n",count,nCountMin,textype,vmoment1,vmoment2,tex,iGlobal,tImageBody[iGlobal]);

        } // for iRang
    } // for iAzim


    // do some more Java Native Interface tricks:
    (*env)->ReleaseIntArrayElements(env, vImage, vImageBody, 0);
    (*env)->ReleaseIntArrayElements(env, zImage, zImageBody, 0);

    jintArray tImage = (*env)->NewIntArray(env,nElems);
    (*env)->SetIntArrayRegion(env,tImage,0,nElems,tImageBody);
    // end of Java Native Interface tricks


    return tImage;
}


