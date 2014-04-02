/*
 * libpassarray.c
 *
 *  Created on: Apr 2, 2014
 *      Author: wbouten
 */

#include <jni.h>
#include <stdio.h>
#include <stdlib.h>

JNIEXPORT jintArray JNICALL
Java_nl_esciencecenter_ncradar_RadarScan_passArray(JNIEnv *env, jobject obj, jintArray vImage)
{
  jsize nElems = (*env)->GetArrayLength(env, vImage);

  jint *vImageBody = (*env)->GetIntArrayElements(env, vImage, NULL);

  printf("input array length = %d\n",nElems);

  jint *tImageBody = (jint*)malloc(nElems*sizeof(jint));

  int sum;
  int iElem;
  sum = 0;
  for (iElem=0;iElem<nElems;iElem++){
      sum = sum + vImageBody[iElem];
      printf("(C) iElem %d = %d\n",iElem,sum);
      tImageBody[iElem] = sum;
  }

  jintArray tImage = (*env)->NewIntArray(env,nElems);
  (*env)->SetIntArrayRegion(env,tImage,0,nElems,tImageBody);
  (*env)->ReleaseIntArrayElements(env, vImage, vImageBody, 0);


  return tImage;
}
