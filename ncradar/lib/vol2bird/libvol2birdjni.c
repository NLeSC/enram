



#include <jni.h>
#include "libvol2bird.c"






JNIEXPORT jfloat JNICALL
Java_nl_esciencecenter_ncradar_JNIMethodsVol2Bird_analyseCells(
JNIEnv *env,
jobject obj,
jintArray dbzImageInt,
jintArray vradImageInt,
jintArray texImageInt,
jintArray clutterImageInt,
jintArray cellImageInt,
jint dbznRang,
jint dbznAzim,
jfloat dbzElev,
jfloat dbzValueScale,
jfloat dbzValueOffset,
jfloat vradValueScale,
jfloat vradValueOffset,
jfloat clutterValueScale,
jfloat clutterValueOffset,
jfloat texValueScale,
jfloat texValueOffset,
jint nCells,
jint areaMin,
jfloat cellDbzMin,
jfloat cellStdDevMax,
jfloat cellClutterFraction,
jfloat vradMinValue,
jfloat dbzClutterMin,
jint cmFlagInt,
jint dualPolFlagInt,
jint verboseInt
)
{




    // do some Java Native interface tricks:
    jint *dbzImageIntBody = (*env)->GetIntArrayElements(env, dbzImageInt, NULL);
    jint *vradImageIntBody = (*env)->GetIntArrayElements(env, vradImageInt, NULL);
    jint *texImageIntBody = (*env)->GetIntArrayElements(env, texImageInt, NULL);
    jint *clutterImageIntBody = (*env)->GetIntArrayElements(env, clutterImageInt, NULL);
    jint *cellImageIntBody = (*env)->GetIntArrayElements(env, cellImageInt, NULL);
    jsize nGlobal = (*env)->GetArrayLength(env, texImageInt);
    // end of Java Native Interface tricks

    unsigned char dbzImageBody[nGlobal];
    unsigned char vradImageBody[nGlobal];
    unsigned char texImageBody[nGlobal];
    unsigned char clutterImageBody[nGlobal];
    unsigned char cmFlag;
    unsigned char dualPolFlag;
    unsigned char verbose;


    int nAzim = dbznAzim;
    int nRang = dbznRang;
    int iAzim;
    int iRang;
    int iGlobal;
    int nCellsValid;

    for (iAzim = 0;iAzim<nAzim;iAzim++){
        for (iRang= 0 ; iRang<nRang;iRang++){

            iGlobal = iAzim*nRang + iRang;

            if (0<=dbzImageIntBody[iGlobal] && dbzImageIntBody[iGlobal]<=255) {
                dbzImageBody[iGlobal] = (unsigned char) dbzImageIntBody[iGlobal];
            }
            else {
                fprintf(stderr,"Error converting type (dbzImageIntBody[iGlobal]).");
                return -1;
            }

            if (0<=vradImageIntBody[iGlobal] && vradImageIntBody[iGlobal]<=255) {
                vradImageBody[iGlobal] = (unsigned char) vradImageIntBody[iGlobal];
            }
            else {
                fprintf(stderr,"Error converting type (vradImageIntBody[iGlobal]).");
                return -1;
            }

            if (0<=texImageIntBody[iGlobal] && texImageIntBody[iGlobal]<=255) {
                texImageBody[iGlobal] = (unsigned char) texImageIntBody[iGlobal];
            }
            else {
                fprintf(stderr,"Error converting type (texImageIntBody[iGlobal]).");
                return -1;
            }

            if (0<=clutterImageIntBody[iGlobal] && clutterImageIntBody[iGlobal]<=255) {
                clutterImageBody[iGlobal] = (unsigned char) clutterImageIntBody[iGlobal];
            }
            else {
                fprintf(stderr,"Error converting type (clutterImageIntBody[iGlobal]).");
                return -1;
            }

        }
    }


    SCANMETA dbzMeta;
    SCANMETA vradMeta;
    SCANMETA texMeta;
    SCANMETA clutterMeta;

    dbzMeta.nRang = dbznRang;
    dbzMeta.nAzim = dbznAzim;
    dbzMeta.elev = dbzElev;

    dbzMeta.valueScale = dbzValueScale;
    dbzMeta.valueOffset = dbzValueOffset;

    vradMeta.valueScale = vradValueScale;
    vradMeta.valueOffset = vradValueOffset;

    clutterMeta.valueScale = clutterValueScale;
    clutterMeta.valueOffset = clutterValueOffset;

    texMeta.valueScale = texValueScale;
    texMeta.valueOffset = texValueOffset;


    // cast to unsigned char
    if (0<=cmFlagInt && cmFlagInt<=255) {
        cmFlag = (unsigned char) cmFlagInt;
    }
    else {
        fprintf(stderr,"Error converting type.");
        return -1;
    }

    // cast to unsigned char
    if (0<=dualPolFlagInt && dualPolFlagInt<=255) {
        dualPolFlag = (unsigned char) dualPolFlagInt;
    }
    else {
        fprintf(stderr,"Error converting type.");
        return -1;
    }

    // cast to unsigned char
    if (0<=verboseInt && verboseInt<=255) {
        verbose = (unsigned char) verboseInt;
    }
    else {
        fprintf(stderr,"Error converting type.");
        return -1;
    }



    nCellsValid = analysecells(&dbzImageBody[0], &vradImageBody[0], &texImageBody[0],
                               &clutterImageBody[0], cellImageIntBody,
                               &dbzMeta, &vradMeta, &texMeta, &clutterMeta,
                               nCells, areaMin, cellDbzMin, cellStdDevMax, cellClutterFraction,
                               vradMinValue, dbzClutterMin, cmFlag,
                               dualPolFlag, verbose);




    // cast to the right type:
    for (iAzim = 0;iAzim<nAzim;iAzim++){
        for (iRang= 0 ; iRang<nRang;iRang++){
            iGlobal = iAzim*nRang + iRang;

            dbzImageIntBody[iGlobal] = (jint) dbzImageBody[iGlobal];
            vradImageIntBody[iGlobal] = (jint) vradImageBody[iGlobal];
            texImageIntBody[iGlobal] = (jint) texImageBody[iGlobal];
            clutterImageIntBody[iGlobal] = (jint) clutterImageBody[iGlobal];

        }
    }


    // do some Java Native interface tricks:
    (*env)->ReleaseIntArrayElements(env, dbzImageInt, dbzImageIntBody, JNI_ABORT);  // FIXME maybe don't use ABORT?
    (*env)->ReleaseIntArrayElements(env, vradImageInt, vradImageIntBody, JNI_ABORT);  // FIXME maybe don't use ABORT?
    (*env)->ReleaseIntArrayElements(env, texImageInt, texImageIntBody, JNI_ABORT);  // FIXME maybe don't use ABORT?
    (*env)->ReleaseIntArrayElements(env, clutterImageInt, clutterImageIntBody, JNI_ABORT);  // FIXME maybe don't use ABORT?
    (*env)->ReleaseIntArrayElements(env, cellImageInt, cellImageIntBody, 0);
    // end of Java Native Interface tricks


    return nCellsValid;



}






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




JNIEXPORT void JNICALL
Java_nl_esciencecenter_ncradar_JNIMethodsVol2Bird_calcTexture(
        JNIEnv *env,
        jobject obj,
        jintArray texImageInt,
        jintArray reflImageInt,
        jintArray vradImageInt,
        jint nRangNeighborhoodInt,
        jint nAzimNeighborhoodInt,
        jint nCountMinInt,
        jint texTypeInt,
        jfloat texOffset,
        jfloat texScale,
        jfloat reflOffset,
        jfloat reflScale,
        jfloat vradOffset,
        jfloat vradScale,
        jint nRang,
        jint nAzim) {

    // do some Java Native interface tricks:
    jint *texImageIntBody = (*env)->GetIntArrayElements(env, texImageInt, NULL);
    jint *reflImageIntBody = (*env)->GetIntArrayElements(env, reflImageInt, NULL);
    jint *vradImageIntBody = (*env)->GetIntArrayElements(env, vradImageInt, NULL);
    jsize nGlobal = (*env)->GetArrayLength(env, texImageInt);
    // end of Java Native Interface tricks

    unsigned char texImageUCharBody[nGlobal];
    unsigned char reflImageUCharBody[nGlobal];
    unsigned char vradImageUCharBody[nGlobal];

    int iGlobal;

    for (iGlobal = 0; iGlobal < nGlobal; iGlobal++){

        if (0<=texImageIntBody[iGlobal] && texImageIntBody[iGlobal]<=255) {
            texImageUCharBody[iGlobal] = (unsigned char) texImageIntBody[iGlobal];
        }
        else {
            fprintf(stderr,"Error converting type (texImageIntBody[iGlobal]).");
            return;
        }

        if (0<=reflImageIntBody[iGlobal] && reflImageIntBody[iGlobal]<=255) {
            reflImageUCharBody[iGlobal] = (unsigned char) reflImageIntBody[iGlobal];
        }
        else {
            fprintf(stderr,"Error converting type (reflImageIntBody[iGlobal]).");
            return;
        }

        if (0<=vradImageIntBody[iGlobal] && vradImageIntBody[iGlobal]<=255) {
            vradImageUCharBody[iGlobal] = (unsigned char) vradImageIntBody[iGlobal];
        }
        else {
            fprintf(stderr,"Error converting type (vradImageIntBody[iGlobal]).");
            return;
        }
    }

    unsigned char nRangNeighborhoodUChar;
    unsigned char nAzimNeighborhoodUChar;
    unsigned char nCountMinUChar;
    unsigned char texTypeUChar;

    if (0<=nRangNeighborhoodInt && nRangNeighborhoodInt<=255) {
        nRangNeighborhoodUChar = (unsigned char) nRangNeighborhoodInt;
    }
    else {
        fprintf(stderr,"Error converting type (nRangNeighborhoodInt).");
        return;
    }


    if (0<=nAzimNeighborhoodInt && nAzimNeighborhoodInt<=255) {
        nAzimNeighborhoodUChar = (unsigned char) nAzimNeighborhoodInt;
    }
    else {
        fprintf(stderr,"Error converting type (nAzimNeighborhoodInt).");
        return;
    }

    if (0<=nCountMinInt && nCountMinInt<=255) {
        nCountMinUChar = (unsigned char) nCountMinInt;
    }
    else {
        fprintf(stderr,"Error converting type (nCountMinInt).");
        return;
    }

    if (0<=texTypeInt && texTypeInt<=255) {
        texTypeUChar = (unsigned char) texTypeInt;
    }
    else {
        fprintf(stderr,"Error converting type (texTypeInt).");
        return;
    }



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



    texture(texImageUCharBody, vradImageUCharBody, reflImageUCharBody,
            &texMeta, &vradMeta, &reflMeta,
            nRangNeighborhoodUChar, nAzimNeighborhoodUChar,
            nCountMinUChar, texTypeUChar);

//    int iElem;
//
//    for (iElem=0; iElem<12;iElem++) {
//        fprintf(stderr,"(C) ");
//        fprintf(stderr,"%d, ",texImageBody[iElem]);
//        fprintf(stderr,"%d, ",reflImageBody[iElem]);
//        fprintf(stderr,"%d",vradImageBody[iElem]);
//        fprintf(stderr,"\n");
//    }



    // cast back to integer type:
    for (iGlobal = 0; iGlobal < nGlobal; iGlobal++) {
        texImageIntBody[iGlobal] = (jint) texImageUCharBody[iGlobal];
        vradImageIntBody[iGlobal] = (jint) vradImageUCharBody[iGlobal];
        reflImageIntBody[iGlobal] = (jint) reflImageUCharBody[iGlobal];
    }



    // do some Java Native interface tricks:
    (*env)->ReleaseIntArrayElements(env, texImageInt, texImageIntBody, 0);
    (*env)->ReleaseIntArrayElements(env, reflImageInt, reflImageIntBody, JNI_ABORT);
    (*env)->ReleaseIntArrayElements(env, vradImageInt, vradImageIntBody, JNI_ABORT);
    // end of Java Native Interface tricks

}






JNIEXPORT jfloat JNICALL
Java_nl_esciencecenter_ncradar_JNIMethodsVol2Bird_classify(
JNIEnv *env,
jobject obj,
jint dbznRang,
jint dbznAzim,
jfloat dbzRangeScale,
jfloat dbzElev,
jfloat dbzHeig,
jfloat dbzValueScale,
jfloat dbzValueOffset,
jfloat dbzAzimScale,
jint dbzMissing,
jfloat vradValueScale,
jfloat vradValueOffset,
jint vradMissing,
jint rawReflMissing,
jfloat clutterValueScale,
jfloat clutterValueOffset,
jintArray cellImage,
jintArray dbzImageInt,
jintArray vradImageInt,
jintArray rawReflImageInt,
jintArray clutterImageInt,
jfloatArray zdata,
jintArray nzdata,
jfloat rangeMin,
jfloat rangeMax,
jfloat HLAYER,
jfloat XOFFSET,
jfloat XSCALE,
jfloat XMEAN,
jfloat height,
jfloat azimMin,
jfloat azimMax,
jfloat vradMin,
jfloat dbzClutter,
jfloat dbzMin,
jfloat dBZx,
jfloat DBZNOISE,
jint NGAPMIN,
jint NGAPBIN,
jint NDBZMIN,
jint layer,
jint id,
jint np,
jint nPointsPtr,
jint nPointsAllPtr,
jint nPointsClutterPtr,
jint nPointsRainPtr,
jint nPointsRainNoFringePtr,
jint clutterFlagInt,
jint rawReflFlagInt,
jint xflagInt
)
{

//    fprintf(stderr, "hello jni\n");
//    fprintf(stderr, "dbznRang = %d\n", dbznRang);
//    fprintf(stderr, "dbznAzim = %d\n", dbznAzim);
//    fprintf(stderr, "dbzRangeScale = %f\n", dbzRangeScale);
//    fprintf(stderr, "dbzElev = %f\n", dbzElev);
//    fprintf(stderr, "dbzHeig = %f\n", dbzHeig);
//    fprintf(stderr, "dbzValueScale = %f\n", dbzValueScale);
//    fprintf(stderr, "dbzValueOffset = %f\n", dbzValueOffset);
//    fprintf(stderr, "dbzAzimScale = %f\n", dbzAzimScale);
//    fprintf(stderr, "dbzMissing = %d\n", dbzMissing);
//    fprintf(stderr, "vradValueScale = %f\n", vradValueScale);
//    fprintf(stderr, "vradValueOffset = %f\n", vradValueOffset);
//    fprintf(stderr, "vradMissing = %d\n", vradMissing);
//    fprintf(stderr, "rawReflMissing = %d\n", rawReflMissing);
//    fprintf(stderr, "clutterValueScale = %f\n", clutterValueScale);
//    fprintf(stderr, "clutterValueOffset = %f\n", clutterValueOffset);
//    fprintf(stderr, "cellImage = %p\n", cellImage);


    // do some Java Native interface tricks:
    jint *cellImageBody = (*env)->GetIntArrayElements(env, cellImage, NULL);
    jint *dbzImageIntBody = (*env)->GetIntArrayElements(env, dbzImageInt, NULL);
    jint *vradImageIntBody = (*env)->GetIntArrayElements(env, vradImageInt, NULL);
    jint *rawReflImageIntBody = (*env)->GetIntArrayElements(env, rawReflImageInt, NULL);
    jint *clutterImageIntBody = (*env)->GetIntArrayElements(env, clutterImageInt, NULL);
    jfloat *zdataBody = (*env)->GetFloatArrayElements(env, zdata, NULL);
    jint *nzdataBody = (*env)->GetIntArrayElements(env, nzdata, NULL);
    jsize nGlobal = (*env)->GetArrayLength(env, cellImage);
    // end of Java Native Interface tricks


    unsigned char dbzImageBody[nGlobal];
    unsigned char vradImageBody[nGlobal];
    unsigned char rawReflImageBody[nGlobal];
    unsigned char clutterImageBody[nGlobal];
    unsigned char clutterFlag;
    unsigned char rawReflFlag;
    unsigned char xflag;

    float *fracclut;
    float *fracrain;
    float *fracbird;
    float *fracfringe;

    int iGlobal;


    for (iGlobal = 0; iGlobal < nGlobal; iGlobal++){

            if (0<=dbzImageIntBody[iGlobal] && dbzImageIntBody[iGlobal]<=255) {
                dbzImageBody[iGlobal] = (unsigned char) dbzImageIntBody[iGlobal];
            }
            else {
                fprintf(stderr,"Error converting type (dbzImageIntBody[iGlobal]).");
                return -1;
            }

            if (0<=vradImageIntBody[iGlobal] && vradImageIntBody[iGlobal]<=255) {
                vradImageBody[iGlobal] = (unsigned char) vradImageIntBody[iGlobal];
            }
            else {
                fprintf(stderr,"Error converting type (vradImageIntBody[iGlobal]).");
                return -1;
            }

            if (0<=rawReflImageIntBody[iGlobal] && rawReflImageIntBody[iGlobal]<=255) {
                rawReflImageBody[iGlobal] = (unsigned char) rawReflImageIntBody[iGlobal];
            }
            else {
                fprintf(stderr,"Error converting type (rawReflImageIntBody[iGlobal]).");
                return -1;
            }

            if (0<=clutterImageIntBody[iGlobal] && clutterImageIntBody[iGlobal]<=255) {
                clutterImageBody[iGlobal] = (unsigned char) clutterImageIntBody[iGlobal];
            }
            else {
                fprintf(stderr,"Error converting type (clutterImageIntBody[iGlobal]).");
                return -1;
            }

    }

    SCANMETA dbzMeta;
    SCANMETA vradMeta;
    SCANMETA rawReflMeta;
    SCANMETA clutterMeta;

    dbzMeta.nRang = dbznRang;
    dbzMeta.nAzim = dbznAzim;
    dbzMeta.rangeScale = dbzRangeScale;
    dbzMeta.elev = dbzElev;
    dbzMeta.heig = dbzHeig;
    dbzMeta.valueScale = dbzValueScale;
    dbzMeta.valueOffset = dbzValueOffset;
    dbzMeta.azimScale = dbzAzimScale;
    dbzMeta.missing = dbzMissing;

    vradMeta.valueScale = vradValueScale;
    vradMeta.valueOffset = vradValueOffset;
    vradMeta.missing = vradMissing;

    rawReflMeta.missing = rawReflMissing;

    clutterMeta.valueScale = clutterValueScale;
    clutterMeta.valueOffset = clutterValueOffset;



    // cast to unsigned char
    if (0<=clutterFlagInt && clutterFlagInt<=255) {
        clutterFlag = (unsigned char) clutterFlagInt;
    }
    else {
        fprintf(stderr,"Error converting type.");
        return -1;
    }

    // cast to unsigned char
    if (0<=rawReflFlagInt && rawReflFlagInt<=255) {
        rawReflFlag = (unsigned char) rawReflFlagInt;
    }
    else {
        fprintf(stderr,"Error converting type.");
        return -1;
    }

    // cast to unsigned char
    if (0<=xflagInt && xflagInt<=255) {
        xflag = (unsigned char) xflagInt;
    }
    else {
        fprintf(stderr,"Error converting type.");
        return -1;
    }


    fprintf(stderr,"%d@%p\n",nPointsClutterPtr,&nPointsClutterPtr);

    classification(dbzMeta, vradMeta, rawReflMeta,
            clutterMeta, cellImageBody, &dbzImageBody[0], &vradImageBody[0],
            &rawReflImageBody[0], &clutterImageBody[0],
            &zdataBody[0], &nzdataBody[0],
            fracclut, fracrain, fracbird, fracfringe,
            rangeMin, rangeMax, HLAYER, XOFFSET,
            XSCALE, XMEAN, height,
            azimMin, azimMax, vradMin, dbzClutter, dbzMin,
            dBZx, DBZNOISE, NGAPMIN, NGAPBIN, NDBZMIN,
            layer, id, &np, &nPointsPtr, &nPointsAllPtr, &nPointsClutterPtr,
            &nPointsRainPtr, &nPointsRainNoFringePtr,
            clutterFlag, rawReflFlag, xflag);

    fprintf(stderr,"%d@%p\n",nPointsClutterPtr,&nPointsClutterPtr);

    // cast back to integer type:
    for (iGlobal = 0; iGlobal < nGlobal; iGlobal++) {
        dbzImageIntBody[iGlobal] = (jint) dbzImageBody[iGlobal];
        vradImageIntBody[iGlobal] = (jint) vradImageBody[iGlobal];
        rawReflImageIntBody[iGlobal] = (jint) rawReflImageBody[iGlobal];
        clutterImageIntBody[iGlobal] = (jint) clutterImageBody[iGlobal];
    }


    // do some Java Native interface tricks:
    (*env)->ReleaseIntArrayElements(env, cellImage, cellImageBody, JNI_ABORT);
    (*env)->ReleaseIntArrayElements(env, dbzImageInt, dbzImageIntBody, JNI_ABORT);
    (*env)->ReleaseIntArrayElements(env, vradImageInt, vradImageIntBody, JNI_ABORT);
    (*env)->ReleaseIntArrayElements(env, rawReflImageInt, rawReflImageIntBody, JNI_ABORT);
    (*env)->ReleaseIntArrayElements(env, clutterImageInt, clutterImageIntBody, JNI_ABORT);
    (*env)->ReleaseFloatArrayElements(env, zdata, zdataBody, JNI_ABORT);
    (*env)->ReleaseIntArrayElements(env, nzdata, nzdataBody, JNI_ABORT);
    // end of Java Native Interface tricks

    return;

}








JNIEXPORT jint JNICALL
Java_nl_esciencecenter_ncradar_JNIMethodsVol2Bird_findCells(
        JNIEnv *env,
        jobject obj,
        jintArray texImageInt,
        jintArray rhoImageInt,
        jintArray zdrImageInt,
        jintArray cellImageInt,
        jint texMissing,
        jint texnAzim,
        jint texnRang,
        jfloat texValueOffset,
        jfloat texValueScale,
        jfloat texRangeScale,
        jfloat texThresMin,
        jint rhoMissing,
        jint rhonAzim,
        jint rhonRang,
        jfloat rhoValueOffset,
        jfloat rhoValueScale,
        jfloat rhoThresMin,
        jint zdrMissing,
        jint zdrnAzim,
        jint zdrnRang,
        jfloat zdrValueOffset,
        jfloat zdrValueScale,
        jfloat zdrThresMin,
        jfloat dbzThresMin,
        jfloat rCellMax,
        jint signInt)
{

    int nAzim = texnAzim;
    int nRang = texnRang;
    int iAzim;
    int iRang;
    int iGlobal;
    int nGlobal;

    int rhoImageIsNull;
    int zdrImageIsNull;

    if (rhoImageInt == NULL) {
        fprintf(stderr,"rhoImage is null.\n");
        rhoImageIsNull = 1;
    }
    else {
        rhoImageIsNull = 0;
    }

    if (zdrImageInt == NULL) {
        fprintf(stderr,"zdrImage is null.\n");
        zdrImageIsNull = 1;
    }
    else {
        zdrImageIsNull = 0;
    }


    nGlobal = nAzim * nRang;

    // do some Java Native interface tricks:

    jint *texImageIntBody = (*env)->GetIntArrayElements(env, texImageInt, NULL);
    unsigned char texImageBody[nGlobal];
    for (iAzim = 0; iAzim < nAzim; iAzim++){
         for (iRang= 0 ; iRang<nRang;iRang++){
             iGlobal = iAzim*nRang + iRang;
             if (0<=texImageIntBody[iGlobal] && texImageIntBody[iGlobal]<=255) {
                 texImageBody[iGlobal] = (unsigned char) texImageIntBody[iGlobal];
             }
             else {
                 fprintf(stderr,"Error converting type (texImageIntBody[iGlobal]).");
                 return -1;
             }
         }
     }


    unsigned char rhoImageBody[nGlobal];
    jint *rhoImageIntBody;
    if (rhoImageInt != NULL) {
        rhoImageIntBody = (*env)->GetIntArrayElements(env, rhoImageInt, NULL);
        for (iAzim = 0;iAzim<nAzim;iAzim++){
            for (iRang= 0 ; iRang<nRang;iRang++){
                iGlobal = iAzim*nRang + iRang;
                if (0<=rhoImageIntBody[iGlobal] && rhoImageIntBody[iGlobal]<=255) {
                    rhoImageBody[iGlobal] = (unsigned char) rhoImageIntBody[iGlobal];
                }
                else {
                    fprintf(stderr,"Error converting type (rhoImageIntBody[iGlobal]).");
                    return -1;
                }
            }
        }
    }

    unsigned char zdrImageBody[nGlobal];
    jint *zdrImageIntBody;
    if (zdrImageInt != NULL) {
        zdrImageIntBody = (*env)->GetIntArrayElements(env, zdrImageInt, NULL);
        for (iAzim = 0;iAzim<nAzim;iAzim++){
            for (iRang= 0 ; iRang<nRang;iRang++){
                iGlobal = iAzim*nRang + iRang;
                if (0<=zdrImageIntBody[iGlobal] && zdrImageIntBody[iGlobal]<=255) {
                    zdrImageBody[iGlobal] = (unsigned char) zdrImageIntBody[iGlobal];
                }
                else {
                    fprintf(stderr,"Error converting type (zdrImageIntBody[iGlobal]).");
                    return -1;
                }
            }
        }
    }


    jint *cellImageIntBody = (*env)->GetIntArrayElements(env, cellImageInt, NULL);
    // end of Java Native Interface tricks


    unsigned char sign;



    // Allocating and initializing memory for cell properties.

    SCANMETA texMeta;
    SCANMETA rhoMeta;
    SCANMETA zdrMeta;

    texMeta.missing = texMissing;
    texMeta.nAzim = texnAzim;
    texMeta.nRang = texnRang;
    texMeta.valueOffset = texValueOffset;
    texMeta.valueScale = texValueScale;
    texMeta.rangeScale = texRangeScale;

    rhoMeta.missing = rhoMissing;
    rhoMeta.nAzim = rhonAzim;
    rhoMeta.nRang = rhonRang;
    rhoMeta.valueOffset = rhoValueOffset;
    rhoMeta.valueScale = rhoValueScale;

    zdrMeta.missing = zdrMissing;
    zdrMeta.nAzim = zdrnAzim;
    zdrMeta.nRang = zdrnRang;
    zdrMeta.valueOffset = zdrValueOffset;
    zdrMeta.valueScale = zdrValueScale;

    int nCells;

    if (-128<=signInt && signInt<=127) {
        sign = (char) signInt;
    }
    else {
        fprintf(stderr,"Error converting type.");
        return -1;
    }

    if (rhoImageIsNull == 1 && zdrImageIsNull == 0) {
            nCells = findcells(&texImageBody[0], NULL, &zdrImageBody[0], &cellImageIntBody[0],
                               &texMeta,     &rhoMeta,     &zdrMeta,
                               texThresMin,  rhoThresMin,  zdrThresMin,
                               dbzThresMin, rCellMax, sign);
    }
    if (rhoImageIsNull == 0 && zdrImageIsNull == 1) {
            nCells = findcells(&texImageBody[0], &rhoImageBody[0], NULL, &cellImageIntBody[0],
                               &texMeta,     &rhoMeta,     &zdrMeta,
                               texThresMin,  rhoThresMin,  zdrThresMin,
                               dbzThresMin, rCellMax, sign);
    }
    if (rhoImageIsNull == 1 && zdrImageIsNull == 1) {
            nCells = findcells(&texImageBody[0], NULL, NULL, &cellImageIntBody[0],
                               &texMeta,     &rhoMeta,     &zdrMeta,
                               texThresMin,  rhoThresMin,  zdrThresMin,
                               dbzThresMin, rCellMax, sign);
    }





    for (iAzim = 0; iAzim < nAzim; iAzim++){
        for (iRang= 0 ; iRang<nRang;iRang++){

            iGlobal = iAzim*nRang + iRang;

            texImageIntBody[iGlobal] = (jint) texImageBody[iGlobal];
            if (rhoImageIsNull == 0) {
                rhoImageIntBody[iGlobal] = (jint) rhoImageBody[iGlobal];
            }
            if (zdrImageIsNull == 0) {
                zdrImageIntBody[iGlobal] = (jint) zdrImageBody[iGlobal];
            }
        }
    }



    // do some Java Native interface tricks:
    (*env)->ReleaseIntArrayElements(env, cellImageInt, cellImageIntBody, 0);
    (*env)->ReleaseIntArrayElements(env, texImageInt, texImageIntBody, JNI_ABORT);
    if (rhoImageIsNull == 0) {
        (*env)->ReleaseIntArrayElements(env, rhoImageInt, rhoImageIntBody, JNI_ABORT);
    }
    if (zdrImageIsNull == 0) {
        (*env)->ReleaseIntArrayElements(env, zdrImageInt, zdrImageIntBody, JNI_ABORT);
    }
    // end of Java Native Interface tricks


    return nCells;


}






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








JNIEXPORT void JNICALL
Java_nl_esciencecenter_ncradar_JNIMethodsVol2Bird_sortCells(
JNIEnv *env,
jobject obj,
jintArray cellPropIRangOfMax,
jintArray cellPropIAzimOfMax,
jfloatArray cellPropDbzAvg,
jfloatArray cellPropTexAvg,
jfloatArray cellPropCv,
jfloatArray cellPropArea,
jfloatArray cellPropClutterArea,
jfloatArray cellPropDbzMax,
jintArray cellPropIndex,
jcharArray cellPropDrop,
jint nCells,
jint method
)
{

    // do some Java Native interface tricks:

    // these arrays have nCells elements:
    jint *cellPropIRangOfMaxBody = (*env)->GetIntArrayElements(env, cellPropIRangOfMax, NULL);
    jint *cellPropIAzimOfMaxBody = (*env)->GetIntArrayElements(env, cellPropIAzimOfMax, NULL);
    jfloat *cellPropDbzAvgBody = (*env)->GetFloatArrayElements(env, cellPropDbzAvg, NULL);
    jfloat *cellPropTexAvgBody = (*env)->GetFloatArrayElements(env, cellPropTexAvg, NULL);
    jfloat *cellPropCvBody = (*env)->GetFloatArrayElements(env, cellPropCv, NULL);
    jfloat *cellPropAreaBody = (*env)->GetFloatArrayElements(env, cellPropArea, NULL);
    jfloat *cellPropClutterAreaBody = (*env)->GetFloatArrayElements(env, cellPropClutterArea, NULL);
    jfloat *cellPropDbzMaxBody = (*env)->GetFloatArrayElements(env, cellPropDbzMax, NULL);
    jint *cellPropIndexBody = (*env)->GetIntArrayElements(env, cellPropIndex, NULL);
    jchar *cellPropDropBody = (*env)->GetCharArrayElements(env, cellPropDrop, NULL);
    // end of Java Native Interface tricks

    int iCell;

    CELLPROP *cellProp;
    /*Allocating and initializing memory for cell properties.*/
    cellProp = (CELLPROP *)malloc(nCells*sizeof(CELLPROP));

    // construct the CELLPROP struct
    for (iCell = 0; iCell<nCells;iCell++) {
        cellProp[iCell].iRangOfMax = cellPropIRangOfMaxBody[iCell];
        cellProp[iCell].iAzimOfMax = cellPropIAzimOfMaxBody[iCell];
        cellProp[iCell].dbzAvg = cellPropDbzAvgBody[iCell];
        cellProp[iCell].texAvg = cellPropTexAvgBody[iCell];
        cellProp[iCell].cv = cellPropCvBody[iCell];
        cellProp[iCell].area = cellPropAreaBody[iCell];
        cellProp[iCell].clutterArea = cellPropClutterAreaBody[iCell];
        cellProp[iCell].dbzMax = cellPropDbzMaxBody[iCell];
        cellProp[iCell].index = cellPropIndexBody[iCell];
        cellProp[iCell].drop = cellPropDropBody[iCell];
    }

    for (iCell=0;iCell<nCells;iCell++) {
        if (method == BYAREA) {
            fprintf(stderr,"B: cellProp[%d].area = %f\n",iCell,cellProp[iCell].area);
        }
        if (method == BYMEAN) {
            fprintf(stderr,"B: cellProp[%d].dbzAvg = %f\n",iCell,cellProp[iCell].dbzAvg);
        }
    }

    sortcells(cellProp,nCells,method);

    for (iCell=0;iCell<nCells;iCell++) {
        if (method == BYAREA) {
            fprintf(stderr,"A: cellProp[%d].area = %f\n",iCell,cellProp[iCell].area);
        }
        if (method == BYMEAN) {
            fprintf(stderr,"A: cellProp[%d].dbzAvg = %f\n",iCell,cellProp[iCell].dbzAvg);
        }
    }

    // deconstruct the CELLPROP struct
    for (iCell = 0; iCell<nCells;iCell++) {
        cellPropIRangOfMaxBody[iCell] = cellProp[iCell].iRangOfMax;
        cellPropIAzimOfMaxBody[iCell] = cellProp[iCell].iAzimOfMax;
        cellPropDbzAvgBody[iCell] = cellProp[iCell].dbzAvg;
        cellPropTexAvgBody[iCell] = cellProp[iCell].texAvg;
        cellPropCvBody[iCell] = cellProp[iCell].cv;
        cellPropAreaBody[iCell] = cellProp[iCell].area;
        cellPropClutterAreaBody[iCell] = cellProp[iCell].clutterArea;
        cellPropDbzMaxBody[iCell] = cellProp[iCell].dbzMax;
        cellPropIndexBody[iCell] = cellProp[iCell].index;
        cellPropDropBody[iCell] = cellProp[iCell].drop;

    }



    // do some more Java Native Interface tricks:
    (*env)->ReleaseIntArrayElements(env, cellPropIRangOfMax, cellPropIRangOfMaxBody, 0);
    (*env)->ReleaseIntArrayElements(env, cellPropIAzimOfMax, cellPropIAzimOfMaxBody, 0);
    (*env)->ReleaseFloatArrayElements(env, cellPropDbzAvg, cellPropDbzAvgBody, 0);
    (*env)->ReleaseFloatArrayElements(env, cellPropTexAvg, cellPropTexAvgBody, 0);
    (*env)->ReleaseFloatArrayElements(env, cellPropCv, cellPropCvBody, 0);
    (*env)->ReleaseFloatArrayElements(env, cellPropArea, cellPropAreaBody, 0);
    (*env)->ReleaseFloatArrayElements(env, cellPropClutterArea, cellPropClutterAreaBody, 0);
    (*env)->ReleaseFloatArrayElements(env, cellPropDbzMax, cellPropDbzMaxBody, 0);
    (*env)->ReleaseIntArrayElements(env, cellPropIndex, cellPropIndexBody, 0);
    (*env)->ReleaseCharArrayElements(env, cellPropDrop, cellPropDropBody, 0);
    // end of Java Native Interface tricks


    // free up the memory that we allocated earlier
    free(cellProp);
    cellProp = NULL;



}








JNIEXPORT jint JNICALL
Java_nl_esciencecenter_ncradar_JNIMethodsVol2Bird_updateMap(
JNIEnv *env,
jobject obj,
jintArray cellImage,
jintArray cellPropIRangOfMax,
jintArray cellPropIAzimOfMax,
jfloatArray cellPropDbzAvg,
jfloatArray cellPropTexAvg,
jfloatArray cellPropCv,
jfloatArray cellPropArea,
jfloatArray cellPropClutterArea,
jfloatArray cellPropDbzMax,
jintArray cellPropIndex,
jcharArray cellPropDrop,
jint nCells,
jint nGlobal,
jint minCellArea)
{

    // do some Java Native interface tricks:
    // arrays with nGlobal elements:
    jint *cellImageBody = (*env)->GetIntArrayElements(env, cellImage, NULL);
    // arrays with nCells elements:
    jint *cellPropIRangOfMaxBody = (*env)->GetIntArrayElements(env, cellPropIRangOfMax, NULL);
    jint *cellPropIAzimOfMaxBody = (*env)->GetIntArrayElements(env, cellPropIAzimOfMax, NULL);
    jfloat *cellPropDbzAvgBody = (*env)->GetFloatArrayElements(env, cellPropDbzAvg, NULL);
    jfloat *cellPropTexAvgBody = (*env)->GetFloatArrayElements(env, cellPropTexAvg, NULL);
    jfloat *cellPropCvBody = (*env)->GetFloatArrayElements(env, cellPropCv, NULL);
    jfloat *cellPropAreaBody = (*env)->GetFloatArrayElements(env, cellPropArea, NULL);
    jfloat *cellPropClutterAreaBody = (*env)->GetFloatArrayElements(env, cellPropClutterArea, NULL);
    jfloat *cellPropDbzMaxBody = (*env)->GetFloatArrayElements(env, cellPropDbzMax, NULL);
    jint *cellPropIndexBody = (*env)->GetIntArrayElements(env, cellPropIndex, NULL);
    jchar *cellPropDropBody = (*env)->GetCharArrayElements(env, cellPropDrop, NULL);
    // end of Java Native Interface tricks

    int iCell;
    int nCellsValid;

    CELLPROP *cellProp;
    /*Allocating and initializing memory for cell properties.*/
    cellProp = (CELLPROP *)malloc(nCells*sizeof(CELLPROP));

    // construct the CELLPROP struct
    for (iCell = 0; iCell<nCells;iCell++) {
        cellProp[iCell].iRangOfMax = cellPropIRangOfMaxBody[iCell];
        cellProp[iCell].iAzimOfMax = cellPropIAzimOfMaxBody[iCell];
        cellProp[iCell].dbzAvg = cellPropDbzAvgBody[iCell];
        cellProp[iCell].texAvg = cellPropTexAvgBody[iCell];
        cellProp[iCell].cv = cellPropCvBody[iCell];
        cellProp[iCell].area = cellPropAreaBody[iCell];
        cellProp[iCell].clutterArea = cellPropClutterAreaBody[iCell];
        cellProp[iCell].dbzMax = cellPropDbzMaxBody[iCell];
        cellProp[iCell].index = cellPropIndexBody[iCell];
        cellProp[iCell].drop = cellPropDropBody[iCell];
    }

    nCellsValid = updatemap(cellImageBody,cellProp,nCells,nGlobal,minCellArea);


    // deconstruct the CELLPROP struct
    for (iCell = 0; iCell<nCells;iCell++) {
        cellPropIRangOfMaxBody[iCell] = cellProp[iCell].iRangOfMax;
        cellPropIAzimOfMaxBody[iCell] = cellProp[iCell].iAzimOfMax;
        cellPropDbzAvgBody[iCell] = cellProp[iCell].dbzAvg;
        cellPropTexAvgBody[iCell] = cellProp[iCell].texAvg;
        cellPropCvBody[iCell] = cellProp[iCell].cv;
        cellPropAreaBody[iCell] = cellProp[iCell].area;
        cellPropClutterAreaBody[iCell] = cellProp[iCell].clutterArea;
        cellPropDbzMaxBody[iCell] = cellProp[iCell].dbzMax;
        cellPropIndexBody[iCell] = cellProp[iCell].index;
        cellPropDropBody[iCell] = cellProp[iCell].drop;
    }


    // do some Java Native interface tricks:
    // arrays with nGlobal elements:
    (*env)->ReleaseIntArrayElements(env, cellImage, cellImageBody, 0);
    // arrays with nCells elements:
    (*env)->ReleaseIntArrayElements(env, cellPropIRangOfMax, cellPropIRangOfMaxBody, 0);
    (*env)->ReleaseIntArrayElements(env, cellPropIAzimOfMax, cellPropIAzimOfMaxBody, 0);
    (*env)->ReleaseFloatArrayElements(env, cellPropDbzAvg, cellPropDbzAvgBody, 0);
    (*env)->ReleaseFloatArrayElements(env, cellPropTexAvg, cellPropTexAvgBody, 0);
    (*env)->ReleaseFloatArrayElements(env, cellPropCv, cellPropCvBody, 0);
    (*env)->ReleaseFloatArrayElements(env, cellPropArea, cellPropAreaBody, 0);
    (*env)->ReleaseFloatArrayElements(env, cellPropClutterArea, cellPropClutterAreaBody, 0);
    (*env)->ReleaseFloatArrayElements(env, cellPropDbzMax, cellPropDbzMaxBody, 0);
    (*env)->ReleaseIntArrayElements(env, cellPropIndex, cellPropIndexBody, 0);
    (*env)->ReleaseCharArrayElements(env, cellPropDrop, cellPropDropBody, 0);
    // end of Java Native Interface tricks

    free(cellProp);
    cellProp = NULL;

    return nCellsValid;


}





