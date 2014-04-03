

#include <jni.h>
#include <stdio.h>
#include "nl_esciencecenter_ncradar_JNIMethodsVol2Bird.h"


// Implementation of native method addIntegers() of JNIMethodsVol2Bird class
JNIEXPORT jint JNICALL Java_nl_esciencecenter_ncradar_JNIMethodsVol2Bird_addIntegers(JNIEnv *env, jobject thisObj, jint a, jint b) {
   printf("Hello World from C :p %d \n",a+b);
   return a+b;
}

