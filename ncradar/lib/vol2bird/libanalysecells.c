/******************************************************************************/
/*This function analyses the cellmap found by the 'findcells' procedure.      */
/*Smalls cells are rejected and the cells are re-numbered according to size   */
/*The final number of cells in the cellmap is returned as an integer          */
/******************************************************************************/

int analysecells(unsigned char *imgz,unsigned char *imgv, unsigned char *imgtex,
        unsigned char *imgcm, int *cellmap, SCANMETA *zmeta,SCANMETA *vmeta,
        SCANMETA *texmeta, SCANMETA *cmmeta, int Ncell,int area, float dbzcell,
        float stdevcell, float clutcell, float vmin,float dbzclutter,
        unsigned char cmflag, unsigned char dualpolflag, unsigned char verbose)
{
    CELLPROP *c;
    int n,ij,ir,ia,NcellValid,nazim,nrang;
    float validarea;
    float zoffset,zscale;

    NcellValid=Ncell;
    zscale=zmeta->zscale;
    zoffset=zmeta->zoffset;
    nrang=zmeta->nrang;
    nazim=zmeta->nazim;

    /*Allocating and initializing memory for cell properties.*/
    c=(CELLPROP *)malloc(Ncell*sizeof(CELLPROP));
    if (!c) {
        printf("Requested memory could not be allocated!\n");
        exit(10);
    }
    for (n=0 ; n<Ncell ; n++) {
        c[n].imax=c[n].jmax=0;
        c[n].area=c[n].clutterarea=c[n].dbz=c[n].tex=0;
        c[n].max=zoffset;
        c[n].index=n;
        c[n].drop=0;
        c[n].cv=0;
    }

    /*Calculation of cell properties.*/
    for (ia=0 ; ia<nazim ; ia++) {
        for (ir=0 ; ir<zmeta->nrang ; ir++) {
            ij=ir+ia*zmeta->nrang;
            if (cellmap[ij]<0) continue;
            //low radial velocities are treated as clutter, not included in calculation cell properties
            if (fabs(vmeta->zscale*imgv[ij]+vmeta->zoffset)<vmin){
                c[cellmap[ij]].clutterarea+=1;
                c[cellmap[ij]].area+=1;
                continue;
            }
            //pixels in clutter map not included in calculation cell properties

            if (cmflag == 1){
                if (ia<cmmeta->nazim && ir<zmeta->nrang){
                    if (cmmeta->zscale*imgcm[ij]+cmmeta->zoffset>dbzclutter){
                        c[cellmap[ij]].clutterarea+=1;
                        c[cellmap[ij]].area+=1;
                        continue;
                    }
                }
            }
            c[cellmap[ij]].area+=1;

            if (zscale*imgz[ij]+zoffset>c[cellmap[ij]].max) {
                c[cellmap[ij]].max=zscale*imgz[ij]+zoffset;
                c[cellmap[ij]].imax=ij%nazim;
                c[cellmap[ij]].jmax=ij/nazim;
            }
            c[cellmap[ij]].dbz+=zscale*imgz[ij]+zoffset;
            c[cellmap[ij]].tex+=(texmeta->zoffset)+(texmeta->zscale)*imgtex[ij];
        }
    }
    for (n=0 ; n<Ncell ; n++) {
        validarea=c[n].area-c[n].clutterarea;
        if (validarea>0){
            c[n].dbz/=validarea;
            c[n].tex/=validarea;
            c[n].cv=10*log10(c[n].tex)-c[n].dbz;
            c[n].cv=c[n].tex/c[n].dbz;
        }
    }

    /*Determine which cells to drop from map based on low mean dBZ / high stdev / small area / high percentage clutter*/
    for (n=0 ; n<Ncell ; n++) {
        if (dualpolflag == 1){
            if (c[n].area<area) c[n].drop=1;
        }
        else {
            if (c[n].area<area || (c[n].dbz<dbzcell && c[n].tex>stdevcell && c[n].clutterarea/c[n].area < clutcell )){
                c[n].drop=1;
            }
        }
    }

    /*Sorting cell properties according to cell area. Drop small cells from map*/
    NcellValid=updatemap(cellmap,c,Ncell,nazim*nrang,area);

    //Printing of cell properties to stdout.
    if (verbose==1){
        printf("#Cell analysis for elevation %f:\n",zmeta->elev);
        printf("#Minimum cell area in pixels   : %i\n",area);
        printf("#Threshold for mean dBZ cell   : %g dBZ\n",dbzcell);
        printf("#Threshold for mean stdev cel  : %g dBZ\n",stdevcell);
        printf("#Valid cells                   : %i/%i\n#\n",NcellValid,Ncell);
        printf("cellinfo: NUM CellArea ClutArea AvgDbz AvgStdev CV MaxVal MaxIII MaxJJJ Dropped\n");
        for (n=0 ; n<Ncell ; n++) {
            if (c[n].area==0) continue;
            printf("cellinfo: %3d %8.1f %8.1f %6.2f %6.2f %6.2f %6.2f %6d %6d %6d\n",
                    n+2,c[n].area,c[n].clutterarea,c[n].dbz,c[n].tex,c[n].cv,c[n].max,c[n].imax,c[n].jmax,c[n].drop);
        }
    }

    free(c);

    return NcellValid;
} //analysecells
