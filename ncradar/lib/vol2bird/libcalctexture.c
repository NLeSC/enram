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



JNIEXPORT void JNICALL
Java_nl_esciencecenter_ncradar_JNIMethodsVol2Bird_calcTexture(
        JNIEnv *env,
        jobject obj,
        jbyteArray texImage,
        jbyteArray reflImage,
        jbyteArray vradImage,
        jbyte nRangNeighborhood,
        jbyte nAzimNeighborhood,
        jbyte nCountMin,
        jbyte texType,
        jfloat texOffset,
        jfloat texScale,
        jfloat reflOffset,
        jfloat reflScale,
        jfloat vradOffset,
        jfloat vradScale,
        jint nRang,
        jint nAzim) {

    // do some Java Native interface tricks:
    jbyte *texImageBody = (*env)->GetByteArrayElements(env, texImage, NULL);
    jbyte *reflImageBody = (*env)->GetByteArrayElements(env, reflImage, NULL);
    jbyte *vradImageBody = (*env)->GetByteArrayElements(env, vradImage, NULL);

    SCANMETA texMeta;
    SCANMETA reflMeta;
    SCANMETA vradMeta;

    texMeta.valueOffset = texOffset;
    texMeta.valueScale = texScale;

    reflMeta.valueOffset = reflOffset;
    reflMeta.valueScale = reflScale;

    vradMeta.valueOffset = vradOffset;
    vradMeta.valueScale = vradScale;
    vradMeta.nRang = nRang;
    vradMeta.nAzim = nAzim;

    // end of Java Native Interface tricks

    texture(texImageBody, vradImageBody, reflImageBody,
            &texMeta, &vradMeta, &reflMeta,
            nRangNeighborhood, nAzimNeighborhood,
            nCountMin, texType);

//    int iElem;
//
//    for (iElem=0; iElem<12;iElem++) {
//        fprintf(stderr,"(C) ");
//        fprintf(stderr,"%d, ",texImageBody[iElem]);
//        fprintf(stderr,"%d, ",reflImageBody[iElem]);
//        fprintf(stderr,"%d",vradImageBody[iElem]);
//        fprintf(stderr,"\n");
//    }


    // do some Java Native interface tricks:
    (*env)->ReleaseByteArrayElements(env, texImage, texImageBody, 0);
    (*env)->ReleaseByteArrayElements(env, reflImage, reflImageBody, JNI_ABORT);
    (*env)->ReleaseByteArrayElements(env, vradImage, vradImageBody, JNI_ABORT);

    // end of Java Native Interface tricks

}



/*****************************************************************************************/
/* This function computes a texture parameter based on a block of (nRangNeighborhood x   */
/* nAzimNeighborhood) pixels. The texture parameter equals the local standard deviation  */
/* in the velocity field (when texType==STDEV).                                          */
/*****************************************************************************************/


void texture(unsigned char *texImage,unsigned char *vradImage, unsigned char *reflImage,
             SCANMETA *texMeta,SCANMETA *vradMeta,SCANMETA *reflMeta,
             unsigned char nRangNeighborhood,unsigned char nAzimNeighborhood,
             unsigned char nCountMin,unsigned char texType)
{
    int iRang;
    int iAzim;
    int iRangLocal;
    int iAzimLocal;
    int nRang;
    int nAzim;
    int iNeighborhood;
    int count;
    unsigned char missingValue;
    int value;
    int index;
    double vmoment1;
    double vmoment2;
    double dbz;
    double tex;
    int iGlobal;
    int iLocal;
    int nGlobal;
    int nNeighborhood;
    float texOffset;
    float texScale;
    float reflOffset;
    float reflScale;
    float vradOffset;
    float vradScale;
    float vRadDiff;

    nRang = vradMeta->nRang;
    nAzim = vradMeta->nAzim;
    nGlobal = nAzim*nRang;
    missingValue = vradMeta->missing;  // FIXME this missingValue is used indiscriminately in vRad, tex and dbz alike

    reflOffset = reflMeta->valueOffset;
    reflScale = reflMeta->valueScale;

    vradOffset = vradMeta->valueOffset;
    vradScale = vradMeta->valueScale;

    texOffset = texMeta->valueOffset;
    texScale = texMeta->valueScale;

    tex = texOffset;         // FIXME why does this variable have a value at this point?

    nNeighborhood = nRangNeighborhood * nAzimNeighborhood;

    for (iAzim = 0; iAzim < nAzim; iAzim++) {
        for (iRang = 0; iRang < nRang; iRang++) {

            iGlobal = iRang + iAzim * nRang;

            /* count number of direct neighbours above threshold */
            count = 0;
            vmoment1 = 0;
            vmoment2 = 0;

            dbz = reflOffset + reflScale * reflImage[iGlobal];

            for (iNeighborhood = 0; iNeighborhood < nNeighborhood; iNeighborhood++) {

                iRangLocal = iRang - nRangNeighborhood/2 + iNeighborhood%nRangNeighborhood;
                iAzimLocal = (nAzim + (iAzim - nAzimNeighborhood/2 + iNeighborhood/nRangNeighborhood))%nAzim;

                iLocal = iRangLocal + iAzimLocal * nRang;

                //fprintf(stderr,"    iAzim=%d,iRang=%d,iNeighborhood=%d;iAzimLocal=%d,iRangLocal=%d:iLocal=%d\n",iAzim, iRang, iNeighborhood, iAzimLocal,iRangLocal,iLocal);

                // FIXME iLocal is wrong at the iRang = 0 and iRang = nRang-1 edges: issue #32
                if (iLocal >= nGlobal || iLocal < 0) {
                    continue;
                }
                if (vradImage[iLocal]==missingValue || reflImage[iLocal]==missingValue) {
                    continue;
                }

                // FIXME why difference between local and global?
                //fprintf(stderr,"vradOffset=%f, vradScale = %f, vradImage[iGlobal]=%d, vradImage[iLocal]=%d\n",
                //         vradOffset, vradScale, vradImage[iGlobal], vradImage[iLocal]);

                vRadDiff = vradOffset + vradScale * (vradImage[iGlobal] - vradImage[iLocal]);
                vmoment1 += vRadDiff;
                vmoment2 += SQUARE(vRadDiff);

                dbz += reflOffset + reflScale * reflImage[iLocal];

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
                if (texType == TEXCV){

                    tex = 10*log10(sqrt(XABS(vmoment2-SQUARE(vmoment1))))-dbz;

                    if (tex < texOffset + texScale) {    // FIXME tex < texOffset would make more sense
                        tex = texOffset + texScale;
                    }

                }

                if (texType == TEXSTDEV){
                    tex = sqrt(XABS(vmoment2-SQUARE(vmoment1)));
                }

                texImage[iGlobal] = ROUND((tex-texOffset)/texScale);

                fprintf(stderr,"(C) count = %d; nCountMin = %d; texType = %d; vmoment1 = %f; vmoment2 = %f; tex = %f; texBody[%d] = %d\n",count,nCountMin,texType,vmoment1,vmoment2,tex,iGlobal,texImage[iGlobal]);

            } //else
        } //for
    } //for
}//texture
