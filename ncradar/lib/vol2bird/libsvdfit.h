/*
 * Copyright 2013 Netherlands eScience Center
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */





// ****************************************************************************
// Definition of parameters for fitting
// ****************************************************************************

#define NPARSFITTEDMAX (16)      /*Maximum number of fit parameters.*/
#define SVDTOL       (1e-5)      /*Accuracy in SV decomposition.*/
#define SVDMIN       (1e-5)      /*Minimum valid SV standard deviation.*/




// *****************************************************************************
// Function prototypes
// *****************************************************************************

int svd_vad1func(float points[], int nDims, float afunc[], int nParsFitted);
int svd_vad2func(float points[], int nDims, float afunc[], int nParsFitted);
int svd_vvp1func(float points[], int nDims, float afunc[], int nParsFitted);
int svd_vvp2func(float points[], int nDims, float afunc[], int nParsFitted);
int svd_vvp3func(float points[], int nDims, float afunc[], int nParsFitted);
int svdcmp(float *a, int m, int n, float w[], float *v);
float svdfit(float *points, int nDims, float yObs[], float yFitted[], int nPoints,
        int (*funcs)(float points[], int nDims, float afunc[], int nParsFitted),
        float parameterVector[], float avar[], int nParsFitted);



