/*
 * findcells.c
 *
 *  Created on: Apr 2, 2014
 *      Author: wbouten
 */

#include <jni.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <math.h>
#include "libvol2bird.h"
//#include "nl_esciencecenter_ncradar_RadarScan.h" // maybe only used when calling java from c?


/******************************************************************************/
/*This function detects the cells in 'image' using an integer threshold value */
/*of 'thres' and a non-recursive algorithm which looks for neighboring pixels */
/*above threshold. On return the marked cells are contained by 'cellmap'. The */
/*number of detected cells/highest index value is returned.                   */
/******************************************************************************/

int findcells(unsigned char *teximage,unsigned char *rhoimage,
        unsigned char *zdrimage,int *cellmap,
        SCANMETA *texmeta, SCANMETA *rhometa, SCANMETA *zdrmeta,
        float threstexmin,float thresrhomin,float threszdrmin,
        float dbzmin,float rcellmax,char sign)
{
    int ncell;
    int ij;
    int i;
    int j;
    int ii;
    int jj;
    int k;
    int count;
    int nrang;
    int nAzim;
    int missing;
    int threstex;
    int thresrho;
    int threszdr;
    int nrang;
    int nazim;
    int threstex;

    ncell = 0;
    thresrho = 0;
    threszdr = 0;
    nrang = texmeta->nrang;
    nazim = texmeta->nazim; // FIXME previously did not exist nAzim/nazim
    missing = texmeta->missing;
    threstex = ROUND((threstexmin-(texmeta->zoffset))/(texmeta->zscale));

    if (rhoimage!=NULL) {
        thresrho = ROUND((thresrhomin-(rhometa->zoffset))/(rhometa->zscale));
    }
    if (zdrimage!=NULL) {
        threszdr = ROUND((threszdrmin-(zdrmeta->zoffset))/(zdrmeta->zscale));
    }

    /*Initializing of connection cellmap.*/

    for (ij = 0; ij<nazim*nrang; ij++) {
        cellmap[ij] = -1;
    }

    /*If threshold is missing, return.*/

    if (threstex==missing) {
        return 0;
    }

    /*Labeling of groups of connected pixels using horizontal, vertical, and */
    /*diagonal connections. The algorithm is described in 'Digital Image */
    /*Processing' by Gonzales and Woods published by Addison-Wesley.*/

    for (j = 0; j<nazim; j++) {
        for (i = 0; i<nrang; i++) {
            if ((i+1)*(texmeta->rscale)>rcellmax) {
                continue;
            }
            if (rhoimage==NULL){
                if (teximage[i+j*nrang]==missing||sign*teximage[i+j*nrang]>sign*threstex) {
                    continue;
                }
                /* count number of direct neighbors above threshold */
                count = 0;
                for (k = 0; k<9; k++) {
                    ii = (i-1+k%3);
                    jj = (nazim+(j-1+k/3))%nazim; /* periodic boundary condition azimuth */
                    if (ii+jj*nrang >= nrang*nazim || ii+jj*nrang < 0) {
                        continue;
                    }
                    if (sign*teximage[ii+jj*nrang]<=sign*threstex) {
                        count++;
                    }
                }
                /* when not enough neighbors with dBZ>DBZRAIN, continue */
                if (count-1 < NEIGHBOURS) {
                    continue;
                }
            }
            else {
                if (!(rhoimage[i+j*nrang]!=missing && zdrimage[i+j*nrang]!=missing &&
                        teximage[i+j*nrang]!=missing && teximage[i+j*nrang]>=dbzmin &&
                        (zdrimage[i+j*nrang]>threszdr||rhoimage[i+j*nrang]>thresrho))) {
                    continue;
                }
                /* count number of direct neighbors above threshold */
                count = 0;
                for (k = 0; k<9; k++) {
                    ii = (i-1+k%3);
                    jj = (nazim+(j-1+k/3))%nazim; /* periodic boundary condition azimuth */
                    if (rhoimage[ii+jj*nrang]>thresrho||zdrimage[ii+jj*nrang]>threszdr) count++;
                }
                /* when not enough neighbors with dBZ>DBZRAIN, continue */
                if (count-1 < NEIGHBOURS) {
                    continue;
                }
            }
            /*Looking for horizontal, vertical, forward diagonal, and backward diagonal */
            /*connections.*/

            for (k=0 ; k<4 ; k++) {

                ii = (i-1+k%3);
                jj = (nazim+(j-1+k/3))%nazim; /* periodic boundary condition azimuth */
                /* index out of range, goto next pixel: */
                if (ii<0||ii>=nrang||jj<0||jj>=nazim) {
                    continue;
                }
                /* no connection found, goto next pixel */
                if (cellmap[ii+jj*nrang]<0) {
                    continue;
                }
                /* if pixel still unassigned, assign same cellnumber as connection */
                if (cellmap[i+j*nrang]<0) {
                    cellmap[i+j*nrang] = cellmap[ii+jj*nrang];
                }
                else {
                    /* if connection found but pixel is already assigned a different cellnumber: */
                    if (cellmap[i+j*nrang]!=cellmap[ii+jj*nrang]) {
                        /* merging cells detected: replace all other occurences by value of connection: */
                        for (ij=0 ; ij<nazim*nrang ; ij++) {
                            if (cellmap[ij]==cellmap[i+j*nrang]) {
                                cellmap[ij] = cellmap[ii+jj*nrang];
                            }
                            /*note: not all ncell need to be used eventually */
                        }
                    }
                }
            }

            /*When no connections are found, give a new number.*/

            if (cellmap[i+j*nrang]<0) {
                cellmap[i+j*nrang] = ncell;
                ncell++;
            }
        }
    }

    /*Returning number of detected cells (including fringe/clutter) .*/

    return ncell;
}//findcells
