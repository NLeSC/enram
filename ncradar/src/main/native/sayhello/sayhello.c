#include <jni.h>
#include <stdio.h>
#include "nl_esciencecenter_ncradar_RadarScan.h"

// Implementation of native method sayHello() of RadarScan class
JNIEXPORT void JNICALL Java_RadarScan_sayHello(JNIEnv *env, jobject thisObj) {
   printf("Hello World from C :p  \n");
   return;
}
