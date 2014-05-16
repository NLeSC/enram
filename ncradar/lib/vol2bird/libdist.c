


#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "libvol2bird.h"
//#include "nl_esciencecenter_ncradar_JNIMethodsVol2Bird.h" // maybe only used when calling java from c?

JNIEXPORT jfloat JNICALL
Java_nl_esciencecenter_ncradar_JNIMethodsVol2Bird_calcDist(
JNIEnv *env,
jobject obj,
jint range1,
jint azim1,
jint range2,
jint azim2,
jfloat rscale,
jfloat ascale)
{

    return dist(range1, azim1, range2, azim2, rscale, ascale);

}


/******************************************************************************/
/*This function calculates the distance in km between two gates               */
/******************************************************************************/
float dist(int range1, int azim1,int range2,int azim2,float rscale,float ascale)
{
    float x1;
    float x2;
    float y1;
    float y2;
    x1 = range1 * cos(azim1 * ascale * DEG2RAD);
    x2 = range2 * cos(azim2 * ascale * DEG2RAD);
    y1 = range1 * sin(azim1 * ascale * DEG2RAD);
    y2 = range2 * sin(azim2 * ascale * DEG2RAD);
    return sqrt(SQUARE(x1-x2) + SQUARE(y1-y2));
} //dist
