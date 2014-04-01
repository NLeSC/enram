

#include <jni.h>
#include <stdio.h>
#include "nl_esciencecenter_ncradar_RadarScan.h"


// Implementation of native method addIntegers() of RadarScan class
JNIEXPORT jint JNICALL Java_nl_esciencecenter_ncradar_RadarScan_addIntegers(JNIEnv *env, jobject thisObj, jint a, jint b) {
   printf("Hello World from C :p %d \n",a+b);
   return a+b;
}

