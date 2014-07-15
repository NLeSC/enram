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
// Functions for linear fitting using Singular Value Decomposition:
// ****************************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "libvol2bird.h"
#include "libsvdfit.h"



int svd_vad1func(float points[],int nDims,float afunc[],int nParsFitted) {

    // ************************************************************************************
    // This function contains the basis-functions and other relevant parameters of
    // the fit model. The function returns the basis functions of the
    // multi-parameter linear fit evaluated at vector X=points[0..nDims-1] in the array
    // afunc[0..nParsFitted-1].
    // This function is intended to be supplied to the fitting routine 'svdfit'.
    // In this case, a three-parameter linear VAD fit is performed on a one
    // dimensional vector space 'X'.
    // ************************************************************************************


    if (nDims != 1) {
        fprintf(stderr, "Number of dimensions is wrong!\n");
        return 0;   //FIXME return 1 or -1 would be better
    }
    if (nParsFitted != 3) {
        fprintf(stderr, "Number of parameters is wrong!\n");
        return 0;   //FIXME return 1 or -1 would be better
    }

    afunc[0] = 0.5;
    afunc[1] = sin(DEG2RAD * points[0]);
    afunc[2] = cos(DEG2RAD * points[0]);

    return 1;    // FIXME return 0 would be better

} //svd_vad1func





int svd_vad2func(float points[],int nDims,float afunc[],int nParsFitted) {

    // ************************************************************************************
    // This function contains the basis-functions and other relevant parameters of
    // the fit model. The function returns the basis functions of the
    // multi-parameter linear fit evaluated at vector X=points[0..nDims-1] in the array
    // afunc[0..nParsFitted-1].
    // This function is intended to be supplied to the fitting routine 'svdfit'.
    // In this case, a five-parameter linear VAD fit is performed on a one
    // dimensional vector space 'X'.
    // ************************************************************************************

    if (nDims != 1) {
        fprintf(stderr, "Number of dimensions is wrong!\n");
        return 0;   //FIXME return 1 or -1 would be better
    }
    if (nParsFitted != 5) {
        fprintf(stderr, "Number of parameters is wrong!\n");
        return 0;   //FIXME return 1 or -1 would be better
    }

    afunc[0] = 0.5;
    afunc[1] = sin(points[0] * DEG2RAD);
    afunc[2] = cos(points[0] * DEG2RAD);
    afunc[3] = sin(2*points[0] * DEG2RAD);
    afunc[4] = cos(2*points[0] * DEG2RAD);

    return 1;    // FIXME return 0 would be better

} //svd_vad2func






int svd_vvp1func(float points[],int nDims,float afunc[],int nParsFitted) {

    // ************************************************************************************
    // This function contains the basis-functions and other relevant parameters of
    // the fit model. The function returns the basis functions of the
    // multi-parameter linear fit evaluated at vector X=points[0..nDims-1] in the array
    // afunc[0..nParsFitted-1].
    // This function is intended to be supplied to the fitting routine 'svdfit'.
    // In this case, a three-parameter linear Area-VVP fit is performed on a two
    // dimensional vector space 'X' in radar coordinates: azimuth (alpha), and
    // elevation (gamma).
    // *************************************************************************************

    float sinAlpha;
    float cosAlpha;
    float sinGamma;
    float cosGamma;

    if (nDims != 2) {
        fprintf(stderr, "Number of dimensions is wrong!\n");
        return 0;   //FIXME return 1 or -1 would be better
    }
    if (nParsFitted != 3) {
        fprintf(stderr, "Number of parameters is wrong!\n");
        return 0;   //FIXME return 1 or -1 would be better
    }

    sinAlpha = sin(points[0] * DEG2RAD);
    cosAlpha = cos(points[0] * DEG2RAD);
    sinGamma = sin(points[1] * DEG2RAD);
    cosGamma = cos(points[1] * DEG2RAD);
    afunc[0] = sinGamma;
    afunc[1] = sinAlpha * cosGamma;
    afunc[2] = cosAlpha * cosGamma;

    return 1;    // FIXME return 0 would be better

} //svd_vvp1func






int svd_vvp2func(float points[],int nDims,float afunc[],int nParsFitted) {

    // ************************************************************************************
    // This function contains the basis-functions and other relevant parameters of
    // the fit model. The function returns the basis functions of the
    // multi-parameter linear fit evaluated at vector X=points[0..nDims-1] in the array
    // afunc[0..nParsFitted-1].
    // This function is intended to be supplied to the fitting routine 'svdfit'.
    // In this case, a six-parameter linear Area-VVP fit is performed on a three
    // dimensional vector space 'X' in radar coordinates: azimuth (alpha),
    // elevation (gamma), and range (rho).
    // ************************************************************************************


    float sinAlpha;
    float cosAlpha;
    float sinGamma;
    float cosGamma;
    float rho;

    if (nDims != 3) {
        fprintf(stderr, "Number of dimensions is wrong!\n");
        return 0;   //FIXME return 1 or -1 would be better
    }
    if (nParsFitted != 6) {
        fprintf(stderr, "Number of parameters is wrong!\n");
        return 0;   //FIXME return 1 or -1 would be better
    }

    sinAlpha = sin(points[0] * DEG2RAD);
    cosAlpha = cos(points[0] * DEG2RAD);
    sinGamma = sin(points[1] * DEG2RAD);
    cosGamma = cos(points[1] * DEG2RAD);
    rho = points[2] * cosGamma;
    afunc[0] = sinGamma;
    afunc[1] = sinAlpha * cosGamma;
    afunc[2] = cosAlpha * cosGamma;
    afunc[3] = 0.5 * rho * cosGamma;
    afunc[4] = 0.5 * rho * (2.0 * sinAlpha * cosAlpha) * cosGamma;
    afunc[5] = 0.5 * rho * (SQUARE(cosAlpha) - SQUARE(sinAlpha)) * cosGamma;

    return 1;    // FIXME return 0 would be better

} //svd_vvp2func







int svd_vvp3func(float points[],int nDims,float afunc[],int nParsFitted) {

    // ************************************************************************************
    // This function contains the basis-functions and other relevant parameters of
    // the fit model. The function returns the basis functions of the
    // multi-parameter linear fit evaluated at vector X=points[0..nDims-1] in the array
    // afunc[0..nParsFitted-1].
    // This function is intended to be supplied to the fitting routine 'svdfit'.
    // In this case, a nine-parameter linear Area-VVP fit is performed on a four
    // dimensional vector space 'X' in radar coordinates: azimuth (alpha),
    // elevation (gamma), range (rho), and height above radar (Znull).
    // ************************************************************************************

    float sinAlpha;
    float cosAlpha;
    float sinGamma;
    float cosGamma;
    float rho;
    float dheig;

    if (nDims != 4) {
        fprintf(stderr, "Number of dimensions is wrong!\n");
        return 0;   //FIXME return 1 or -1 would be better
    }
    if (nParsFitted != 9) {
        fprintf(stderr, "Number of parameters is wrong!\n");
        return 0;   //FIXME return 1 or -1 would be better
    }

    sinAlpha = sin(points[0] * DEG2RAD);
    cosAlpha = cos(points[0] * DEG2RAD);
    sinGamma = sin(points[1] * DEG2RAD);
    cosGamma = cos(points[1] * DEG2RAD);
    rho = points[2] * cosGamma;
    dheig = points[2] * sinGamma - points[3];
    afunc[0] = sinGamma;
    afunc[1] = sinAlpha * cosGamma;
    afunc[2] = cosAlpha * cosGamma;
    afunc[3] = 0.5 * rho * cosGamma;
    afunc[4] = 0.5 * rho * (2 * sinAlpha * cosAlpha) * cosGamma;
    afunc[5] = 0.5 * rho * (SQUARE(cosAlpha) - SQUARE(sinAlpha)) * cosGamma;
    afunc[6] = sinAlpha * cosGamma * dheig;
    afunc[7] = cosAlpha * cosGamma * dheig;
    afunc[8] = sinGamma * dheig;

    return 1;    // FIXME return 0 would be better

} //svd_vvp3func




int svdcmp(float *a,int m,int n,float w[],float *v) {

    // ************************************************************************************
    // Given a matrix a[0..m*n-1], this function computes its singular value
    // decomposition, A=U.W.V^T. The matrix U replaces A on output. The diagonal
    // matrix of singular values W is output as a vector w[0..n-1]. The matrix V
    // (not the transpose V^T) is output as v[0..n*n-1]. This function has
    // been modified from the original one in Numerical Recipes (2nd ed., paragraph
    // 2.6). Indices in the matrices now run from 0 to n-1/m-1, and allocation of
    // arrays is simplified. In addition, the calculation of 'pythagoras' has been
    // incorporated explicitly. See Numerical Recipes for more details.
    // ************************************************************************************


    int flag;
    int i;
    int iIteration;
    int nIterationsMax;
    int j;
    int jj;
    int k;
    int l;
    int nm;
    float *rv1;
    float anorm;
    float c;
    float f;
    float g;
    float h;
    float s;
    float scale;
    float x;
    float y;
    float z;

    nIterationsMax = 30;

    l = 0;
    nm = 0;

    /*Allocation of memory.*/

    rv1 = (float *)malloc(n*sizeof(float));
    if (rv1 == NULL) {
        fprintf(stderr, "Requested memory could not be allocated!\n");
        return 0;  //FIXME return 1 or -1 would be better
    }

    /*Start of very stable algorithm by Forsythe et al.*/
    /*Householder reduction to bidiagonal form.*/

    g = 0.0;
    scale = 0.0;
    anorm = 0.0;
    for (i = 0; i < n; i++) {
        l = i + 1;
        rv1[i] = scale * g;
        g = 0.0;
        s = 0.0;
        scale = 0.0;
        if (i < m) {
            for (k = i; k < m; k++) {
                scale += fabs(a[i+n*k]);
            }
            if (scale != 0) {
                for (k = i; k < m; k++) {
                    a[i+n*k] /= scale;
                    s += a[i+n*k] * a[i+n*k];
                }
                f = a[i+n*i];
                g = -sqrt(s) * SIGN(f);
                h = f * g - s;
                a[i+n*i] = f - g;

                for (j = l; j < n; j++) {
                    s = 0.0;
                    for (k = i; k < m; k++) {
                        s += a[i+n*k] * a[j+n*k];
                    }
                    f = s / h;
                    for (k = i; k < m; k++) {
                        a[j+n*k] += f * a[i+n*k];
                    }
                }
                for (k = i; k < m; k++) {
                    a[i+n*k] *= scale;
                }
            }
        }
        w[i] = scale * g;
        g = 0.0;
        s = 0.0;
        scale = 0.0;
        if (i < m && i != (n-1)) {
            for (k = l; k < n; k++) {
                scale += fabs(a[k+n*i]);
            }
            if (scale != 0) {
                for (k = l; k < n; k++) {
                    a[k+n*i] /= scale;
                    s += a[k+n*i] * a[k+n*i];
                }

                f = a[l+n*i];
                g = -sqrt(s) * SIGN(f);
                h = f * g - s;
                a[l+n*i] = f - g;
                for (k = l; k < n; k++) {
                    rv1[k] = a[k+n*i] / h;
                }

                for (j = l; j < m; j++) {
                    s = 0.0;
                    for (k = l; k < n; k++) {
                        s += a[k+n*j] * a[k+n*i];
                    }

                    for (k = l; k < n; k++) {
                        a[k+n*j] += s * rv1[k];
                    }
                }

                for (k = l; k < n; k++) {
                    a[k+n*i] *= scale;
                }
            }
        }
        anorm = XYMAX(anorm,(fabs(w[i]) + fabs(rv1[i])));
    }

    /*Accumulation of right-hand transformations.*/

    for (i = n-1; i >= 0; i--) {
        if (i < n-1) {
            if (g != 0) {
                for (j = l; j < n; j++) {
                    v[i+n*j] = (a[j+n*i] / a[l+n*i]) / g;
                }
                for (j = l; j < n; j++) {
                    s = 0.0;
                    for (k = l; k < n; k++) {
                        s += a[k+n*i] * v[j+n*k];
                    }

                    for (k = l; k < n; k++) {
                        v[j+n*k] += s * v[i+n*k];
                    }
                }
            }
            for (j = l; j < n; j++) {
                v[j+n*i] = v[i+n*j] = 0.0;
            }
        }
        v[i+n*i] = 1.0;
        g = rv1[i];
        l = i;
    }

    /*Accumulation of left-hand transformations.*/

    for (i = XYMIN(m,n) - 1; i >= 0; i--) {
        l = i + 1;
        g = w[i];
        for (j = l; j < n; j++) {
            a[j+n*i] = 0.0;
        }
        if (g != 0) {
            g = 1.0 / g;
            for (j = l; j < n; j++) {
                s = 0.0;
                for (k = l; k < m; k++) {
                    s += a[i+n*k] * a[j+n*k];
                }
                f = (s / a[i+n*i]) * g;
                for (k = i; k < m; k++) {
                    a[j+n*k] += f * a[i+n*k];
                }
            }
            for (j = i; j < m; j++) {
                a[i+n*j] *= g;
            }
        }
        else {
            for (j = i; j < m; j++) {
                a[i+n*j] = 0.0;
            }
        }
        ++a[i+n*i];
    }

    /*Diagonalization of the bidiagonal form: Loop over singular values, and */
    /*over allowed iterations.*/

    for (k = n-1; k >= 0; k--) {
        for (iIteration = 1; iIteration <= nIterationsMax; iIteration++) {
            flag = 1;
            for (l = k; l >= 0; l--) {
                nm = l - 1;
                if ((float)(fabs(rv1[l]) + anorm) == anorm) {
                    flag = 0;
                    break;
                }
                if ((float)(fabs(w[nm]) + anorm) == anorm) {
                    break;
                }
            }
            if (flag != 0) {
                c = 0.0;
                s = 1.0;
                for (i = l; i <= k; i++) {
                    f = s * rv1[i];
                    rv1[i] = c * rv1[i];
                    if ((float)(fabs(f) + anorm) == anorm) {
                        break;
                    }
                    g = w[i];
                    if (fabs(f) > fabs(g)) {
                        h = fabs(f) * sqrt(1 + SQUARE(g / f));
                    }
                    else {
                        h = (fabs(g) == 0 ? 0.0 : fabs(g) * sqrt(1 + SQUARE(f / g)));
                    }
                    w[i] = h;
                    h = 1.0 / h;
                    c = g * h;
                    s = -f * h;
                    for (j = 0; j < m; j++) {
                        y = a[nm+n*j];
                        z = a[i+n*j];
                        a[nm+n*j] = y * c + z * s;
                        a[i+n*j] = z * c - y * s;
                    }
                }
            }

            /*Convergence. Singular value is made nonnegative.*/

            z = w[k];
            if (l == k) {
                if (z < 0.0) {
                    w[k] = -z;
                    for (j = 0; j < n; j++) {
                        v[k+n*j] = -v[k+n*j];
                    }
                }
                break;
            }
            if (iIteration == nIterationsMax) {
                fprintf(stderr, "No convergence in %d svdcmp iterations!\n",nIterationsMax);
                return 0;    //FIXME return 1 or -1 would be better
            }
            x = w[l];
            nm = k - 1;
            y = w[nm];
            g = rv1[nm];
            h = rv1[k];
            f = ((y - z) * (y + z) + (g - h) * (g + h)) / (2.0 * h * y);
            if (fabs(f) > 1.0) {
                g = fabs(f) * sqrt(1 + SQUARE(1 / f));
            }
            else {
                g = sqrt(1 + SQUARE(f));
            }
            f = ((x - z) * (x + z) + h * ((y / (f + g * SIGN(f))) - h)) / x;

            /*Next QR transformation.*/

            c = 1.0;
            s = 1.0;
            for (j = l; j <= nm; j++) {
                i = j + 1;
                g = rv1[i];
                y = w[i];
                h = s * g;
                g = c * g;
                if (fabs(f) > fabs(h)) {
                    z = fabs(f) * sqrt(1 + SQUARE(h / f));
                }
                else {
                    z = (fabs(h) == 0 ? 0.0 : fabs(h) * sqrt(1 + SQUARE(f / h)));
                }
                rv1[j] = z;
                c = f / z;
                s = h / z;
                f = x * c + g * s;
                g = g * c - x * s;
                h = y * s;
                y *= c;
                for (jj = 0; jj < n; jj++) {
                    x = v[j+n*jj];
                    z = v[i+n*jj];
                    v[j+n*jj] = x * c + z * s;
                    v[i+n*jj] = z * c - x * s;
                }
                if (fabs(f) > fabs(h)) {
                    z = fabs(f) * sqrt(1 + SQUARE(h / f));
                }
                else {
                    z = (fabs(h) == 0 ? 0.0 : fabs(h) * sqrt(1 + SQUARE(f / h)));
                }

                /*Rotation can be arbitrary if z=0.*/

                w[j] = z;
                if (z != 0) {
                    z = 1.0 / z;
                    c = f * z;
                    s = h * z;
                }
                f = c * g + s * y;
                x = c * y - s * g;
                for (jj = 0; jj < m; jj++) {
                    y = a[j+n*jj];
                    z = a[i+n*jj];
                    a[j+n*jj] = y * c + z * s;
                    a[i+n*jj] = z * c - y * s;
                }
            }
            rv1[l] = 0.0;
            rv1[k] = f;
            w[k] = x;
        }
    }

    /*Cleaning of memory.*/

    free(rv1);

    return 1;    //FIXME return 0 would be better

} //svdcmp





float svdfit(float *points,int nDims,float yObs[],float yFitted[],int nPoints,
        int (*funcs)(float points[],int nDims,float afunc[],int nParsFitted),
        float parameterVector[],float avar[],int nParsFitted) {


    // ************************************************************************************************
    // This function performs a multi-dimensional linear fit using singular value
    // decomposition. See Numerical Recipes (2nd ed., paragraph 15.4) for details.
    // Given a set of data points points[0..nPoints*nDims-1], yObs[0..nPoints-1], use Chi-square
    // minimization to determine the coefficients a[0..nParsFitted-1] of the fitting
    // function y=SUM_i[apar_i*afunc_i(points)]. The dimensionality of the input vector
    // 'points' is equal to 'nDims', generally this will be equal to 1.
    // Here we solve the fitting equations using singular value decomposition of
    // the nPoints by nParsFitted matrix. The program returns values for the array  with fit
    // parameters 'parameterVector', covariance matrix 'cvm', and Chi-square.
    // The user supplies a function with the fit model 'funcs(points,nDims,afunc,nParsFitted)'
    // that returns the 'nParsFitted' basis functions evaluated at points[0..nDims-1] in the
    // array afunc[0..nParsFitted-1].
    // ************************************************************************************************

    // FIXME There is no matrix called 'cvm' perhaps he means 'avar'?


    int iParFitted;
    int iParFittedRows;
    int iParFittedCols;
    int nParsFittedRows;
    int nParsFittedCols;
    int k;
    int iPoint;
    float afunc[NPARSFITTEDMAX];             // FIXME nParsFitted would make more sense
                                             // FIXME afunc is the design matrix?
                                             // FIXME order of afunc parameters is not u,v,w?
    float singularValues[NPARSFITTEDMAX];    // FIXME nParsFitted would make more sense
    float v[NPARSFITTEDMAX*NPARSFITTEDMAX];  // FIXME nParsFitted would make more sense
    float wti[NPARSFITTEDMAX];               // FIXME nParsFitted would make more sense
    float *u;
    float singularValueMax;
    float sum;
    float chisq;

    // Checking whether the input numbers are within bounds:
    if (nParsFitted > NPARSFITTEDMAX) {
        fprintf(stderr, "Number of fit parameters is too large!\n");
        return -1.0;
    }
    if (nPoints <= nParsFitted) {
        fprintf(stderr, "Number of data points is too small!\n");
        return -1.0;
    }

    // Allocation of memory for arrays.
    u = (float *)malloc(nPoints*nParsFitted*sizeof(float));
    if (!u) {
        fprintf(stderr, "Requested memory could not be allocated!\n");
        return -1.0;
    }

    // Filling of the design matrix of the fitting problem (u[iPoint][iParFitted]).
    for (iPoint = 0; iPoint < nPoints; iPoint++) {

        // note pointer arithmetic in this next statement:
        if (!(*funcs)(points+nDims*iPoint,nDims,afunc,nParsFitted)) {
            return -1.0;
        }
        for (iParFitted = 0; iParFitted < nParsFitted; iParFitted++) {
            u[iParFitted+nParsFitted*iPoint] = afunc[iParFitted];
        }
    }

    // Singular value decomposition of the design matrix of the fit.
    if (!svdcmp(u,nPoints,nParsFitted,singularValues,v)) {
        return -1.0;
    }

    // Removal of the singular values.
    singularValueMax = 0.0;
    for (iParFitted = 0; iParFitted < nParsFitted; iParFitted++) {
        if (singularValues[iParFitted] > singularValueMax) {
            singularValueMax = singularValues[iParFitted];
        }
    }
    for (iParFitted = 0; iParFitted < nParsFitted; iParFitted++) {
        if (singularValues[iParFitted] < SVDTOL*singularValueMax) {
            singularValues[iParFitted] = 0.0;
        }
    }

    // Calculation of fit parameters 'parameterVector' using backsubstitution with 'yObs'.
    if (!svbksb(u,singularValues,v,nPoints,nParsFitted,yObs,parameterVector)) {
        return -1.0;
    }

    // Calculation of variances of fit parameters 'parameterVector'.

    for (iParFitted = 0; iParFitted < nParsFitted; iParFitted++) {
        wti[iParFitted] = 0.0;
        if (singularValues[iParFitted] != 0) {
            wti[iParFitted] = 1.0/(singularValues[iParFitted]*singularValues[iParFitted]);
        }
    }
    for (iParFittedCols = 0; iParFittedCols < nParsFittedCols; iParFittedCols++) {
        avar[iParFittedCols] = 0.0;
        for (iParFittedRows = 0 ; iParFittedRows < nParsFitted; iParFittedRows++) {
            // FIXME double nParsFitted loop? seems unlikely
            k = iParFittedCols + nParsFittedCols*iParFittedRows;
            avar[iParFittedCols] += v[k] * v[k] * wti[iParFittedRows];
        }
    }

    /*Calculation of fitted y's and Chi-square of the fit.*/
    chisq = 0.0;
    for (iPoint = 0; iPoint < nPoints; iPoint++) {

        // note pointer arithmetic in this next statement:
        if (!(*funcs)(points+nDims*iPoint,nDims,afunc,nParsFitted)) {
            return -1.0;
        }
        sum = 0.0;
        for (iParFitted = 0; iParFitted < nParsFitted; iParFitted++) {
            sum += parameterVector[iParFitted] * afunc[iParFitted];
        }
        yFitted[iPoint] = sum;
        chisq += SQUARE(yObs[iPoint]-yFitted[iPoint]);
    }
    chisq /= nPoints-nParsFitted;

    /*Cleaning of memory.*/

    free(u);

    return chisq;
} //svdfit




