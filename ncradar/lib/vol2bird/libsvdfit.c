
//    ****************************************************************************
//    Functions for linear fitting using Singular Value Decomposition:
//    ****************************************************************************
//
//    ****************************************************************************
//    This function performs a multi-dimensional linear fit using singular value
//    decomposition. See Numerical Recipes (2nd ed., paragraph 15.4) for details.
//    Given a set of data points points[0..nPoints*nDims-1], yObs[0..nPoints-1], use Chi-square
//    minimization to determine the coefficients a[0..nParsFitted-1] of the fitting
//    function y=SUM_i[apar_i*afunc_i(points)]. The dimensionality of the input vector
//    'points' is equal to 'nDims', generally this will be equal to 1.
//    Here we solve the fitting equations using singular value decomposition of
//    the nPoints by nParsFitted matrix. The program returns values for the array  with fit
//    parameters 'parameterVector', covariance matrix 'cvm', and Chi-square.
//    The user supplies a function with the fit model 'funcs(points,nDims,afunc,nParsFitted)'
//    that returns the 'nParsFitted' basis functions evaluated at points[0..nDims-1] in the
//    array afunc[0..nParsFitted-1].
//    ****************************************************************************

// FIXME There is no matrix called 'cvm' perhaps he means 'avar'?

float svdfit(float *points,int nDims,float yObs[],float yFitted[],int nPoints,
        int (*funcs)(float points[],int nDims,float afunc[],int nParsFitted),
        float parameterVector[],float avar[],int nParsFitted)
{

    int iParFitted;
    int iParFittedRows;
    int iParFittedCols;
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
        if (singularValues[iParFitted]) {   // FIXME if this is a test for not equal to zero, it's better to say (bla != 0)
            wti[iParFitted] = 1.0/(singularValues[iParFitted]*singularValues[iParFitted]);
        }
    }
    for (iParFittedCols = 0; iParFittedCols < nParsFittedCols; iParFittedCols++) {
        avar[iParFittedCols] = 0.0;
        for (iParFittedRows = 0 ; iParFittedRows < nParsFitted; iParFittedRows++) {
            // FIXME double nParsFitted loop? seems unlikely
            k = iParFittedCols + nParsFittedCols*iParFittedRows;
            avar[iParFittedCols] += v[k] * v[k] * wti[iParsFittedRows];
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





