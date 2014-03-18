
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


/******************************************************************************/
/*This function computes a texture parameter based on a block of (ntexrang x  */
/* ntexazim) pixels. The texture parameter equals the local standard deviation*/
/*in the velocity field.                                                      */
/******************************************************************************/

void texture(unsigned char *teximg,unsigned char *vimage, unsigned char *zimage,
		SCANMETA *texmeta,SCANMETA *vmeta,SCANMETA *zmeta,
		unsigned char ntexrang,unsigned char ntexazim,
		unsigned char ntexmin,unsigned char textype)
{
  int i,j,ii,jj,k,count,nrang,nazim,missing;
  int value,index;
  double vmoment1,vmoment2,dbz,tex=texmeta->zoffset;
  nrang=vmeta->nrang;
  nazim=vmeta->nazim;
  missing=vmeta->missing;

   for (j=0 ; j<nazim ; j++) {
      for (i=0 ; i<nrang ; i++) {
	 /* count number of direct neighbours above threshold */
	 count=0; vmoment1=0; vmoment2=0; dbz=(zmeta->zoffset)+(zmeta->zscale)*(zimage[i+j*nrang]);
	 for (k=0 ; k<ntexrang*ntexazim ; k++) {
            ii=(i-ntexrang/2+k%ntexrang);
	    jj=(nazim+(j-ntexazim/2+k/ntexrang))%nazim; /* periodic boundary condition azimuth */
   //         ii=(i-1+k%3);
   //         jj=(nazim+(j-1+k/3))%nazim; /* periodic boundary condition azimuth */
            index=ii+jj*nrang;
	    if (index >= nrang*nazim || index < 0) continue;
            if (vimage[ii+jj*nrang]==missing || zimage[ii+jj*nrang]==missing) continue;
   //         if (vimage[ii+jj*nrang]==missing) value=-(vmeta->zoffset)/(vmeta->zscale);
	    else value=vimage[ii+jj*nrang];
            vmoment1+=(vmeta->zoffset)+(vmeta->zscale)*(vimage[i+j*nrang]-value);
            vmoment2+=SQUARE((vmeta->zoffset)+(vmeta->zscale)*(vimage[i+j*nrang]-value));
            dbz+=(zmeta->zoffset)+(zmeta->zscale)*(zimage[ii+jj*nrang]);
	    count++;
	 }
	 vmoment1/=count;
	 vmoment2/=count;
	 dbz/=count;
	 /* when not enough neighbours, continue */
	 if (count < ntexmin) teximg[i+j*nrang]=missing;
	 else {
	    if (textype == TEXCV){
	       tex=10*log10(sqrt(XABS(vmoment2-SQUARE(vmoment1))))-dbz;     
	       if (tex < texmeta->zoffset+texmeta->zscale) tex=texmeta->zoffset+texmeta->zscale;
	    }
	    if (textype == TEXSTDEV){
	       tex=sqrt(XABS(vmoment2-SQUARE(vmoment1)));
	    }
	    teximg[i+j*nrang]=ROUND((tex-texmeta->zoffset)/(texmeta->zscale));
	 } //else
      } //for
  } //for 
}//texture

/******************************************************************************/
/*This function detects the cells in 'image' using an integer threshold value */
/*of 'thres' and a non-recursive algorithm which looks for neighboring pixels */
/*above threshold. On return the marked cells are contained by 'cellmap'. The */
/*number of detected cells/highest index value is returned.                   */
/******************************************************************************/

int findcells(unsigned char *teximage,unsigned char *rhoimage, 
              unsigned char *zdrimage,int *cellmap,
	      SCANMETA *meta, SCANMETA *rhometa, SCANMETA *zdrmeta, 
	      float threstexmin,float thresrhomin,float threszdrmin,
	      float dbzmin,float rcellmax,char sign)
{
int ncell=0,ij,i,j,ii,jj,k,count,nrang,nazim,missing,threstex,thresrho=0,threszdr=0;
nrang=meta->nrang;
nazim=meta->nazim;
missing=meta->missing;
threstex=ROUND((threstexmin-(meta->zoffset))/(meta->zscale));

if (rhoimage!=NULL) thresrho=ROUND((thresrhomin-(rhometa->zoffset))/(rhometa->zscale));
if (zdrimage!=NULL) threszdr=ROUND((threszdrmin-(zdrmeta->zoffset))/(zdrmeta->zscale));

/*Initializing of connection cellmap.*/

for (ij=0 ; ij<nazim*nrang ; ij++) cellmap[ij]=-1;

/*If threshold is missing, return.*/

if (threstex==missing) return 0;

/*Labeling of groups of connected pixels using horizontal, vertical, and */
/*diagonal connections. The algorithm is described in 'Digital Image */
/*Processing' by Gonzales and Woods published by Addison-Wesley.*/

for (j=0 ; j<nazim ; j++) {
   for (i=0 ; i<nrang ; i++) {
      if ((i+1)*(meta->rscale)>rcellmax) continue;
      if (rhoimage==NULL){
         if (teximage[i+j*nrang]==missing||sign*teximage[i+j*nrang]>sign*threstex) continue; 
         /* count number of direct neighbours above threshold */
         count=0;
         for (k=0 ; k<9 ; k++) {
            ii=(i-1+k%3);
            jj=(nazim+(j-1+k/3))%nazim; /* periodic boundary condition azimuth */
	    if (ii+jj*nrang >= nrang*nazim || ii+jj*nrang < 0) continue;
            if (sign*teximage[ii+jj*nrang]<=sign*threstex) count++;
         }
         /* when not enough neighbours with dBZ>DBZRAIN, continue */
         if (count-1 < NEIGHBOURS) continue;
      }
      else {
//         if (teximage[i+j*nrang]==missing||sign*teximage[i+j*nrang]>sign*threstex){
	    if (!(rhoimage[i+j*nrang]!=missing && zdrimage[i+j*nrang]!=missing &&
                 teximage[i+j*nrang]!=missing && teximage[i+j*nrang]>=dbzmin &&
		(zdrimage[i+j*nrang]>threszdr||rhoimage[i+j*nrang]>thresrho))) continue; 
//	 }
         /* count number of direct neighbours above threshold */
         count=0;
         for (k=0 ; k<9 ; k++) {
            ii=(i-1+k%3);
            jj=(nazim+(j-1+k/3))%nazim; /* periodic boundary condition azimuth */
//            if (sign*teximage[ii+jj*nrang]<=sign*threstex || rhoimage[ii+jj*nrang]>thresrho) count++;
            if (rhoimage[ii+jj*nrang]>thresrho||zdrimage[ii+jj*nrang]>threszdr) count++;
         }
         /* when not enough neighbours with dBZ>DBZRAIN, continue */
         if (count-1 < NEIGHBOURS) continue;
      }
/*Looking for horizontal, vertical, forward diagonal, and backward diagonal */
/*connections.*/

      for (k=0 ; k<4 ; k++) {
         ii=(i-1+k%3);
         jj=(nazim+(j-1+k/3))%nazim; /* periodic boundary condition azimuth */
         /* index out of range, goto next pixel: */
         if (ii<0||ii>=nrang||jj<0||jj>=nazim) continue;
         /* no connection found, goto next pixel */
         if (cellmap[ii+jj*nrang]<0) continue;
         /* if pixel still unassigned, assign same cellnumber as connection */
         if (cellmap[i+j*nrang]<0) cellmap[i+j*nrang]=cellmap[ii+jj*nrang];
         else {
            /* if connection found but pixel is already assigned a different cellnumber: */
            if (cellmap[i+j*nrang]!=cellmap[ii+jj*nrang]) {
               /* merging cells detected: replace all other occurences by value of connection: */
               for (ij=0 ; ij<nazim*nrang ; ij++) {
                  if (cellmap[ij]==cellmap[i+j*nrang]) cellmap[ij]=cellmap[ii+jj*nrang];
                  /*note: not all ncell need to be used eventually */
               }
            }
         }
      }

/*When no connections are found, give a new number.*/

      if (cellmap[i+j*nrang]<0) {
         cellmap[i+j*nrang]=ncell;
         ncell++;
      }
   }
}

/*Returning number of detected cells (including fringe/clutter) .*/

return ncell;
}//findcells


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
//for (ij=0 ; ij<nazim*nrang ; ij++) {
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
      //   if (c[n].area<area||c[n].cv>0.5)
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

/******************************************************************************/
/* This function classifies the range gates, distiguishing between clutter,   */
/* rain, fringes, empty and valid gates. It returns the classification        */
/* and layer counters                                                         */
/******************************************************************************/
void classification(SCANMETA zmeta, SCANMETA vmeta, SCANMETA uzmeta, 
     SCANMETA cmmeta,int *cellmap,
     unsigned char *zscan,unsigned char *vscan,
     unsigned char *uzscan,unsigned char *cmscan,
     float *zdata,int *nzdata,
     float *fracclut,float *fracrain,float *fracbird, float *fracfringe,
     float rminscan,float rmaxscan,float HLAYER,float XOFFSET,
     float XSCALE,float XMEAN,float height,
     float amin,float amax,float vmin,float dbzclutter,float dBZmin,
     float dBZx,float DBZNOISE,int NGAPMIN,int NGAPBIN,int NDBZMIN,
     int layer,int id,int *np,int *Npntp,int *Npntallp,int *Npntclutp,
     int *Npntrainp,int *NpntrainNoFringep,
     unsigned char cmflag,unsigned char uzflag,unsigned char xflag)
{
  int i,j,ij,ia,llayer,l,ndat,ir,n;
  int Npnt,Npntall,Npntclut,Npntrain,NpntrainNoFringe;
  float dBZ,range,heigbeam,azim;
  float FNAN=0.0/0.0;

  n=*np;
  Npnt=*Npntp;
  Npntall=*Npntallp;
  Npntclut=*Npntclutp;
  Npntrain=*Npntrainp;
  NpntrainNoFringe=*NpntrainNoFringep;
  
  l=layer;
  llayer=l*NDATA;

  for (ir=0 ; ir<zmeta.nrang ; ir++) {
     range=(ir+0.5)*zmeta.rscale;
     if (range<rminscan||range>rmaxscan) continue;
     heigbeam=range*sin(zmeta.elev*DEG2RAD)+zmeta.heig;
     if (fabs(height-heigbeam)>0.5*HLAYER) continue;
     for (ia=0 ; ia<zmeta.nazim ; ia++) {
        azim=ia*zmeta.ascale;
	n++;
        if (azim<=amin||azim>amax) continue;
	Npntall++;
	//cluttermap points:
	if (cmflag==1){
           if (ir<cmmeta.nrang && ia<cmmeta.nazim){
	      if (cmmeta.zscale*cmscan[ir+ia*vmeta.nrang]+cmmeta.zoffset>dbzclutter){
		  Npntclut++;
		  continue;
	      }
	   }
	}
	//points without valid reflectivity data, but WITH raw reflectivity data are points
	//dropped by the signal preprocessor. These will be treated as clutter.
	if (uzflag==1){
           if (zscan[ir+ia*zmeta.nrang]==zmeta.missing &&
	       uzscan[ir+ia*uzmeta.nrang]!=uzmeta.missing){
	       Npntclut++;
	       continue;
	   }
	}
	//if non-zero reflectivity but doppler data missing, treat as clutter:
	if (zscan[ir+ia*zmeta.nrang]!=zmeta.missing &&
	    vscan[ir+ia*zmeta.nrang]==vmeta.missing){
	    Npntclut++;
	    continue;
	}
	dBZ=zmeta.zscale*zscan[ir+ia*zmeta.nrang]+zmeta.zoffset;
	if (dBZ < dBZmin) dBZ=DBZNOISE;
	//treat zero doppler speed as clutter:
        if (vscan[ir+ia*vmeta.nrang]!=vmeta.missing&&
            fabs(vmeta.zscale*vscan[ir+ia*vmeta.nrang]+vmeta.zoffset)<vmin){
	    Npntclut++;
	    continue;
	}
	if (cellmap[ir+ia*vmeta.nrang]>0 || dBZ > dBZx) {
	   if (cellmap[ir+ia*vmeta.nrang]>1) { //cluttermap without added fringes
              if (isnan(zdata[1+llayer])) zdata[1+llayer]=0;
	      zdata[1+llayer]+=exp(0.1*log(10)*dBZ);
	      NpntrainNoFringe++;
	   }
	if (isnan(zdata[2+llayer])) zdata[2+llayer]=0;
	zdata[2+llayer]+=exp(0.1*log(10)*dBZ);
	Npntrain++;
	continue;
	}
	if (isnan(zdata[0+llayer])) zdata[0+llayer]=0;
	if (isnan(zdata[2+llayer])) zdata[2+llayer]=0;
	if (xflag==1) zdata[0+llayer]+=exp(0.1*log(10)*dBZ)*XMEAN/(XOFFSET+XSCALE/range);
	else zdata[0+llayer]+=exp(0.1*log(10)*dBZ);
        zdata[2+llayer]+=exp(0.1*log(10)*dBZ);
        Npnt++;
      }//for ia
   }//for ir

  *np=n;
  *Npntp=Npnt;
  *Npntallp=Npntall;
  *Npntclutp=Npntclut;
  *Npntrainp=Npntrain;
  *NpntrainNoFringep=NpntrainNoFringe;

 return; 
}//computelayers



/******************************************************************************/
/*This function updates the cellmap by dropping cells and reindexing the map  */
/*Leaving index 0 unused, will be used for assigning cell fringes             */
/******************************************************************************/
int updatemap(int *cellmap,CELLPROP *c,int Ncell,int Npoints,int area)
{
int ij,n,NcellValid=Ncell;

for (ij=0 ; ij<Npoints ; ij++) {
//   printf("ij=%i,c[ij]4.dbz = %f \n",ij,c[n].dbz);

   if (cellmap[ij]<0) continue;
   if (c[cellmap[ij]].drop) cellmap[ij]=-1;


}

/*Sort the cells by area and determine number of valid cells*/
sortcells(c,Ncell,BYAREA);

while (NcellValid>0&&c[NcellValid-1].area<area) NcellValid--;

/*reindex the map*/
for (n=0 ; n<NcellValid ; n++){ 
   for (ij=0 ; ij<Npoints ; ij++) {
      if (cellmap[ij]==c[n].index) cellmap[ij]=n+1;
   }
}
/*reindex the cellproperties object*/
for (n=0 ; n<Ncell ; n++) {
   c[n].index=n+1;
}

return NcellValid;
} //updatemap


/******************************************************************************/
/*Sorting of the cell properties using cell area or mean.                     */
/*Assume an area or mean equal to zero for cells that are marked 'dropped'    */
/******************************************************************************/
void sortcells(CELLPROP *c,int ncell,int method) 
{
int n,m;
CELLPROP tmp;

/*Sorting of data elements using straight insertion method.*/
if (method==BYAREA){
   for (n=1 ; n<ncell ; n++) {
      tmp=c[n];
      m=n-1;
      while (m>=0&&c[m].area*XABS(c[m].drop-1)<tmp.area*XABS(tmp.drop-1)) {
         c[m+1]=c[m];
         m--;
      }
      c[m+1]=tmp;
   }
}
else if (method==BYMEAN){
   for (n=1 ; n<ncell ; n++) {
      tmp=c[n];
      m=n-1;
      while (m>=0&&c[m].dbz*XABS(c[m].drop-1)<tmp.dbz*XABS(tmp.drop-1)) {
         c[m+1]=c[m];
         m--;
      }
      c[m+1]=tmp;
   }
}
/*Return.*/

return;
}//sortcells


/******************************************************************************/
/*This function enlarges cellmap by additional fringe.                        */
/*First a block around each pixel is searched for pixels within a distance    */
/*equal to 'fringe'.                                                          */
/*                                                                            */
/******************************************************************************/
void fringecells(int *cellmap,SCANMETA meta,float fringe)
{
int i,j,k,ii,jj,nrang,nazim,rblock,ablock,edge;
float ascale,rscale,tmp;
nrang=meta.nrang;
nazim=meta.nazim;
ascale=meta.ascale;
rscale=meta.rscale;


rblock=ROUND(fringe/rscale);
for (j=0 ; j<nazim ; j++){
   for (i=0 ; i<nrang ; i++){
      if(cellmap[i+j*nrang]<=1) continue; //already fringe or not in cellmap
      //determine whether current pixel is a pixel on the edge of a cell
      edge=0;
      for (k=0 ; k < 9 && !edge ; k++){
         ii=(i-1+k%3);
         jj=(nazim+(j-1+k/3))%nazim; 
         if (ii<0||ii>=nrang) continue;
	 if(cellmap[ii+jj*nrang]<1) edge=1; //THIS SHOULD BE <=1, BUT THEN TAKES MUCH MUCH LONGER
      }                                     //NOW ONLY CELL PIXELS WITHOUT ANY BORDERING FRINGE ARE 'FRINGED'
      if(!edge) continue;
      //search a limited block around pixel (i,j) only
      tmp=(XYMAX(0,i-rblock)*rscale*ascale*DEG2RAD);
      if (tmp<fringe/nazim) ablock=nazim;
      else ablock=ROUND(fringe/tmp);
//      printf("i=%i,j=%i,k=%i,ii=%i,jj=%i,id=%i \n",i,j,k,ii,jj,ii+jj*nrang);
      for (k=0 ; k<(2*rblock+1)*(2*ablock+1) ; k++) {
         ii=(i-rblock+k%(2*rblock+1));
         jj=(nazim+(j-ablock+k/(2*rblock+1)))%nazim; // periodic boundary condition azimuth
	 if (ii<0||ii>=nrang) continue;
	 //if not within range or already in cellmap or already a fringe, do nothing
//	 if ((dist(i,j,ii,jj,rscale,ascale)>fringe || cellmap[ii+jj*nrang]>=1) && ii+jj*nrang < 105626) printf("i=%i,j=%i,k=%i,ii=%i,jj=%i,id=%i,distance=%f,fringe=%f\n",i,j,k,ii,jj,ii+jj*nrang,dist(i,j,ii,jj,rscale,ascale),fringe);
         if (dist(i,j,ii,jj,rscale,ascale)>fringe || cellmap[ii+jj*nrang]>=1) continue;
//	 if (ii+jj*nrang < 105626) printf("i=%i,j=%i,k=%i,ii=%i,jj=%i,id=%i,rscale=%f,ascale=%f,fringe=%f\n",i,j,k,ii,jj,ii+jj*nrang,rscale,ascale,fringe);
	 //include pixel (ii,jj) in fringe
//	 if (ii+jj*nrang < 105626) printf("i=%i,j=%i,k=%i,ii=%i,jj=%i,id=%i,old cellmap=%i\n",i,j,k,ii,jj,ii+jj*nrang,cellmap[ii+jj*nrang]);
         cellmap[ii+jj*nrang]=1; //assign index 1 to fringes
      }
   }
} //for
return;
} //fringecells

/******************************************************************************/
/*This function calculates the distance in km between two gates               */
/******************************************************************************/
float dist(int range1, int azim1,int range2,int azim2,float rscale,float ascale)
{
float x1,x2,y1,y2;
x1=range1*cos(azim1*ascale*DEG2RAD);
x2=range2*cos(azim2*ascale*DEG2RAD);
y1=range1*sin(azim1*ascale*DEG2RAD);
y2=range2*sin(azim2*ascale*DEG2RAD);
return sqrt(SQUARE(x1-x2)+SQUARE(y1-y2));
} //dist


/******************************************************************************/
/*This function detects gaps in the azimuthal distribution of the radial      */
/*velocity data. For this, a histogram of the azimuths of the available       */
/*velocity data is made using 'NGAPBIN' azimuth bins. Subsequently, the number*/
/*data points per azimuth bin is determined. When two consecutive azimuth bins*/
/*contain less than 'NGAPMIN' velocity points, a gap is detected.             */
/*The number of velocity datapoints is 'Npnt' and the dimension of each       */
/*x-point is 'Ndx' (x[0...Npnt*Ndx-1]). The azimuth coordinate is assumed to  */
/*be the first x-coordinate.                                                  */
/*The function returns true (1) when a gap is detected and false (0) when no  */
/*gap is found.                                                               */
/******************************************************************************/
int azimuth_gap(float x[],int Ndx,int Npnt,int ngapmin,int NGAPBIN)
{
int gap=0,Nsector[NGAPBIN],n,m;

/*Initialize histogram.*/

for (m=0 ; m<NGAPBIN ; m++) Nsector[m]=0;

/*Collect histogram.*/

//printf("x[Ndx*n]*NGAPBIN=%i, or %f", x[Ndx*n]*NGAPBIN);

for (n=0 ; n<Npnt ; n++) {
   m=(x[Ndx*n]*NGAPBIN)/360.0;
   Nsector[m%NGAPBIN]++;
}

/*Detect gaps.*/

for (gap=0, m=0 ; m<NGAPBIN ; m++) {
   if (Nsector[m]<ngapmin&&Nsector[(m+1)%NGAPBIN]<ngapmin) gap=1;
}

return gap;
}


/******************************************************************************/
/*Basis functions for Singular Value Decomposition linear fitting.            */
/******************************************************************************/

/******************************************************************************/
/*This function contains the basis-functions and other relevant parameters of */
/*the fit model. The function returns the basis functions of the              */
/*multi-parameter linear fit evaluated at vector X=x[0..Ndx-1] in the array   */
/*afunc[0..Npar-1].                                                           */
/*This function is intended to be supplied to the fitting routine 'svdfit'.   */
/*In this case, a three-parameter linear VAD fit is performed on a one        */
/*dimensional vector space 'X'.                                               */
/******************************************************************************/
int svd_vad1func(float x[],int Ndx,float afunc[],int Npar) 
{
if (Ndx!=1) {
   printf("#Number of dimensions is wrong!\n");
   return 0;
}
if (Npar!=3) {
   printf("#Number of parameters is wrong!\n");
   return 0;
}
afunc[0]=0.5;
afunc[1]=sin(DEG2RAD*x[0]);
afunc[2]=cos(DEG2RAD*x[0]);
return 1;
}

/******************************************************************************/
/*This function contains the basis-functions and other relevant parameters of */
/*the fit model. The function returns the basis functions of the              */
/*multi-parameter linear fit evaluated at vector X=x[0..Ndx-1] in the array   */
/*afunc[0..Npar-1].                                                           */
/*This function is intended to be supplied to the fitting routine 'svdfit'.   */
/*In this case, a five-parameter linear VAD fit is performed on a one         */
/*dimensional vector space 'X'.                                               */
/******************************************************************************/
int svd_vad2func(float x[],int Ndx,float afunc[],int Npar) 
{
if (Ndx!=1) {
   printf("#Number of dimensions is wrong!\n");
   return 0;
}
if (Npar!=5) {
   printf("#Number of parameters is wrong!\n");
   return 0;
}
afunc[0]=0.5;
afunc[1]=sin(x[0]*DEG2RAD);
afunc[2]=cos(x[0]*DEG2RAD);
afunc[3]=sin(2*x[0]*DEG2RAD);
afunc[4]=cos(2*x[0]*DEG2RAD);
return 1;
}

/******************************************************************************/
/*This function contains the basis-functions and other relevant parameters of */
/*the fit model. The function returns the basis functions of the              */
/*multi-parameter linear fit evaluated at vector X=x[0..Ndx-1] in the array   */
/*afunc[0..Npar-1].                                                           */
/*This function is intended to be supplied to the fitting routine 'svdfit'.   */
/*In this case, a three-parameter linear Area-VVP fit is performed on a two   */
/*dimensional vector space 'X' in radar coordinates: azimuth (alpha), and     */
/*elevation (gamma).                                                          */
/******************************************************************************/
int svd_vvp1func(float x[],int Ndx,float afunc[],int Npar) 
{
float sinalpha,cosalpha,singamma,cosgamma;
if (Ndx!=2) {
   printf("#Number of dimensions is wrong!\n");
   return 0;
}
if (Npar!=3) {
   printf("#Number of parameters is wrong!\n");
   return 0;
}
sinalpha=sin(x[0]*DEG2RAD);
cosalpha=cos(x[0]*DEG2RAD);
singamma=sin(x[1]*DEG2RAD);
cosgamma=cos(x[1]*DEG2RAD);
afunc[0]=singamma;
afunc[1]=sinalpha*cosgamma;
afunc[2]=cosalpha*cosgamma;
return 1;
}

/******************************************************************************/
/*This function contains the basis-functions and other relevant parameters of */
/*the fit model. The function returns the basis functions of the              */
/*multi-parameter linear fit evaluated at vector X=x[0..Ndx-1] in the array   */
/*afunc[0..Npar-1].                                                           */
/*This function is intended to be supplied to the fitting routine 'svdfit'.   */
/*In this case, a six-parameter linear Area-VVP fit is performed on a three   */
/*dimensional vector space 'X' in radar coordinates: azimuth (alpha),         */
/*elevation (gamma), and range (rho).                                         */
/******************************************************************************/
int svd_vvp2func(float x[],int Ndx,float afunc[],int Npar) 
{
float sinalpha,cosalpha,singamma,cosgamma,rho;
if (Ndx!=3) {
   printf("#Number of dimensions is wrong!\n");
   return 0;
}
if (Npar!=6) {
   printf("#Number of parameters is wrong!\n");
   return 0;
}
sinalpha=sin(x[0]*DEG2RAD);
cosalpha=cos(x[0]*DEG2RAD);
singamma=sin(x[1]*DEG2RAD);
cosgamma=cos(x[1]*DEG2RAD);
rho=x[2]*cosgamma;
afunc[0]=singamma;
afunc[1]=sinalpha*cosgamma;
afunc[2]=cosalpha*cosgamma;
afunc[3]=0.5*rho*cosgamma;
afunc[4]=0.5*rho*(2.0*sinalpha*cosalpha)*cosgamma;
afunc[5]=0.5*rho*(SQUARE(cosalpha)-SQUARE(sinalpha))*cosgamma;
return 1;
}

/******************************************************************************/
/*This function contains the basis-functions and other relevant parameters of */
/*the fit model. The function returns the basis functions of the              */
/*multi-parameter linear fit evaluated at vector X=x[0..Ndx-1] in the array   */
/*afunc[0..Npar-1].                                                           */
/*This function is intended to be supplied to the fitting routine 'svdfit'.   */
/*In this case, a nine-parameter linear Area-VVP fit is performed on a four   */
/*dimensional vector space 'X' in radar coordinates: azimuth (alpha),         */
/*elevation (gamma), range (rho), and height above radar (Znull).             */
/******************************************************************************/
int svd_vvp3func(float x[],int Ndx,float afunc[],int Npar) 
{
float sinalpha,cosalpha,singamma,cosgamma,rho,dheig;
if (Ndx!=4) {
   printf("#Number of dimensions is wrong!\n");
   return 0;
}
if (Npar!=9) {
   printf("#Number of parameters is wrong!\n");
   return 0;
}
sinalpha=sin(x[0]*DEG2RAD);
cosalpha=cos(x[0]*DEG2RAD);
singamma=sin(x[1]*DEG2RAD);
cosgamma=cos(x[1]*DEG2RAD);
rho=x[2]*cosgamma;
dheig=x[2]*singamma-x[3];
afunc[0]=singamma;
afunc[1]=sinalpha*cosgamma;
afunc[2]=cosalpha*cosgamma;
afunc[3]=0.5*rho*cosgamma;
afunc[4]=0.5*rho*(2*sinalpha*cosalpha)*cosgamma;
afunc[5]=0.5*rho*(SQUARE(cosalpha)-SQUARE(sinalpha))*cosgamma;
afunc[6]=sinalpha*cosgamma*dheig;
afunc[7]=cosalpha*cosgamma*dheig;
afunc[8]=singamma*dheig;
return 1;
}

/******************************************************************************/
/*Functions for linear fitting using Singular Value Decomposition:            */
/******************************************************************************/

/******************************************************************************/
/*This function performs a multi-dimensional linear fit using singular value  */
/*decomposition. See Numerical Recipes (2nd ed., paragraph 15.4) for details. */
/*Given a set of data points x[0..Npnt*Ndx-1],y[0..Npnt-1] use Chi-square     */
/*minimization to determine the coefficients a[0..Npar-1] of the fitting      */
/*function y=SUM_i[apar_i*afunc_i(x)]. The dimensionality of the input vector */
/*'x' is equal to 'Ndx', generally this will be equal to 1.                   */
/*Here we solve the fitting equations using singular value decomposition of   */
/*the Npnt by Npar matrix. The program returns values for the array  with fit */
/*parameters 'apar', covariance matrix 'cvm', and Chi-square.                 */
/*The user supplies a function with the fit model 'funcs(x,Ndx,afunc,Npar)'   */
/*that returns the 'Npar' basis functions evaluated at x=x[0..Ndx-1] in the   */
/*array afunc[0..Npar-1].                                                     */
/******************************************************************************/
float svdfit(float *x,int Ndx,float y[],float yfit[],int Npnt,
		int (*funcs)(float x[],int Ndx,float afunc[],int Npar),
					float apar[],float avar[],int Npar) 
{
int i,j,k;
float afunc[NPARMX],w[NPARMX],v[NPARMX*NPARMX],wti[NPARMX],*u;
float wmax,sum,chisq;
/*Checking numbers.*/

if (Npar>NPARMX) {
   printf("#Number of fit parameters is too large!\n");
   return -1.0;
}
if (Npnt<=Npar) {
   printf("#Number of data points is too small!\n");
   return -1.0;
}

/*Allocation of memory for arrays.*/
u=(float *)malloc(Npnt*Npar*sizeof(float));
if (!u) {
   printf("Requested memory could not be allocated!\n");
   return -1.0;
}

/*Filling of the design matrix of the fitting problem (u[i][j]).*/

for (i=0 ; i<Npnt ; i++) {
   if (!(*funcs)(x+Ndx*i,Ndx,afunc,Npar)) return -1.0;
   for (j=0 ; j<Npar ; j++) u[j+Npar*i]=afunc[j];
}

/*Singular value decomposition of the design matrix of the fit.*/

if (!svdcmp(u,Npnt,Npar,w,v)) return -1.0;

/*Removal of the singular values.*/

for (wmax=0.0,j=0 ; j<Npar ; j++) if (w[j]>wmax) wmax=w[j];
for (j=0 ; j<Npar ; j++) if (w[j]<SVDTOL*wmax) w[j]=0.0;

/*Calculation of fit parameters 'apar' using backsubstitution with 'y'.*/

if (!svbksb(u,w,v,Npnt,Npar,y,apar)) return -1.0;

/*Calculation of variances of fit parameters 'apar'.*/

for (i=0 ; i<Npar ;i++) {
   wti[i]=0.0;
   if (w[i]) wti[i]=1.0/(w[i]*w[i]);
}
for (i=0 ; i<Npar ;i++) {
   for (avar[i]=0.0,k=0 ; k<Npar ; k++) avar[i]+=v[i+Npar*k]*v[i+Npar*k]*wti[k];
}

/*Calculation of fitted y's and Chi-square of the fit.*/

for (chisq=0.0,k=0 ; k<Npnt ; k++) {
	
   if (!(*funcs)(x+Ndx*k,Ndx,afunc,Npar)) return -1.0;
   for (sum=0.0,j=0 ; j<Npar ; j++) sum+=apar[j]*afunc[j];
   yfit[k]=sum;
   chisq+=SQUARE(y[k]-yfit[k]);
}
chisq/=Npnt-Npar;

/*Cleaning of memory.*/

free(u);

return chisq;
}

/******************************************************************************/
/*Given a matrix a[0..m*n-1], this function computes its singular value       */
/*decomposition, A=U.W.V^T. The matrix U replaces A on output. The diagonal   */
/*matrix of singular values W is output as a vector w[0..n-1]. The matrix V   */
/*(not the transpose V^T) is output as v[0..n*n-1]. This function has         */
/*been modified from the original one in Numerical Recipes (2nd ed., paragraph*/
/*2.6). Indices in the matrices now run from 0 to n-1/m-1, and allocation of  */
/*arrays is simplified. In addition, the calculation of 'pythagoras' has been */
/*incorporated explicitly. See Numerical Recipes for more details.            */
/******************************************************************************/
int svdcmp(float *a,int m,int n,float w[],float *v) 
{
int flag,i,its,j,jj,k,l=0,nm=0;
float *rv1,anorm,c,f,g,h,s,scale,x,y,z;

/*Allocation of memory.*/

rv1=(float *)malloc(n*sizeof(float));
if (rv1==NULL) {
   printf("Requested memory could not be allocated!\n");
   return 0;
}

/*Start of very stable algorithm by Forsythe et al.*/
/*Householder reduction to bidiagonal form.*/

g=scale=anorm=0.0;
for (i=0 ; i<n ; i++) {
   l=i+1;
   rv1[i]=scale*g;
   g=s=scale=0.0;
   if (i<m) {
      for (k=i ; k<m ; k++) scale+=fabs(a[i+n*k]);
      if (scale) {
         for (k=i ; k<m ; k++) {
            a[i+n*k]/=scale;
            s+=a[i+n*k]*a[i+n*k];
         }
         f=a[i+n*i];
         g=-sqrt(s)*SIGN(f);
         h=f*g-s;
         a[i+n*i]=f-g;
         for (j=l ; j<n ; j++) {
            for (s=0.0,k=i ; k<m ; k++) s+=a[i+n*k]*a[j+n*k];
            f=s/h;
            for (k=i ; k<m ; k++) a[j+n*k]+=f*a[i+n*k];
         }
         for (k=i ; k<m ; k++) a[i+n*k]*=scale;
      }
   }
   w[i]=scale*g;
   g=s=scale=0.0;
   if (i<m&&i!=(n-1)) {
      for (k=l ; k<n ; k++) scale += fabs(a[k+n*i]);
      if (scale) {
         for (k=l ; k<n ; k++) {
            a[k+n*i]/=scale;
            s+=a[k+n*i]*a[k+n*i];
         }
         f=a[l+n*i];
         g=-sqrt(s)*SIGN(f);
         h=f*g-s;
         a[l+n*i]=f-g;
         for (k=l ; k<n ; k++) rv1[k]=a[k+n*i]/h;
         for (j=l ; j<m ; j++) {
            for (s=0.0,k=l ; k<n ; k++) s+=a[k+n*j]*a[k+n*i];
            for (k=l ; k<n ; k++) a[k+n*j]+=s*rv1[k];
         }
         for (k=l ; k<n ; k++) a[k+n*i]*=scale;
      }
   }
   anorm=XYMAX(anorm,(fabs(w[i])+fabs(rv1[i])));
}

/*Accumulation of right-hand transformations.*/

for (i=n-1 ; i>=0 ; i--) {
   if (i<n-1) {
      if (g) {
         for (j=l ; j<n ; j++) v[i+n*j]=(a[j+n*i]/a[l+n*i])/g;
         for (j=l ; j<n ; j++) {
            for (s=0.0,k=l ; k<n ; k++) s+=a[k+n*i]*v[j+n*k];
            for (k=l ; k<n ; k++) v[j+n*k]+=s*v[i+n*k];
         }
      }
      for (j=l ; j<n ; j++) v[j+n*i]=v[i+n*j]=0.0;
   }
   v[i+n*i]=1.0;
   g=rv1[i];
   l=i;
}

/*Accumulation of left-hand transformations.*/

for (i=XYMIN(m,n)-1 ; i>=0 ; i--) {
   l=i+1;
   g=w[i];
   for (j=l ; j<n ; j++) a[j+n*i]=0.0;
   if (g) {
      g=1.0/g;
      for (j=l ; j<n ; j++) {
         for (s=0.0,k=l ; k<m ; k++) s+=a[i+n*k]*a[j+n*k];
         f=(s/a[i+n*i])*g;
         for (k=i ; k<m ; k++) a[j+n*k]+=f*a[i+n*k];
      }
      for (j=i ; j<m ; j++) a[i+n*j]*=g;
   } 
   else for (j=i ; j<m ; j++) a[i+n*j]=0.0;
   ++a[i+n*i];
}

/*Diagonalization of the bidiagonal form: Loop over singular values, and */
/*over allowed iterations.*/

for (k=n-1 ; k>=0 ; k--) {
   for (its=1 ; its<=30 ; its++) {
      flag=1;
      for (l=k ; l>=0 ; l--) {
         nm=l-1;
         if ((float)(fabs(rv1[l])+anorm)==anorm) {
            flag=0;
            break;
         }
         if ((float)(fabs(w[nm])+anorm)==anorm) break;
      }
      if (flag) {
         c=0.0;
         s=1.0;
         for (i=l ; i<=k ; i++) {
            f=s*rv1[i];
            rv1[i]=c*rv1[i];
            if ((float)(fabs(f)+anorm)==anorm) break;
            g=w[i];
            if (fabs(f)>fabs(g)) h=fabs(f)*sqrt(1+SQUARE(g/f));
            else h=(fabs(g)==0?0.0:fabs(g)*sqrt(1+SQUARE(f/g))); 
            w[i]=h;
            h=1.0/h;
            c=g*h;
            s=-f*h;
            for (j=0 ; j<m ; j++) {
               y=a[nm+n*j];
               z=a[i+n*j];
               a[nm+n*j]=y*c+z*s;
               a[i+n*j]=z*c-y*s;
            }
         }
      }

/*Convergence. Singular value is made nonnegative.*/

      z=w[k];
      if (l==k) {
         if (z<0.0) {
            w[k]=-z;
            for (j=0 ; j<n ; j++) v[k+n*j]=-v[k+n*j];
         }
         break;
      }
      if (its==30) {
         printf("#No convergence in 30 svdcmp iterations!\n");
         return 0;
      }
      x=w[l];
      nm=k-1;
      y=w[nm];
      g=rv1[nm];
      h=rv1[k];
      f=((y-z)*(y+z)+(g-h)*(g+h))/(2.0*h*y);
      if (fabs(f)>1.0) g=fabs(f)*sqrt(1+SQUARE(1/f));
      else g=sqrt(1+SQUARE(f)); 
      f=((x-z)*(x+z)+h*((y/(f+g*SIGN(f)))-h))/x;

/*Next QR transformation.*/

      c=s=1.0;
      for (j=l ; j<=nm ; j++) {
         i=j+1;
         g=rv1[i];
         y=w[i];
         h=s*g;
         g=c*g;
         if (fabs(f)>fabs(h)) z=fabs(f)*sqrt(1+SQUARE(h/f));
         else z=(fabs(h)==0?0.0:fabs(h)*sqrt(1+SQUARE(f/h))); 
         rv1[j]=z;
         c=f/z;
         s=h/z;
         f=x*c+g*s;
         g=g*c-x*s;
         h=y*s;
         y*=c;
         for (jj=0 ; jj<n ; jj++) {
            x=v[j+n*jj];
            z=v[i+n*jj];
            v[j+n*jj]=x*c+z*s;
            v[i+n*jj]=z*c-x*s;
         }
         if (fabs(f)>fabs(h)) z=fabs(f)*sqrt(1+SQUARE(h/f));
         else z=(fabs(h)==0?0.0:fabs(h)*sqrt(1+SQUARE(f/h))); 

/*Rotation can be arbitrary if z=0.*/

         w[j]=z;
         if (z) {
            z=1.0/z;
            c=f*z;
            s=h*z;
         }
         f=c*g+s*y;
         x=c*y-s*g;
         for (jj=0 ; jj<m ; jj++) {
            y=a[j+n*jj];
            z=a[i+n*jj];
            a[j+n*jj]=y*c+z*s;
            a[i+n*jj]=z*c-y*s;
         }
      }
      rv1[l]=0.0;
      rv1[k]=f;
      w[k]=x;
   }
}

/*Cleaning of memory.*/

free(rv1);

return 1;
}

/******************************************************************************/
/*This function solves A.X=B for as vector X, where A is specified by the     */
/*arrays u[0..m*n-1], w[0..n-1], and v[0..n*n-1] as returned by function      */
/*'svdcmp' (singular value decomposition). m and n are the dimensions of A,   */
/*and will be equal for square matrices. b[0..m-1] is the input right-hand    */
/*side. x[0..n-1] is the output solution vector. No input quantities are      */
/*destroyed, so the routine may be called sequentially with different b's.    */
/*See Numerical Recipes (2nd ed., paragraph 2.6) for details.                 */
/******************************************************************************/
int svbksb(float *u,float w[],float *v,int m,int n,float b[],float x[]) 
{
int jj,j,i;
float sum,*tmp;

/*Allocation of memory.*/

tmp=(float *)malloc(n*sizeof(float));
if (tmp==NULL) {
   printf("Requested memory could not be allocated!\n");
   return 0;
}

/*First part of inversion: calculation of Tmp = (W^-1.U^T).B. Singular values */
/*of W are discarded.*/

for (j=0 ; j<n ; j++) {
   sum=0.0;
   if (w[j]) {
      for (i=0 ; i<m ; i++) sum+=u[j+n*i]*b[i];
      sum/=w[j];
   }
   tmp[j]=sum;
}

/*Second part: calculation of X = V.Tmp = (V.W^-1.U^T).B = A^-1.B.*/

for (j=0 ; j<n ; j++) {
   sum=0.0;
   for (jj=0 ; jj<n ; jj++) sum+=v[jj+n*j]*tmp[jj];
   x[j]=sum;
}

/*Cleaning of memory.*/

free(tmp);

return 1;
}

/******************************************************************************/
/******************************************************************************/
// 			IDL FUNCTIONS
/******************************************************************************/
/******************************************************************************/


/******************************************************************************/
/* This function receives the arguments from IDL for the call                 */
/* to texture, and calls texture. It returns the updated texture array        */
/* vtex                       						      */
/******************************************************************************/

void call_texture(int argc,void *argv[])
{
  unsigned char* vtex  = (unsigned char*)	argv[0];
  unsigned char* vscan = (unsigned char*)	argv[1];
  unsigned char* zscan = (unsigned char*)	argv[2];
  SCANMETA* texmeta = (SCANMETA*)		argv[3];
  SCANMETA* vmeta   = (SCANMETA*)		argv[4];
  SCANMETA* zmeta   = (SCANMETA*)		argv[5];
  unsigned char* ntexrangp = (unsigned char*)	argv[6];
  unsigned char* ntexazimp = (unsigned char*)	argv[7];
  unsigned char* ntexminp  = (unsigned char*)	argv[8];
  unsigned char* textypep  = (unsigned char*)	argv[9];

  unsigned char ntexrang,ntexazim,ntexmin,textype;
  
  ntexrang=*ntexrangp;
  ntexazim=*ntexazimp;
  ntexmin=*ntexminp;
  textype=*textypep;
  
  texture(vtex,vscan,zscan,texmeta,vmeta,zmeta,
	  ntexrang,ntexazim,ntexmin,textype);

}//call_texture

/******************************************************************************/
/* This function receives the arguments from IDL for the call                 */
/* to findcells, and calls findcells. It returns the number of cells Ncell    */
/* and the updated cellmap array              				      */
/******************************************************************************/

int call_findcells(int argc,void *argv[])
{

  unsigned char* teximage = (unsigned char*)	argv[0];
  unsigned char* rhoimage = (unsigned char*)	argv[1];
  unsigned char* zdrimage = (unsigned char*)	argv[2];
  int* cellmap            = (int*)		argv[3];
  SCANMETA* meta          = (SCANMETA*)		argv[4];
  SCANMETA* rhometa       = (SCANMETA*)		argv[5];
  SCANMETA* zdrmeta       = (SCANMETA*)		argv[6];
  float* threstexminp     = (float*)		argv[7];
  float* thresrhominp     = (float*)		argv[8];
  float* threszdrminp     = (float*)		argv[9];
  float* dbzminp          = (float*)		argv[10];
  float* rcellmaxp        = (float*)		argv[11];
  char* signp             = (char*)		argv[12];

/* IF input parameters need to be NULL pointers
   call them as 255 BYTES (255b or -1b in IDL) or
   an array of bytes, filles with 255b or -1b. In that 
  case they will be recast as NULL pointers here*/
   
  if (*rhoimage == 255) 
  {
    rhoimage=NULL;
    rhometa =NULL;
  }  
  if (*zdrimage == 255) 
  {
    zdrimage=NULL;
    zdrmeta =NULL;
  }			     
  			     
  float threstexmin,thresrhomin,threszdrmin,dbzmin,rcellmax;
  char sign;   
  int Ncell;
  
  thresrhomin = *thresrhominp;   
  threszdrmin = *threszdrminp; 
  dbzmin      = *dbzminp; 
  rcellmax    = *rcellmaxp; 
  sign        = *signp;

  Ncell=findcells(teximage,rhoimage,zdrimage,cellmap,meta,rhometa,
        zdrmeta,threstexmin,thresrhomin,threszdrmin,dbzmin,rcellmax,
	sign);

  return Ncell;
  
}//call_findcells

/******************************************************************************/
/* This function receives the arguments from IDL for the call                 */
/* to analysecells, and calls analysecellscells. It returns the number	      */
/* of valid cells and the updated cellmap.				      */
/******************************************************************************/

int call_analysecells(int argc,void *argv[])
{
  unsigned char* zscan 	= (unsigned char*)		argv[0];
  unsigned char* vscan 	= (unsigned char*)		argv[1];
  unsigned char* vtex  	= (unsigned char*)		argv[2];
  unsigned char* cmscan	= (unsigned char*)		argv[3];
  int* cellmap		= (int*)			argv[4];
  SCANMETA* zmeta	= (SCANMETA*)			argv[5];
  SCANMETA* vmeta   	= (SCANMETA*)			argv[6];
  SCANMETA* texmeta 	= (SCANMETA*)			argv[7];
  SCANMETA* cmmeta 	= (SCANMETA*)			argv[8];
  int* Ncellp		= (int*)			argv[9];
  int* cellareap	= (int*)			argv[10];
  float* dbzcellp	= (float*)			argv[11];
  float* stdevcellp	= (float*)			argv[12];
  float* clutcellp	= (float*)			argv[13];
  float* vminp		= (float*)			argv[14];
  float* dbzclutterp	= (float*)			argv[15];
  unsigned char* cmflagp	= (unsigned char*)	argv[16];
  unsigned char* dualpolflagp	= (unsigned char*)	argv[17];
  unsigned char* verboseflagp 	= (unsigned char*)	argv[18];

  int Ncell,cellarea,NcellValid;
  float dbzcell,stdevcell,clutcell,vmin,dbzclutter;
  unsigned char cmflag,dualpolflag,verboseflag;
  
  Ncell = *Ncellp;
  cellarea = *cellareap;
  dbzcell = *dbzcellp;
  stdevcell = *stdevcellp;
  clutcell = *clutcellp;
  vmin = *vminp;  
  dbzclutter = *dbzclutterp;
  
  cmflag = *cmflagp;
  dualpolflag = *dualpolflagp;
  verboseflag = *verboseflagp;

  NcellValid=analysecells(zscan,vscan,vtex,cmscan,cellmap,zmeta,
       vmeta,texmeta,cmmeta,Ncell,cellarea,dbzcell,stdevcell,
       clutcell,vmin,dbzclutter,cmflag,dualpolflag,verboseflag);
       
  return NcellValid;

}//call_analysecells

/******************************************************************************/
/* This function receives the arguments from IDL for the call                 */
/* to fringecellscells, and calls fringecells. For efficiency the increase in */
/* of indices by 3 and index change -1 to 0 is incoporated here as well       */
/* It returns the updated cellmap.				      	      */
/******************************************************************************/

void call_fringecells(int argc,void *argv[])
{
   int* cellmap		= (int*)		argv[0];
   SCANMETA* zmeta	= (SCANMETA*)		argv[1];
   unsigned char* zscan = (unsigned char*)	argv[2];
   float* emaskp	= (float*)		argv[3];
   float* dBZx		= (float*)		argv[4];
   int* id		= (int*)		argv[5];

   int ij;

/*   (*zmeta).nrang == zmeta->nrang  */

/*increase cellmap indices by 3 and change index -1 (nodata) to 0 */
/*Add pixels with dBZ>dBZx to cellmap, using index NcellValid+1 0 */
/*Now nodata=0,fringe=1,>dBZx=2,cell1=3,cell2=4, etc ...          */

   for (ij=0 ; ij<zmeta->nrang*zmeta->nazim ; ij++){
     if (cellmap[ij]==-1){
	if (zmeta->zscale*zscan[ij]+zmeta->zoffset>*dBZx) cellmap[ij]=2;
	else cellmap[ij]=0;
     }
     else cellmap[ij]+=3;
   }
   fringecells(cellmap,*zmeta,*emaskp);

}//call_fringecells

/******************************************************************************/
/* This function receives the arguments from IDL for the call                 */
/* to classification, and calls classification. It returns zdata, nzdata, and */
/* various range gate classification collections Npts*                        */
/* NOTE: 2D arrays are used, but simply passed as if they were 1D arrays. The */
/* translation is: x[i,j] (IDL) = x[i+jlayer], where jlayer = j*NDATA  (C#)   */
/* No REFORM() is necessary in IDL, just declare as x=REPLICATE(NLAYER,NDATA) */
/******************************************************************************/

void call_classification(int argc,void *argv[])
{
   SCANMETA* zmeta	= (SCANMETA*)		argv[0];
   SCANMETA* vmeta	= (SCANMETA*)		argv[1];
   SCANMETA* uzmeta	= (SCANMETA*)		argv[2];
   SCANMETA* cmmeta	= (SCANMETA*)		argv[3];
   int* cellmap		= (int*)		argv[4];
   unsigned char* zscan = (unsigned char*)	argv[5];
   unsigned char* vscan = (unsigned char*)	argv[6];
   unsigned char* uzscan= (unsigned char*)	argv[7];
   unsigned char* cmscan= (unsigned char*)	argv[8];
   float* zdata		= (float*)		argv[9];
   int* nzdata		= (int*)		argv[10];
   float* fracclut	= (float*)		argv[11];
   float* fracrain	= (float*)		argv[12];
   float* fracbird	= (float*)		argv[13];
   float* fracfringe	= (float*)		argv[14];
   float* rminscan	= (float*)		argv[15];
   float* rmaxscan	= (float*)		argv[16];
   float* HLAYER   	= (float*)		argv[17];
   float* XOFFSET   	= (float*)		argv[18];
   float* XSCALE   	= (float*)		argv[19];
   float* XMEAN   	= (float*)		argv[20];
   float* height 	= (float*)		argv[21];
   float* amin	 	= (float*)		argv[22];
   float* amax	 	= (float*)		argv[23];
   float* vmin		= (float*)		argv[24];
   float* dbzclutter	= (float*)		argv[25];
   float* dBZmin	= (float*)		argv[26];
   float* dBZx		= (float*)		argv[27];
   float* DBZNOISE	= (float*)		argv[28];
   int* NGAPMIN		= (int*)		argv[29];
   int* NGAPBIN		= (int*)		argv[30];
   int* NDBZMIN		= (int*)		argv[31];
   int* layer		= (int*)		argv[32];
   int* id		= (int*)		argv[33];
   int* n		= (int*)		argv[34];
   int* Npnt		= (int*)		argv[35];
   int* Npntall 	= (int*)		argv[36];
   int* Npntclut	= (int*)		argv[37];
   int* Npntrain	= (int*)		argv[38];
   int* NpntrainNoFringe= (int*)		argv[39];
   unsigned char* cmflag= (unsigned char*)	argv[40];
   unsigned char* uzflag= (unsigned char*)	argv[41];
   unsigned char* xflag = (unsigned char*)	argv[42];
  
   classification(*zmeta,*vmeta,*uzmeta,*cmmeta,cellmap,
                 zscan,vscan,uzscan,cmscan,zdata,nzdata,
		 fracclut,fracrain,fracbird,fracfringe,
                 *rminscan,*rmaxscan,*HLAYER,*XOFFSET,*XSCALE,*XMEAN,
		 *height,*amin,*amax,*vmin,*dbzclutter,
		 *dBZmin,*dBZx,*DBZNOISE,*NGAPMIN,*NGAPBIN,*NDBZMIN,
		 *layer,*id,n,Npnt,Npntall,Npntclut,Npntrain,
		 NpntrainNoFringe,*cmflag,*uzflag,*xflag);
 
}


/******************************************************************************/
/* This function receives the arguments from IDL for the call                 */
/* to vap, and calls vap.                                                     */
/******************************************************************************/

void call_vap(int argc,void *argv[])
{
   SCANMETA* vmeta	= (SCANMETA*)		argv[0];
   int* cellmap		= (int*)		argv[1];
   unsigned char* vscan = (unsigned char*)	argv[2];
   float* udata		= (float*)		argv[3];
   float* vdata		= (float*)		argv[4];
   float* wdata		= (float*)		argv[5];
   float* xdata		= (float*)		argv[6];
   float* ndata		= (float*)		argv[7];
   float* cdata		= (float*)		argv[8];
   int* Ndx		= (int*)		argv[9];
   int* Npnt		= (int*)		argv[10];
   int* Npntall 	= (int*)		argv[11];
   int* Npntclut	= (int*)		argv[12];
   int* Npntrain	= (int*)		argv[13];
   int* NpntrainNoFringe= (int*)		argv[14];
   int* Npntmax		= (int*)		argv[15];
   float* rminscan	= (float*)		argv[16];
   float* rmaxscan	= (float*)		argv[17];
   float* HLAYER   	= (float*)		argv[18];
   float* height 	= (float*)		argv[19];
   int* layer		= (int*)		argv[20];
   int* id		= (int*)		argv[21];
   int* n		= (int*)		argv[22];
					

   printf("vap\n");
/*   vvp(vmeta[id],cellmap.(id),vscan.(id), 
       udata,vdata,wdata,xdata,ndata,cdata, 
       Ndx, Npnt,Npntall,Npntclut,Npntrain, 
       NpntrainNoFringe,Npntmax,   	    
       rminscan[id],rmaxscan[id],	    
       HLAYER,height,	    	    	    
       l,id,n,) 			    
*/
}

/******************************************************************************/
/* This function computes the wind velocity components                        */
/******************************************************************************/
void vvp(SCANMETA vmeta,unsigned char *vscan,float *x,float *y,
     int *c,int *cellmap,int Ndx,int *Npntmaxp,int NGAPBIN,
     float rminscan,float rmaxscan,float HLAYER,float height,
     float vmin,int idata,int layer,int id,int *np)
{
  int i,j,ij,ia,k,llayer,l,ir,n,m;
  int Npntmax;
  float heigbeam,range,azim;

  n=*np;
  Npntmax=*Npntmaxp;

  l=layer;
  k=idata;
  llayer=l*NDATA;
  
//  printf("id=%i,k=%i,l=%i,n=%i,Npntmax=%i,",id-1,k,l,n,Npntmax);
//  printf("rminscan=%4.1f,rmaxscan=%4.1f\n",rminscan,rmaxscan);
  
  for (ir=0 ; ir<vmeta.nrang ; ir++) {
     range=(ir+0.5)*vmeta.rscale;
     if (range<rminscan||range>rmaxscan) continue;
     heigbeam=range*sin(vmeta.elev*DEG2RAD)+vmeta.heig;
     if (fabs(height-heigbeam)>0.5*HLAYER) continue;
     for (ia=0 ; ia<vmeta.nazim ; ia++) {
        if (vscan[ir+ia*vmeta.nrang]==vmeta.missing) continue;
	switch (k) { 
	   case 0:
	     if (cellmap[ir+ia*vmeta.nrang]>0) continue; //outside rain clutter map only
	     break;
	   case 1:
	     if (cellmap[ir+ia*vmeta.nrang]<2) continue; //inside rain clutter map without fringe only
	     break;
	}
        x[Ndx*n]=(ia+0.5)*vmeta.ascale;
        x[1+Ndx*n]=vmeta.elev;
        if (Ndx>2) x[2+Ndx*n]=range;
        if (Ndx>3) x[3+Ndx*n]=height-vmeta.heig;
        y[n]=vmeta.zscale*vscan[ir+ia*vmeta.nrang]+vmeta.zoffset;
	c[n]=cellmap[ir+ia*vmeta.nrang];
        if (fabs(y[n])>=vmin) n++;
     }  //for ia
  } //for ir
//  printf("In vvp    n = %i\n",n);

  *np=n;
  *Npntmaxp=Npntmax;

}//vvp


/******************************************************************************/
/* This function receives the arguments from IDL for the call                 */
/* to vvp, and calls vvp.                                                     */
/******************************************************************************/

void call_vvp(int argc,void *argv[])
{
   SCANMETA* vmeta	= (SCANMETA*)		argv[0]; 
   unsigned char* vscan = (unsigned char*)	argv[1]; 
   float* x		= (float*)		argv[2];
   float* y		= (float*)		argv[3];
   int* c		= (int*)		argv[4];
   int* cellmap		= (int*)		argv[5];
   int* Ndx		= (int*)		argv[6];
   int* Npntmax		= (int*)		argv[7];
   int* NGAPBIN		= (int*)		argv[8];
   float* rminscan	= (float*)		argv[9];;
   float* rmaxscan	= (float*)		argv[10];
   float* HLAYER   	= (float*)		argv[11];
   float* height 	= (float*)		argv[12];
   float* vmin		= (float*)		argv[13];
   int* idata		= (int*)		argv[14];
   int* layer		= (int*)		argv[15];
   int* id		= (int*)		argv[16];
   int* n		= (int*)		argv[17];
						     
//   printf("n=%i, Nnpt=%i \n",*n,*Npnt);

   vvp(*vmeta,vscan,x,y,c,cellmap,*Ndx,Npntmax,*NGAPBIN,
       *rminscan,*rmaxscan,*HLAYER,*height,*vmin,*idata,
       *layer,*id,n);

}

/******************************************************************************/
/* This function receives the arguments from IDL for the call                 */
/* to azimuth_gap and svdfit						      */
/******************************************************************************/

int call_fit(int argc,void *argv[])
{
   float* udata		= (float*)		argv[0];
   float* vdata		= (float*)		argv[1];
   float* wdata		= (float*)		argv[2];
   float* xdata		= (float*)		argv[3];
   float* cdata		= (float*)		argv[4];
   int* ndata		= (int*)		argv[5];
   float* x		= (float*)		argv[6];
   float* y		= (float*)		argv[7];
   float* yfit		= (float*)		argv[8];
   int* c		= (int*)		argv[9];
   int* Ndx		= (int*)		argv[10];
   int* Npar   		= (int*)		argv[11];
   int* Npnt   		= (int*)		argv[12];
   int* Npntmax		= (int*)		argv[13];
   int* ngapmin		= (int*)		argv[14];
   int* NGAPBIN		= (int*)		argv[15];
   float* vdmax		= (float*)		argv[16];
   int* vsign		= (int*)		argv[17];
   int* kdata		= (int*)		argv[18];
   int* layer		= (int*)		argv[19];
   
   int k,l,llayer,m,n,Ncell,p,gap;
  
   int id;
   
   float apar[NPARMX],avar[NPARMX],chisq;
   int func_par;
   int (*funcs)(float x[],int Ndx,float afunc[],int Npar)=svd_vvp1func;   

/* Determine the right fit function from the given fit parameters. */
/* The product of the parameters are unique                        */

   func_par=(*Ndx)*(*Npar);
   switch (func_par) { 
     case 3:
       funcs=svd_vad1func;
       printf("#Calculation of VAD1 profile from volume data.\n");
     break;
     case 5:
	 funcs=svd_vad2func;
	 printf("#Calculation of VAD2 profile from volume data.\n");
       break;
     case 6:
       funcs=svd_vvp1func;
//	 printf("#Calculation of VVP1 profile from volume data.\n");
       break;
     case 18:
       funcs=svd_vvp2func;
       printf("#Calculation of VVP2 profile from volume data.\n");
       break;
     case 36:
       funcs=svd_vvp3func;
       printf("#Calculation of VVP3 profile from volume data.\n");
       break;
     default :
       printf("#Unknown svdfit parameters. Aborting.\n");
       exit(11);
   }

   gap=azimuth_gap(x,*Ndx,*Npnt,*ngapmin,*NGAPBIN);
//       if (gap) printf("k=%i,l=%i,n=%i\n",*kdata,*layer,*Npnt);

   if (gap) return gap;
//   printf("Ndx=%i,Npnt=%i,Npar=%i\n",*Ndx,*Npnt,*Npar);


   if ((chisq=svdfit(x,*Ndx,y,yfit,*Npnt,funcs,apar,avar,*Npar))<SVDMIN) return;

   for (n=0, m=0, Ncell=0 ; m<*Npnt ; m++) {
      if (fabs(y[m]-yfit[m])<(*vdmax)) {
//         printf("<  n=%i,vdmax=%f,yfit[n]=%f,fabs(y[n])=%f\n",n,*vdmax,yfit[n],fabs(y[n]));
         for (p=0 ; p<(*Ndx) ; p++) x[p+(*Ndx)*n]=x[p+(*Ndx)*m];
         y[n]=y[m];
   	 c[n]=c[m];
         n++;
	 if (c[m]>0) Ncell++;
      } //if
   } //for
   *Npnt=n;

   gap=azimuth_gap(x,*Ndx,*Npnt,*ngapmin,*NGAPBIN);
   if (gap) return gap;

   if ((chisq=svdfit(x,*Ndx,y,yfit,*Npnt,funcs,apar,avar,*Npar))<SVDMIN) return;

   k=*kdata;
   l=*layer;
   llayer=l*NDATA;

   udata[k+llayer]=(*vsign)*apar[1];
   vdata[k+llayer]=(*vsign)*apar[2];
   wdata[k+llayer]=(*vsign)*apar[0];
   xdata[k+llayer]=sqrt(chisq);
   ndata[k+llayer]=*Npnt;
   cdata[k+llayer]=(float) Ncell/(*Npnt);

}
