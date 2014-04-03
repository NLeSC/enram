#include <jni.h>
#include <stdio.h>
#include "nl_esciencecenter_ncradar_JNIMethodsVol2Bird.h"

// Implementation of native method sayHello() of JNIMethodsVol2Bird class
JNIEXPORT void JNICALL Java_nl_esciencecenter_ncradar_JNIMethodsVol2Bird_sayHello(JNIEnv *env, jobject thisObj) {
   printf("Hello World from C :p  \n");
   return;
}
