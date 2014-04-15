/*
 * libfindcells.c
 *
 *  Created on: Apr 2, 2014
 *      Author: Jurriaan H. Spaaks, Netherlands eScience Center
 */

#include <jni.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <math.h>
#include "libvol2bird.h"
//#include "nl_esciencecenter_ncradar_JNIMethodsVol2Bird.h" // maybe only used when calling java from c?


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
    int iRang;
    int iAzim;
    int ii;
    int jj;
    int k;
    int count;
    int nRang;
    int nAzim;
    int texMissing;
    double texOffset;
    double texScale;

    int rhoMissing;
    double rhoOffset;
    double rhoScale;

    int zdrMissing;
    double zdrOffset;
    double zdrScale;

    int threstex;
    int thresrho;
    int threszdr;
    int iGlobal;

    ncell = 0;
    thresrho = 0;
    threszdr = 0;
    nRang = texmeta->nrang;
    nAzim = texmeta->nazim;

    texMissing = texmeta->missing;
    texOffset = texmeta->zoffset;
    texScale = texmeta->zscale;

    rhoMissing = rhometa->missing;
    rhoOffset = rhometa->zoffset;
    rhoScale = rhometa->zscale;

    zdrMissing = zdrmeta->missing;
    zdrOffset = zdrmeta->zoffset;
    zdrScale = zdrmeta->zscale;

    iGlobal = iRang+iAzim*nRang;

    threstex = ROUND((threstexmin-texOffset)/texScale);  // FIXME why type is int?

    if (rhoimage!=NULL) {
        thresrho = ROUND((thresrhomin-rhoOffset)/rhoScale);
    }
    if (zdrimage!=NULL) {
        threszdr = ROUND((threszdrmin-zdrOffset)/zdrScale);
    }

    /*Initializing of connection cellmap.*/
    for (ij = 0; ij<nAzim*nRang; ij++) {
        cellmap[ij] = -1;
    }

    /*If threshold is missing, return.*/
    if (threstex==texMissing) {
        return 0;  // FIXME return zero is traditionally used to signal "everything is great!"
    }

    /*Labeling of groups of connected pixels using horizontal, vertical, and */
    /*diagonal connections. The algorithm is described in 'Digital Image */
    /*Processing' by Gonzales and Woods published by Addison-Wesley.*/

    for (iAzim=0; iAzim<nAzim; iAzim++) {
        for (iRang = 0; iRang<nRang; iRang++) {

            if ((iRang+1)*(texmeta->rscale)>rcellmax) {
                continue;
            }

            if (rhoimage==NULL){
                if (teximage[iGlobal]==texMissing||sign*teximage[iGlobal]>sign*threstex) {
                    continue;
                }
                /* count number of direct neighbors above threshold */
                count = 0;
                for (k = 0; k<9; k++) {
                    ii = (iRang-1+k%3);
                    jj = (nAzim+(iAzim-1+k/3))%nAzim; /* periodic boundary condition azimuth */
                    if (ii+jj*nRang >= nRang*nAzim || ii+jj*nRang < 0) {
                        continue;
                    }
                    if (sign*teximage[ii+jj*nRang]<=sign*threstex) {
                        count++;
                    }
                }
                /* when not enough neighbors with dBZ>DBZRAIN, continue */
                if (count-1 < NEIGHBOURS) {
                    continue;
                }
            }
            else {
                if (!(rhoimage[iGlobal]!=rhoMissing && zdrimage[iGlobal]!=zdrMissing &&
                        teximage[iGlobal]!=texMissing && teximage[iGlobal]>=dbzmin &&
                        (zdrimage[iGlobal]>threszdr||rhoimage[iGlobal]>thresrho))) {
                    continue;
                }
                /* count number of direct neighbors above threshold */
                count = 0;
                for (k = 0; k<9; k++) {
                    ii = (iRang-1+k%3);
                    jj = (nAzim+(iAzim-1+k/3))%nAzim; /* periodic boundary condition azimuth */
                    if (rhoimage[ii+jj*nRang]>thresrho||zdrimage[ii+jj*nRang]>threszdr) count++;
                }
                /* when not enough neighbors with dBZ>DBZRAIN, continue */
                if (count-1 < NEIGHBOURS) {
                    continue;
                }
            }
            /*Looking for horizontal, vertical, forward diagonal, and backward diagonal */
            /*connections.*/

            for (k=0; k<4; k++) {

                ii = (iRang-1+k%3);
                jj = (nAzim+(iAzim-1+k/3))%nAzim; /* periodic boundary condition azimuth */

                /* index out of range, goto next pixel: */
                if (ii<0||ii>=nRang||jj<0||jj>=nAzim) {
                    continue;
                }

                /* no connection found, goto next pixel */
                if (cellmap[ii+jj*nRang]<0) {
                    continue;
                }

                /* if pixel still unassigned, assign same cellnumber as connection */
                if (cellmap[iGlobal]<0) {
                    cellmap[iGlobal] = cellmap[ii+jj*nRang];
                }
                else {
                    /* if connection found but pixel is already assigned a different cellnumber: */
                    if (cellmap[iGlobal]!=cellmap[ii+jj*nRang]) {
                        /* merging cells detected: replace all other occurences by value of connection: */
                        for (ij=0 ; ij<nAzim*nRang ; ij++) {
                            if (cellmap[ij]==cellmap[iGlobal]) {
                                cellmap[ij] = cellmap[ii+jj*nRang];
                            }
                            /*note: not all ncell need to be used eventually */
                        }
                    }
                }
            }

            /*When no connections are found, give a new number.*/
            if (cellmap[iGlobal]<0) {
                cellmap[iGlobal] = ncell;
                ncell++;
            }
        }
    }

    /*Returning number of detected cells (including fringe/clutter) .*/

    return ncell;
}//findcells
