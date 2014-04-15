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
              float texThresMin,float rhoThresMin,float zdrThresMin,
              float dbzmin,float rcellmax,char sign)
{
    int ncell;
    int ij;
    int iRang;
    int nRang;
    int iAzim;
    int nAzim;
    int iRangLocal;
    int iAzimLocal;
    int iNeighbourhood;
    int count;

    int texMissing;
    int texThres;
    double texOffset;
    double texScale;

    int rhoMissing;
    int rhoThres;
    double rhoOffset;
    double rhoScale;

    int zdrMissing;
    int zdrThres;
    double zdrOffset;
    double zdrScale;

    int iGlobal;
    int nGlobal;
    int iLocal;

    ncell = 0;

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

    rhoThres = 0;
    zdrThres = 0;
    texThres = ROUND((texThresMin-texOffset)/texScale);  // FIXME why type is int?

    iGlobal = iRang+iAzim*nRang;
    nGlobal = nAzim*nRang;


    if (rhoimage!=NULL) {
        rhoThres = ROUND((rhoThresMin-rhoOffset)/rhoScale);
    }
    if (zdrimage!=NULL) {
        zdrThres = ROUND((zdrThresMin-zdrOffset)/zdrScale);
    }

    /*Initializing of connection cellmap.*/
    for (iGlobal = 0; iGlobal<nGlobal; iGlobal++) {
        cellmap[iGlobal] = -1;
    }

    /*If threshold is missing, return.*/
    if (texThres==texMissing) {
        return 0;  // FIXME return zero is traditionally used to signal "everything is great!"
    }

    /*Labeling of groups of connected pixels using horizontal, vertical, and */
    /*diagonal connections. The algorithm is described in 'Digital Image */
    /*Processing' by Gonzales and Woods published by Addison-Wesley.*/

    for (iAzim=0; iAzim<nAzim; iAzim++) {
        for (iRang=0; iRang<nRang; iRang++) {

            if ((iRang+1)*(texmeta->rscale)>rcellmax) {  // FIXME what is the difference between zscale and rscale // FIXME why iRang +1 ?
                continue;
            }

            /* count number of direct neighbors above threshold */
            count = 0;

            if (rhoimage==NULL){
                if (teximage[iGlobal]==texMissing || sign*teximage[iGlobal]>sign*texThres) {  // FIXME why sign x2? ... sort of an ABS?
                    continue;
                }
                for (iNeighborhood=0; iNeighborhood<9; iNeighborhood++) {
                    iRangLocal = (iRang-1+iNeighborhood%3);
                    iAzimLocal = (nAzim+(iAzim-1+iNeighborhood/3))%nAzim; /* periodic boundary condition azimuth */
                    iLocal = iRangLocal+iAzimLocal*nRang;
                    if (iLocal >= nGlobal || iLocal < 0) {
                        continue;
                    }
                    if (sign*teximage[iLocal]<=sign*texThres) { // FIXME sign 2x ?
                        count++;
                    }
                }
            }
            else {
                if (!(rhoimage[iGlobal]!=rhoMissing && zdrimage[iGlobal]!=zdrMissing &&
                        teximage[iGlobal]!=texMissing && teximage[iGlobal]>=dbzmin &&
                        (zdrimage[iGlobal]>zdrThres||rhoimage[iGlobal]>rhoThres))) {
                    continue;
                }
                for (iNeighborhood=0; iNeighborhood<9; iNeighborhood++) {
                    iRangLocal = (iRang-1+iNeighborhood%3);
                    iAzimLocal = (nAzim+(iAzim-1+iNeighborhood/3))%nAzim; /* periodic boundary condition azimuth */
                    iLocal = iRangLocal+iAzimLocal*nRang;
                    if (rhoimage[iLocal]>rhoThres||zdrimage[iLocal]>zdrThres) {
                        count++;
                    }
                }
            } // if (rhoimage==NULL)

            /* when not enough neighbors with dBZ>DBZRAIN, continue */
            if (count-1 < NEIGHBOURS) {
                continue;
            }




            /*Looking for horizontal, vertical, forward diagonal, and backward diagonal */
            /*connections.*/

            for (k=0; k<4; k++) {

                iRangLocal = (iRang-1+k%3); // FIXME is this right given that k counts to only 4, not 9?
                iAzimLocal = (nAzim+(iAzim-1+k/3))%nAzim; /* periodic boundary condition azimuth */
                iLocal = iRangLocal+iAzimLocal*nRang;

                /* index out of range, goto next pixel: */
                if (iRangLocal<0 || iRangLocal>=nRang || iAzimLocal<0 || iAzimLocal>=nAzim) {
                    continue;
                }

                /* no connection found, goto next pixel */
                if (cellmap[iLocal]<0) {
                    continue;
                }

                /* if pixel still unassigned, assign same cellnumber as connection */
                if (cellmap[iGlobal]<0) {
                    cellmap[iGlobal] = cellmap[iLocal];
                }
                else {
                    /* if connection found but pixel is already assigned a different cellnumber: */
                    if (cellmap[iGlobal]!=cellmap[iLocal]) {
                        /* merging cells detected: replace all other occurences by value of connection: */
                        for (ij=0 ; ij<nGlobal ; ij++) {
                            if (cellmap[ij]==cellmap[iGlobal]) {
                                cellmap[ij] = cellmap[iLocal];
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

        } // (iRang=0; iRang<nRang; iRang++)
    } // for (iAzim=0; iAzim<nAzim; iAzim++)

    /*Returning number of detected cells (including fringe/clutter) .*/

    return ncell;
}//findcells
