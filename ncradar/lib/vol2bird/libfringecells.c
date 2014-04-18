



#include <jni.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <math.h>
#include "libvol2bird.h"
//#include "nl_esciencecenter_ncradar_JNIMethodsVol2Bird.h" // maybe only used when calling java from c?

JNIEXPORT void JNICALL
Java_nl_esciencecenter_ncradar_JNIMethodsVol2Bird_fringeCells(
JNIEnv *env,
jobject obj,
jintArray cellImage,
jint nRang,
jint nAzim,
jfloat aScale,
jfloat rScale,
jfloat fringe)
{
    // do some Java Native interface tricks:
    jint *cellImageBody = (*env)->GetIntArrayElements(env, cellImage, NULL);
    // end of Java Native Interface tricks

    fringecells(cellImageBody,nRang,nAzim,aScale,rScale,fringe);

    // do some Java Native interface tricks:
    (*env)->ReleaseIntArrayElements(env, cellImage, cellImageBody, 0);
    // end of Java Native Interface tricks


}




/******************************************************************************/
/*This function enlarges cellmap by additional fringe.                        */
/*First a block around each pixel is searched for pixels within a distance    */
/*equal to 'fringe'.                                                          */
/*                                                                            */
/******************************************************************************/
void fringecells(int *cellImage,int nRang, int nAzim, float aScale, float rScale, float fringe)
{
    int iRang;
    int iAzim;
    int iNeighborhood;
    int iRangLocal;
    int iAzimLocal;
    int iLocal;
    int rBlock;
    int aBlock;
    int edge;
    int iGlobal;
    float tmp;

    rBlock = ROUND(fringe/rScale);
    for (iAzim = 0; iAzim<nAzim; iAzim++){
        for (iRang = 0; iRang<nRang; iRang++){

            iGlobal = iRang+iAzim*nRang;

            if(cellImage[iGlobal]<=1) {
                continue; //already fringe or not in cellImage
            }

            //determine whether current pixel is a pixel on the edge of a cell
            edge = 0;
            for (iNeighborhood=0; iNeighborhood < 9 && !edge; iNeighborhood++){
                iRangLocal = iRang-1+iNeighborhood%3;
                iAzimLocal = (nAzim+(iAzim-1+iNeighborhood/3))%nAzim;
                iLocal = iRangLocal + iAzimLocal * nRang;
                if (iRangLocal<0 || iRangLocal>=nRang) {  // FIXME why only this dimension?
                    continue;
                }
                if(cellImage[iLocal]<1) {
                    edge = 1; //THIS SHOULD BE <=1, BUT THEN TAKES MUCH MUCH LONGER
                }
            } //NOW ONLY CELL PIXELS WITHOUT ANY BORDERING FRINGE ARE 'FRINGED'

            if(!edge) {
                continue;
            }

            //search a limited block around pixel (iRang,iAzim) only
            tmp = (XYMAX(0,iRang-rBlock)*rScale*aScale*DEG2RAD);
            if (tmp<fringe/nAzim) {
                aBlock = nAzim;
            }
            else {
                aBlock = ROUND(fringe/tmp);
            }
            for (iNeighborhood = 0; iNeighborhood<(2*rBlock+1)*(2*aBlock+1); iNeighborhood++) {

                iRangLocal = iRang - rBlock + iNeighborhood%(2*rBlock+1);
                iAzimLocal = (nAzim + (iAzim - aBlock + iNeighborhood/(2*rBlock+1)))%nAzim;
                iLocal = iRangLocal + iAzimLocal * nRang;
                if (iRangLocal<0 || iRangLocal>=nRang) {  // FIXME why only test this dimension?
                    continue;
                }
                //if not within range or already in cellImage or already a fringe, do nothing
                if (dist(iRang,iAzim,iRangLocal,iAzimLocal,rScale,aScale)>fringe || cellImage[iLocal]>=1) {
                    continue;
                }
                //include pixel (iRangLocal,iAzimLocal) in fringe
                cellImage[iLocal] = 1; //assign index 1 to fringes
            }
        }
    } //for
    return;
} //fringecells
