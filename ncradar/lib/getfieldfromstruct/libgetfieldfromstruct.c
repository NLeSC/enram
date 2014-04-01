

#include <jni.h>
#include <stdio.h>
#include "nl_esciencecenter_ncradar_RadarScan.h"


// Implementation of native method getFieldFromStruct() of RadarScan class
JNIEXPORT jint JNICALL Java_nl_esciencecenter_ncradar_RadarScan_getFieldFromStruct(JNIEnv *env, jobject thisObj) {
   printf("Hello World from C/getFieldFromStruct");
   int a = 4;
   return a;
}

