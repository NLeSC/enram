/******************************************************************************/
/*This program calculates the VVP bird reflectivity profiles from a           */
/*radar volume scan.                                                          */
/*The results of the analysis per height level are printed to stdout and      */
/*saved in a KNMI HDF5 file.                                                  */
/*Several parameters can be modified using the command line:                  */
/*Execute without arguments for a list of these parameters and help           */
/******************************************************************************/

/*Program: vol2birdprof_h5.c*/
/*Author: Adriaan Dokter (KNMI)*/
/*Written: Jun 2008*/  
/*Version: Feb 2011*/
/*Bugfix in raincell finding 26-04-2013*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h> 
#include "hdf5.h"
#include "hdf5_hl.h"

/******************************************************************************/
/*Definition of standard parameters.                                          */
/******************************************************************************/

#define PI         (3.141592653589793) /*Degrees to radians.*/
#define DEG2RAD    (0.017453293)       /*Degrees to radians.*/
#define RAD2DEG    (57.29578)          /*Radians to degrees.*/
#define LSTR       (128)               /*Length of all strings used.*/
#define NSCANX     (64)                /*Maximum number of elevation scans.*/
#define RADIUS43   (8495.0)            /*Earth radius used for height calculations.*/

/******************************************************************************/
/*Definition of general macros:                                               */
/******************************************************************************/

#define XABS(x)    (((x)<0)?(-(x)):(x))
#define SIGN(x)    (((x)<0)?-1:1)
#define ROUND(x)   (((x)>0)?(int)((x)+0.5):(int)((x)-0.5))
#define SQUARE(x)  ((x)*(x))
#define XYMAX(x,y) (((x)<(y))?(y):(x))
#define XYMIN(x,y) (((x)<(y))?(x):(y))

/******************************************************************************/
/*Defined (default) parameters.                                               */
/******************************************************************************/
#define HLAYER       (0.200) /*Thickness of height layer in km.               */
#define NLAYER       (30)    /*Number of stacked height layers.               */
#define NDATA        (4)     /*Data dimension height layers.                  */
#define RANGMIN      (5.0)   /*Minimum range for WRWP analysis in km.         */
#define RANGMINSTDEV (5.0)   /*Minimum range for WRWP determination for the   */
	                     /*radial velocity st.dev. in km.                 */
#define RANGMAXSTDEV (25.0)  /*Maximum range for WRWP determination for the   */
	                     /*radial velocity st.dev. in km.                 */
#define RANGMAX     (25.0)   /*Maximum range for WRWP analysis in km.         */
#define AZIMMIN     (0.0)    /*Minimum azimuth for WRWP analysis in degrees.  */
#define AZIMMAX     (360.0)  /*Maximum azimuth for WRWP analysis in degrees.  */
#define VRADMIN     (1.0)    /*Minimum magnitude of velocity in m/s.          */
#define NGAPBIN     (8)      /*Number of bins where availability is analyzed. */
#define NGAPMIN     (5)      /*Minimum number of points per analyses bin.     */
#define NDBZMIN     (25)     /*Minimum number of points necessary for valid   */
                             /*height layer reflectivity estimate             */
#define VDIFMAX     (10.0)   /*Maximum deviation of radial velocity in m/s.   */
#define VMASKMAX    (0.0)    /*A radial velocity difference with the          */
		             /*neighbouring azimuth. Pixel lower than VMASKMAX*/
		 	     /*leads to inclusion in rainmask (0=no mask).    */
#define EMASKMAX    (5.0)    /*Border size in km to enlarge rainmask          */
#define RHOMIN      (0.9)    /*Minimum correlation for precipitation classific*/
#define ZDRMIN      (3.0)    /*Minimum differential reflectivity insects/chaff*/
#define DBZMIN      (-100.0) /*Minimum reflectivity used in calc of avg dBZ.  */
#define DBZMAX      (20.0)   /*Maximum reflectivity used in calc of avg dBZ.  */
#define DBZNOISE    (-40.0)  /*Noise level used for missing reflectivity data */
#define DBZRAIN     (0.0)    /*Minimum reflectivity for qualification as rain */
#define DBZCELL     (15.0)   /*Raincell reflectivity threshold                */
#define STDEVCELL   (5.0)    /*Raincell local standard deviation threshold    */
#define AREACELL    (4)      /*Minimum area for a rain cell in pixels         */
#define CLUTPERCCELL (0.5)   /*Maximum percentage of clutter in a cell        */
#define NEIGHBOURS  (5)      /*Minimum number of directly neighouring pixels  */
		             /*with dBZ>DBZRAIN [1-8]                         */
#define STDEVSCALE  (0.1)    /*                                               */				   
#define NTEXBINAZIM (3)      /*Azimuth bins on which to base texture field    */
#define NTEXBINRANG (3)      /*Range bins on which to base texture field      */
#define NTEXMIN     (4)	     /*Min necessary number of gates for texture      */
#define DBZCLUTTER  (-10.0)  /*Minimum reflectivity for clutter               */

#define DBZFACTOR   (360.7)  /*Conversion Z to cm^2/km^3 at C-band (5.3 cm)   */
#define SIGMABIRD   (11.0)   /*Bird radar cross section in cm^2               */
#define STDEVBIRD   (2.0)    /*Min radial standard deviation in m/s for birds */
		
/******************************************************************************/
/*Definition of parameters for sorting:                                       */
/******************************************************************************/
#define BYAREA      (1)
#define BYMEAN      (2)
				   
/******************************************************************************/
/*Definition of parameters for fitting:                                       */
/******************************************************************************/

#define NPARMX     (16)      /*Maximum number of fit parameters.*/
#define SVDTOL     (1e-5)    /*Accuracy in SV decomposition.*/
#define SVDMIN     (1e-5)    /*Minimum valid SV standard deviation.*/

/******************************************************************************/
/*Structure for containing SCAN metadata:                                     */
/******************************************************************************/

struct scanmeta {
    int date;                /*Date of scan data in YYYYMMDD.*/
    int time;                /*Time of scan data in HHMMSS.*/
    float heig;              /*Height of radar antenna in km.*/
    float elev;              /*Elevation of scan in deg.*/
    int nrang;               /*Number of range bins in scan.*/
    int nazim;               /*Number of azimuth rays in scan.*/
    float rscale;            /*Size of range bins in scan in km.*/
    float ascale;            /*Size of azimuth steps in scan in deg.*/
    int azim0;               /*Ray number with which radar scan started.*/
    float zoffset;           /*Offset value of quantity contained by scan.*/
    float zscale;            /*Scale of value of quantity contained by scan.*/
    int missing;             /*Missing value of quantity contained by scan.*/
    float PRFh;              /*High PRF used for scan in Hz.*/
    float PRFl;              /*Low PRF used for scan in Hz.*/
    float pulse;             /*Pulse length in microsec.*/
    float radcnst;           /*Radar constant in dB.*/
    float txnom;             /*Nominal maximum TX power in kW.*/
    float antvel;            /*Antenna velocity in deg/s.*/	
    float lon;               /*Longitude of radar in deg.*/	
    float lat;               /*Latitude of radar in deg.*/	
};

struct cellprop {
    int imax;
    int jmax;
    float dbz;
    float tex;
    float cv;
    float area;
    float clutterarea;
    float max;
    int index;
    char drop;
};

typedef struct scanmeta SCANMETA;
typedef struct cellprop CELLPROP;

/******************************************************************************/
/*Prototypes of local functions:                                              */
/******************************************************************************/

int read_h5_scan(hid_t file,int iscan,char type,SCANMETA *meta,unsigned char *data[]);
void string2datetime(char *string,int *date,int *time);
void printhelp(int argc,char *argv[]);
void texture(unsigned char *teximg,unsigned char *vimage, unsigned char *zimage,
		SCANMETA *vmeta, SCANMETA *zmeta, SCANMETA *texmeta,
		int ntexrang,int ntexazim,int ntexmin);
int findcells(unsigned char *image,unsigned char *rhoimage,unsigned char *zdrimage, int *cellmap,
		SCANMETA *meta,SCANMETA *rhometa,SCANMETA *zdrmeta, float threstexmin,
		float thresrhomin,float zdrmin,float dbzmin, float rcellmax,char sign);
int analysecells(unsigned char *imgz,unsigned char *imgv, unsigned char *imgtex, unsigned char *imgcm,
		int *cellmap,SCANMETA *zmeta,SCANMETA *vmeta,SCANMETA *texmeta,SCANMETA *cmmeta,
		int Ncell,int area,float dbzcell,float stdevcell, float clutcell, float vmin,float dbzclutter,
		unsigned char cmflag,unsigned char dualpolflag, unsigned char verbose);
void fringecells(int *cellmap,SCANMETA meta,float fringe);
float dist(int range1, int azim1,int range2,int azim2,float rscale,float ascale);
void sortcells(CELLPROP *c,int ncell, int method);
int updatemap(int *cellmap,CELLPROP *c, int Ncell,int Npoints, int area);

/******************************************************************************/
/*Prototypes of local HDF5 handling functions.                                */
/******************************************************************************/

herr_t H5LTmake_dataset_zip(hid_t loc_id,char *dset_name,int rank,hsize_t *dims,
		hid_t tid,void *data);
void write_image_group(hid_t file,int num,char *prodname,char *geopar,
		unsigned char *image,int ncols,int nrows,int missing,
		int nodata,float offset,float scale,float elev);
void modify_groupname(char *groupname,int isec,char *section);
void group2productname(char *groupname,char *sec4,char *prodname);

/******************************************************************************/
/*Basis functions for Singular Value Decomposition linear fitting.            */
/******************************************************************************/

int svd_vvp1func(float x[],int Ndx,float afunc[],int Npar);

/******************************************************************************/
/*Prototypes of functions for fitting using Singular Value Decomposition:     */
/******************************************************************************/

float svdfit(float *x,int Ndx,float y[],float yfit[],int Npnt,
			int (*funcs)(float x[],int Ndx,float afunc[],int Npar),
					float apar[],float avar[],int Npar);
int svdcmp(float *a,int m,int n,float w[],float *v);
int svbksb(float *u,float w[],float *v,int m,int n,float b[],float x[]);

/******************************************************************************/
/*Prototypes of other local functions.*/
/******************************************************************************/

int azimuth_gap(float x[],int Ndx,int Npnt,int ngapmin);

/******************************************************************************/
/*The main program:                                                           */
/******************************************************************************/
int main(int argc,char *argv[])
{
char *infile,*outfile,*cmfile=NULL,object[LSTR],string[LSTR]; //,pnb[8];
unsigned char *vscan[NSCANX],*zscan[NSCANX],*uzscan[NSCANX],*cmscan[NSCANX],flag[NSCANX],
	      *rscan[NSCANX],*zdrscan[NSCANX],*vtex[NSCANX],*scan,imgflag=0,
	      uzflag=1,cmflag=0,dualpolflag=0,verboseflag=0; //,vpixel[8];
int *cellmap[NSCANX];
float elevations[NSCANX];
int ndata[NDATA][NLAYER],nzdata[NDATA][NLAYER],Nscanin=0,Nscan=0,Npntmax=0,Ndx=2,Npar=3,Nlev=0,Ncell=0,NcellValid;
float fracclut[NLAYER],fracrain[NLAYER],fracbird[NLAYER],fracfringe[NLAYER];
int Npnt,Npntrain,NpntrainNoFringe,Npntclut,Npntall,zero=0,three=3,id,i,j,k,l,n,m,p,ia,ir,ij,idmin;
float dbzcell=DBZCELL,stdevcell=STDEVCELL,clutcell=CLUTPERCCELL,stdevbird=STDEVBIRD,sigmabird=SIGMABIRD,dbzrain=DBZRAIN,dbzclutter=DBZCLUTTER;
int cellarea=AREACELL,ntexbinazim=NTEXBINAZIM,ntexbinrang=NTEXBINRANG,ntexmin=NTEXMIN,ngapmin=NGAPMIN;
int result=0;
float rmin=RANGMIN,rminstdev=RANGMINSTDEV,rmaxstdev=RANGMAXSTDEV,rmax=RANGMAX,amin=AZIMMIN,amax=AZIMMAX,
      rcellmax=RANGMAX+5,vmin=VRADMIN,vdmax=VDIFMAX,dBZx=DBZMAX,dBZmin=DBZMIN,
	vmask=VMASKMAX,emask=EMASKMAX,rhomin=RHOMIN,zdrmin=ZDRMIN,rangscan=0,minrange=RANGMIN,maxrange=RANGMAX;
float *x,*y,*yfit,*z,apar[NPARMX],avar[NPARMX],chisq,height,heigbeam,range,azim;
char *c;
float dBZ,missing=-9999,texscale=STDEVSCALE,texoffset=0,FNAN=0.0/0.0;
float hdata[NDATA][NLAYER],udata[NDATA][NLAYER],vdata[NDATA][NLAYER],wdata[NDATA][NLAYER],xdata[NDATA][NLAYER],
      zdata[NDATA][NLAYER],cdata[NDATA][NLAYER],etabird[NLAYER],rhobird[NLAYER],dd[NDATA][NLAYER],ff[NDATA][NLAYER];
float rminscan[NSCANX]={0},rmaxscan[NSCANX]={0};
int (*funcs)(float x[],int Ndx,float afunc[],int Npar)=svd_vvp1func;
hid_t h5_in,h5_out,h5_cm=-1;
hsize_t dims,dimsz[2],dimsv[2];
SCANMETA vmeta[NSCANX],zmeta[NSCANX],uzmeta[NSCANX],cmmeta[NSCANX],rmeta[NSCANX],zdrmeta[NSCANX],texmeta[NSCANX];
struct scanmeta meta;
unsigned char *imagez,*imageuz,*imagev,*image;
int Nrows,Ncols;
float xoffset,yoffset,xscale,yscale;
char sec4[LSTR],proj4str[LSTR],groupname[LSTR],prodname[LSTR],geopar[LSTR];
int nodata=255; /* used for exported images only */

/*Reading name of datafile from command line.*/
if (argc<3) {
   printhelp(argc,argv);
   exit(1);
}
infile=argv[1];
outfile=argv[2];
for (m=0 ; m<NSCANX ; m++) flag[m]=1;
for (n=3 ; n<argc ; n++) {
   if (strcmp(argv[n],"--help")==0) printhelp(argc,argv);
   if (strcmp(argv[n],"-h")==0) printhelp(argc,argv);
   if (strcmp(argv[n],"-img")==0) {
      printf("#Including image data in profile output.\n");
      imgflag=1;
   }
   if (strcmp(argv[n],"-cm")==0) {
      cmfile=argv[n+1];	   
      cmflag=1;
   }
   if (strcmp(argv[n],"-dualpol")==0) {
      dualpolflag=1;
   }
   if (strcmp(argv[n],"-verbose")==0) {
      verboseflag=1;
   }
   if (sscanf(argv[n],"-rms%d",&m)==1&&m>0&&m<NSCANX) flag[m]=0;
   sscanf(argv[n],"-rmin%f",&rmin);
   sscanf(argv[n],"-rminstdev%f",&rminstdev);
   sscanf(argv[n],"-rmaxstdev%f",&rmaxstdev);
   if (sscanf(argv[n],"-rmax%f",&rmax)==1) rcellmax=rmax;
   sscanf(argv[n],"-amin%f",&amin);
   sscanf(argv[n],"-amax%f",&amax);
   sscanf(argv[n],"-rcellmax%f",&rcellmax);
   sscanf(argv[n],"-vmin%f",&vmin);
   sscanf(argv[n],"-vdif%f",&vdmax);
   sscanf(argv[n],"-ngapmin%d",&ngapmin);
   sscanf(argv[n],"-dbzx%f",&dBZx);
   sscanf(argv[n],"-dbzmin%f",&dBZmin);
   sscanf(argv[n],"-vmask%f",&vmask);
   sscanf(argv[n],"-emask%f",&emask);
   sscanf(argv[n],"-rhomin%f",&rhomin);
   sscanf(argv[n],"-zdrmin%f",&zdrmin);
   sscanf(argv[n],"-dbzclutter%f",&dbzclutter);
   sscanf(argv[n],"-stdevbird%f",&stdevbird);
   sscanf(argv[n],"-dbzcell%f",&dbzcell);
   sscanf(argv[n],"-stdevcell%f",&stdevcell);
   sscanf(argv[n],"-clutcell%f",&clutcell);
   sscanf(argv[n],"-cellarea%d",&cellarea);
   sscanf(argv[n],"-dbzrain%f",&dbzrain);
   sscanf(argv[n],"-ntexbinazim%d",&ntexbinazim);
   sscanf(argv[n],"-ntexbinrang%d",&ntexbinrang);
   sscanf(argv[n],"-ntexmin%d",&ntexmin);
   if (sscanf(argv[n],"-rscan%imin%f",&m,&rangscan)==2&&m>0&&m<NSCANX) rminscan[m]=rangscan;
   if (sscanf(argv[n],"-rscan%imax%f",&m,&rangscan)==2&&m>0&&m<NSCANX) rmaxscan[m]=rangscan;
} 

/*Switching off error messaging by native HDF5 lib.*/

H5Eset_auto(H5P_DEFAULT,NULL,NULL);

/*Opening of HDF5 radar input file.*/

printf("#Opening of HDF5 radar input file : %s\n",infile);
h5_in=H5Fopen(infile,H5F_ACC_RDONLY,H5P_DEFAULT);
if (h5_in<0) {
   printf("#HDF5 radar input file could not be opened!\n");
   exit(2);
}

/*Opening of HDF5 output file.*/

printf("#Opening of HDF5 radar output file: %s\n",outfile);
h5_out=H5Fcreate(outfile,H5F_ACC_TRUNC,H5P_DEFAULT,H5P_DEFAULT);
if (h5_out<0) {
   printf("#HDF5 radar output file could not be opened!\n");
   exit(3);
}

/*Opening of HDF5 clutter map file.*/
if (cmflag==1){
   printf("#Opening of HDF5 clutter map file: %s\n",cmfile);
   h5_cm=H5Fopen(cmfile,H5F_ACC_RDONLY,H5P_DEFAULT);
   if (h5_cm<0) {
      printf("#HDF5 clutter map file could not be opened!\n");
      exit(4);
   }
}

/*Finalizing rmin/rmax arrays.*/

for (l=0 ; l<NSCANX ; l++) {
   if (rminscan[l]==0) rminscan[l]=rmin;
   if (rmaxscan[l]==0) rmaxscan[l]=rmax;
}
/*Initializing profile arrays.*/

for (l=0 ; l<NLAYER ; l++) {
   for (m=0 ; m<NDATA ; m++) {
      udata[m][l]=vdata[m][l]=wdata[m][l]=xdata[m][l]=zdata[m][l]=cdata[m][l]=ff[m][l]=dd[m][l]=FNAN;
      nzdata[m][l]=ndata[m][l]=0;
   }
   fracclut[l]=fracrain[l]=fracbird[l]=fracfringe[l]=etabird[l]=rhobird[l]=FNAN;
}

/*Reading of radial velocity and reflectivity data of the selected scans.*/
/*Allocating rainmask arrays.*/

H5LTget_attribute_int(h5_in,"/overview","number_scan_groups",&Nscanin);
if (Nscanin<=0&&Nscanin>=NSCANX) {
   printf("#Number of scan groups is out of range!\n");
   exit(5);
}
for (m=0,n=1 ; n<=Nscanin ; n++) {
   if (!flag[n]) continue;
   result=read_h5_scan(h5_in,n,'V',vmeta+m,vscan+m);
   if (result&&read_h5_scan(h5_in,n,'Z',zmeta+m,zscan+m)){
      vtex[m]=(unsigned char *)malloc(vmeta[m].nrang*vmeta[m].nazim*sizeof(unsigned char));
      cellmap[m]=(int *)malloc(vmeta[m].nrang*vmeta[m].nazim*sizeof(int));

      texmeta[m]=vmeta[m]; texmeta[m].zscale=texscale; texmeta[m].zoffset=texoffset;
      if (cellmap[m]==NULL || vtex[m]==NULL) {
         printf("Requested memory could not be allocated!\n");
         return 0;
      }
      if (!read_h5_scan(h5_in,n,'U',uzmeta+m,uzscan+m)) uzflag=0;
      if (cmflag == 1){
         if (read_h5_scan(h5_cm,n,'M',cmmeta+m,cmscan+m)){
	     if (!(cmmeta[m].nrang == zmeta[m].nrang && cmmeta[m].nazim == zmeta[m].nazim)){
                printf("#WARNING: Dimensions of scan %i do not match clutter map dimensions!\n",n);
	     }
         }
         else {
            printf("#Cannot open corresponding clutter map for scan %i! Dropping scan ...\n",n);
	    flag[n]=0;
	    free(vtex[m]);free(cellmap[m]);
	    continue;
         }
      }
      if (dualpolflag == 1){
         if (!(read_h5_scan(h5_in,n,'R',rmeta+m,rscan+m)&&read_h5_scan(h5_in,n,'D',zdrmeta+m,zdrscan+m))){
            printf("#Cannot open correlation coefficient/differential reflectivity data for scan %i! Dropping scan ...\n",n);
	    flag[n]=0;
	    free(vtex[m]);free(cellmap[m]);
	    continue;
	 }
      }
      rminscan[m]=rminscan[n]; //reindex rmin/rmax arrays
      rmaxscan[m]=rmaxscan[n];
      m++;
   }
     else{
         printf("#Cannot open reflectivity/doppler data for scan %i! Dropping scan ...\n",n);
         flag[n]=0;
	 continue;
     }
}
Nscan=m;
if (Nscan==0) {
   printf("#Number of selected scan groups is zero!\n");
   exit(9);
}

/*Copying contents of input file to output file.*/

H5Ocopy(h5_in,"/overview",h5_out,"/overview",H5P_DEFAULT,H5P_DEFAULT);
H5Ocopy(h5_in,"/geographic",h5_out,"/geographic",H5P_DEFAULT,H5P_DEFAULT);
H5Ocopy(h5_in,"/image1",h5_out,"/image1",H5P_DEFAULT,H5P_DEFAULT);
H5Ocopy(h5_in,"/radar1",h5_out,"/radar1",H5P_DEFAULT,H5P_DEFAULT);
H5Ocopy(h5_in,"/visualisation1",h5_out,"/visualisation1",H5P_DEFAULT,H5P_DEFAULT);
H5Ocopy(h5_in,"/visualisation2",h5_out,"/visualisation2",H5P_DEFAULT,H5P_DEFAULT);
H5Ocopy(h5_in,"/visualisation3",h5_out,"/visualisation3",H5P_DEFAULT,H5P_DEFAULT);


if (imgflag==1){
   for (n=1; n<=Nscanin ; n++) { 
      if (!flag[n]) continue;
      sprintf(object,"/scan%d",n);
      H5Ocopy(h5_in,object,h5_out,object,H5P_DEFAULT,H5P_DEFAULT);
      sprintf(object,"/scan%d/scan_Z_data",n);
      H5Adelete_by_name(h5_out, object, "PALETTE",H5P_DEFAULT);
      H5IMlink_palette(h5_out, object, "/visualisation1/color_palette");
      sprintf(object,"/scan%d/scan_uZ_data",n);
      H5Adelete_by_name(h5_out, object, "PALETTE",H5P_DEFAULT);
      H5IMlink_palette(h5_out, object, "/visualisation1/color_palette");
      sprintf(object,"/scan%d/scan_V_data",n);
      H5Adelete_by_name(h5_out, object, "PALETTE",H5P_DEFAULT);
      H5IMlink_palette(h5_out, object, "/visualisation2/color_palette");
      sprintf(object,"/scan%d/scan_W_data",n);
      H5Adelete_by_name(h5_out, object, "PALETTE",H5P_DEFAULT);
      H5IMlink_palette(h5_out, object, "/visualisation3/color_palette");
   }
}

/*Initializing arrays.*/
for (n=0 ; n<Nscan ; n++) {
   for (l=0 ; l<vmeta[n].nrang*vmeta[n].nazim ; l++) {
      vtex[n][l]=0;
      cellmap[n][l]=-1;
   }
   elevations[n]=vmeta[n].elev;
}

/*Compute velocity texture field*/
/*Compute precipitation cellmap for PPIs*/
for (id=0 ; id<Nscan ; id++) {
   texture(vtex[id],vscan[id],zscan[id], vmeta+id, zmeta+id, texmeta+id,ntexbinrang,ntexbinazim,ntexmin);
   if (dualpolflag == 1){
      Ncell=NcellValid=findcells(zscan[id],rscan[id],zdrscan[id],cellmap[id],zmeta+id,
		      rmeta+id,zdrmeta+id,dbzrain,rhomin,zdrmin,dBZmin,rcellmax,-1);
   }
   else {
      Ncell=findcells(zscan[id],NULL,NULL,cellmap[id],zmeta+id,NULL,NULL,dbzrain,rhomin,zdrmin,dBZmin,rcellmax,-1);
   }
   NcellValid=analysecells(zscan[id],vscan[id],vtex[id],cmscan[id],cellmap[id],zmeta+id,
  	   vmeta+id,texmeta+id,cmmeta+id,Ncell,cellarea,dbzcell,stdevcell,clutcell,vmin,dbzclutter,cmflag,dualpolflag,verboseflag);
}

/*increase cellmap indices by 3 and change index -1 (nodata) to 0 */
/*Add pixels with dBZ>dBZx to cellmap, using index 2              */
/*Now nodata=0,fringe=1,>dBZx=2,cell1=3,cell2=4, etc ...          */

for (id=0 ; id<Nscan; id++){
   for (ij=0 ; ij<vmeta[id].nrang*vmeta[id].nazim ; ij++){
     if (cellmap[id][ij]==-1){
	if (zmeta[id].zscale*zscan[id][ij]+zmeta[id].zoffset>dBZx) cellmap[id][ij]=2;
	else cellmap[id][ij]=0;
     }
     else cellmap[id][ij]+=3;
   }
   fringecells(cellmap[id],vmeta[id],emask);
}

/*Printing metadata of selected scans to stdout.*/
  printf("#YYYYMMDD HHMMSS Elev Nrang Nazim Rscale Ascale Voffset Vscale Zoffset Zscale\n");
  for (n=0 ; n<Nscan ; n++) {
     printf("#%08d %06d %4.1f %5d %5d %6.3f %6.3f %7.2f %6.2f %7.2f %6.2f\n",vmeta[n].date,
     vmeta[n].time,vmeta[n].elev,vmeta[n].nrang,vmeta[n].nazim,vmeta[n].rscale,
     vmeta[n].ascale,vmeta[n].zoffset,vmeta[n].zscale,zmeta[n].zoffset,zmeta[n].zscale);
  }

for (l=0 ; l<NLAYER ; l++) {
   Npnt=Npntrain=NpntrainNoFringe=Npntclut=Npntall=n=0;
   height=(l+0.5)*HLAYER;
   for (id=0 ; id<Nscan ; id++) {
      for (ir=0 ; ir<vmeta[id].nrang ; ir++) {
         range=(ir+0.5)*vmeta[id].rscale;
         if (range<rminscan[id]||range>rmaxscan[id]) continue;
         heigbeam=range*sin(vmeta[id].elev*DEG2RAD)+vmeta[id].heig;
         if (fabs(height-heigbeam)>0.5*HLAYER) continue;
         for (ia=0 ; ia<vmeta[id].nazim ; ia++) {
            azim=ia*vmeta[id].ascale;
	    n++;
            if (azim<=amin||azim>amax) continue;
	    Npntall++;
	    //cluttermap points:
	    if (cmflag==1){
               if (ir<cmmeta[id].nrang && ia<cmmeta[id].nazim){
	          if (cmmeta[id].zscale*cmscan[id][ir+ia*vmeta[id].nrang]+cmmeta[id].zoffset>dbzclutter){
		      Npntclut++;
		      continue;
	          }
	       }
	    }
	    //points without valid reflectivity data, but WITH raw reflectivity data are points
	    //dropped by the signal preprocessor. These will be treated as clutter.
	    if (uzflag==1){
               if (zscan[id][ir+ia*zmeta[id].nrang]==zmeta[id].missing &&
		   uzscan[id][ir+ia*uzmeta[id].nrang]!=uzmeta[id].missing){
		   Npntclut++;
		   continue;
	       }
	    }
	    //if non-zero reflectivity but doppler data missing, treat as clutter:
	    if (zscan[id][ir+ia*zmeta[id].nrang]!=zmeta[id].missing &&
	        vscan[id][ir+ia*vmeta[id].nrang]==vmeta[id].missing){
	        Npntclut++;
	        continue;
	    }
	    dBZ=zmeta[id].zscale*zscan[id][ir+ia*zmeta[id].nrang]+zmeta[id].zoffset;
	    if (dBZ < dBZmin) dBZ=DBZNOISE;
	    //treat zero doppler speed as clutter:
            if (vscan[id][ir+ia*vmeta[id].nrang]!=vmeta[id].missing&&
                fabs(vmeta[id].zscale*vscan[id][ir+ia*vmeta[id].nrang]+vmeta[id].zoffset)<vmin){
		Npntclut++;
		continue;
	    }
	    if (cellmap[id][ir+ia*vmeta[id].nrang]>0 || dBZ > dBZx) {
	       if (cellmap[id][ir+ia*vmeta[id].nrang]>1) { //rainmap without added fringes
		  if (isnan(zdata[1][l])) zdata[1][l]=0;
	          zdata[1][l]+=exp(0.1*log(10)*dBZ);
		  NpntrainNoFringe++;
	       }
	       if (isnan(zdata[2][l])) zdata[2][l]=0;
	       zdata[2][l]+=exp(0.1*log(10)*dBZ);
	       Npntrain++;
	       continue;
	    }
	    if (isnan(zdata[0][l])) zdata[0][l]=0;
	    if (isnan(zdata[2][l])) zdata[2][l]=0;
	    else zdata[0][l]+=exp(0.1*log(10)*dBZ);
            zdata[2][l]+=exp(0.1*log(10)*dBZ);
            Npnt++;
         }
      }
   }
   nzdata[0][l]=Npntall-Npntrain-Npntclut;
   nzdata[1][l]=NpntrainNoFringe;
   nzdata[2][l]=Npntall-Npntclut;
   if (Npntall>0){
      fracclut[l]=1.*Npntclut/Npntall;
      fracrain[l]=1.*NpntrainNoFringe/Npntall;
      fracbird[l]=1.*nzdata[0][l]/Npntall;
      fracfringe[l]=1.*(Npntrain-NpntrainNoFringe)/Npntall;
   }
   if (Npnt>NDBZMIN) zdata[0][l]=10*log(zdata[0][l]/nzdata[0][l])/log(10);
   else zdata[0][l]=FNAN;
   if (Npntrain>NDBZMIN) zdata[1][l]=10*log(zdata[1][l]/nzdata[1][l])/log(10);
   else zdata[1][l]=FNAN;
   if (Npnt+Npntrain>NGAPMIN*NGAPBIN) zdata[2][l]=10*log(zdata[2][l]/nzdata[2][l])/log(10);
   else zdata[2][l]=FNAN;
   if (n>Npntmax) Npntmax=n;
}

/* Determine max number of gates per height layer for the range (rminstdev,rmaxstdev) */
for (l=0 ; l<NLAYER ; l++) {
   n=0;
   height=(l+0.5)*HLAYER;
   for (id=0 ; id<Nscan ; id++) {
      for (ir=0 ; ir<vmeta[id].nrang ; ir++) {
         range=(ir+0.5)*vmeta[id].rscale;
         if (range<rminstdev||range>rmaxstdev) continue;
         heigbeam=range*sin(vmeta[id].elev*DEG2RAD)+vmeta[id].heig;
         if (fabs(height-heigbeam)>0.5*HLAYER) continue;
         n+=vmeta[id].nazim;
      }
   }
   if (n>Npntmax) Npntmax=n;
}

/*Allocation of memory for collection of velocity data.*/
x=(float *)malloc(Ndx*(Npntmax+1)*sizeof(float));
y=(float *)malloc(Ndx*(Npntmax+1)*sizeof(float));
z=(float *)malloc(Ndx*(Npntmax+1)*sizeof(float));
yfit=(float *)malloc(Ndx*(Npntmax+1)*sizeof(float));
c=(char *)malloc(Ndx*(Npntmax+1)*sizeof(char));
if (!x||!y||!yfit||!c) {
   printf("Requested memory could not be allocated!\n");
   exit(10);
}

/*Calculation of wind profile using VVP */
   for (k=0 ; k<NDATA ; k++) {
      for (l=0 ; l<NLAYER ; l++) {
         height=(l+0.5)*HLAYER;
         for (n=0,id=0, idmin=0 ; id<Nscan ; id++) {
            if (vmeta[idmin].ascale<vmeta[id].ascale) idmin=id;
	    if (k<3){
	       minrange=rminscan[id]; maxrange=rmaxscan[id];
	    }
	    else{
	       minrange=rminstdev; maxrange=rmaxstdev;
	    }	    
            for (ir=0 ; ir<vmeta[id].nrang ; ir++) {
               range=(ir+0.5)*vmeta[id].rscale;
               if (range<minrange||range>maxrange) continue;
               heigbeam=range*sin(vmeta[id].elev*DEG2RAD)+vmeta[id].heig;
               if (fabs(height-heigbeam)>0.5*HLAYER) continue;
               for (ia=0 ; ia<vmeta[id].nazim ; ia++) {
                  if (vscan[id][ir+ia*vmeta[id].nrang]==vmeta[id].missing) continue;
		  switch (k) { 
		     case 0:
		       if (cellmap[id][ir+ia*vmeta[id].nrang]>0) continue; //outside rain clutter map only
		       break;
		     case 1:
		       if (cellmap[id][ir+ia*vmeta[id].nrang]<2) continue; //inside rain clutter map without fringe only
		       break;
		  }
		  if (cmflag==1){
	             if (cmmeta[id].zscale*cmscan[id][ir+ia*vmeta[id].nrang]+cmmeta[id].zoffset>dbzclutter) continue;
	          }
                  x[Ndx*n]=(ia+0.5)*vmeta[id].ascale;
                  x[1+Ndx*n]=vmeta[id].elev;
                  if (Ndx>2) x[2+Ndx*n]=range;
                  if (Ndx>3) x[3+Ndx*n]=height-vmeta[id].heig;
                  y[n]=vmeta[id].zscale*vscan[id][ir+ia*vmeta[id].nrang]+vmeta[id].zoffset;
                  z[n]=zmeta[id].zscale*zscan[id][ir+ia*zmeta[id].nrang]+zmeta[id].zoffset;
	          c[n]=cellmap[id][ir+ia*vmeta[id].nrang];
                  if (fabs(y[n])>=vmin) n++;
               }  //for
            } //for
         } //for
         Npnt=n;
         if (azimuth_gap(x,Ndx,Npnt,ngapmin)) continue;
         if ((chisq=svdfit(x,Ndx,y,yfit,Npnt,funcs,apar,avar,Npar))<SVDMIN) continue;      
         for (n=0, m=0, Ncell=0 ; m<Npnt ; m++) {
            if (fabs(y[m]-yfit[m])<vdmax) {
               for (p=0 ; p<Ndx ; p++) x[p+Ndx*n]=x[p+Ndx*m];
               y[n]=y[m];
               z[n]=z[m];
   	       c[n]=c[m];
               n++;
	       if (c[m]>0) Ncell++;
            } //if
         } //for
         Npnt=n;
         if (azimuth_gap(x,Ndx,Npnt,ngapmin)) continue;
         if ((chisq=svdfit(x,Ndx,y,yfit,Npnt,funcs,apar,avar,Npar))<SVDMIN) continue;
         udata[k][l]=apar[1];
         vdata[k][l]=apar[2];
         wdata[k][l]=apar[0];
         xdata[k][l]=sqrt(chisq);
         ndata[k][l]=Npnt;
         cdata[k][l]=(float) Ncell/Npnt;
	 if(!(isnan(udata[k][l])||isnan(vdata[k][l]))){
            ff[k][l]=sqrt(udata[k][l]*udata[k][l]+vdata[k][l]*vdata[k][l]);
            dd[k][l]=atan2(udata[k][l],vdata[k][l])*RAD2DEG;
	 }
         if (dd[k][l]<0) dd[k][l]+=360;
      } //for
   } //for

/*Removing height layers without any data.*/
for (n=0,l=0 ; l<NLAYER ; l++) {
   if (ndata[0][l]<=Npar && ndata[1][l]<=Npar && ndata[2][l]<=Npar) continue;
   for (k=0 ; k<NDATA ; k++ ) {
      udata[k][n]=udata[k][l];
      vdata[k][n]=vdata[k][l];
      wdata[k][n]=wdata[k][l];
      xdata[k][n]=xdata[k][l];
      zdata[k][n]=zdata[k][l];
      ndata[k][n]=ndata[k][l];
      cdata[k][n]=cdata[k][l];
      hdata[k][n]=(l+0.5)*HLAYER;
      ff[k][n]=ff[k][l];
      dd[k][n]=dd[k][l];
      if (ndata[k][n]==0) {
	  zdata[k][n]=FNAN;
      }
   }
   fracclut[n]=fracclut[l];
   fracrain[n]=fracrain[l];
   fracfringe[n]=fracfringe[l];
   fracbird[n]=fracbird[l];
   n++;
   if (n>Nlev) Nlev=n;
}

/*Printing of profile to stdout.*/

printf("#Date    Time Heig Uwind  Vwind  Speed Direc Wwind  Count StdDev dBZAll dBZBird Precip\n");
for (l=0 ; l<Nlev ; l++) {
   printf("%08d %04d %4.1f %6.2f %6.2f %5.2f %5.1f %6.2f %5d %6.2f %6.2f %6.2f %4.2f\n",vmeta[0].date,
   	vmeta[0].time/100,hdata[2][l],udata[0][l],vdata[0][l],ff[0][l],dd[0][l],wdata[0][l],ndata[2][l],
	xdata[2][l],zdata[2][l],zdata[0][l],cdata[2][l]);
}//for

/*Converting reflectivity factor to bird reflectivity*/
for (l=0 ; l<Nlev ; l++) {
  if (xdata[2][l]>stdevbird){
	  etabird[l]=DBZFACTOR*pow(10,zdata[0][l]/10);
	  rhobird[l]=etabird[l]/sigmabird;
  }
  else {
	  etabird[l]=0;
	  rhobird[l]=0;
  }
}
/*Saving profile to HDF5 output file.*/ 
H5Gcreate(h5_out,"/overview",H5P_DEFAULT,H5P_DEFAULT,H5P_DEFAULT);
H5LTset_attribute_int(h5_out,"/overview","number_scan_groups",&zero,1);
H5LTset_attribute_int(h5_out,"/overview","number_profile_groups",&three,1);
H5LTset_attribute_float(h5_out,"/overview","elevations_used",elevations,Nscan);
H5LTset_attribute_float(h5_out,"/overview","ranges_used_min",rminscan,Nscan);
H5LTset_attribute_float(h5_out,"/overview","ranges_used_max",rmaxscan,Nscan);
dims=Nlev;
H5Gcreate(h5_out,"/profile1",H5P_DEFAULT,H5P_DEFAULT,H5P_DEFAULT);
H5LTset_attribute_int(h5_out,"/profile1","profile_number_levels",&Nlev,1);
H5LTset_attribute_float(h5_out,"/profile1","profile_missing_data",&missing,1);
H5LTset_attribute_float(h5_out,"/profile1","profile_stdev_threshold",&stdevbird,1);
H5LTset_attribute_float(h5_out,"/profile1","profile_sigmabird",&sigmabird,1);
H5LTmake_dataset_float(h5_out,"/profile1/profile_height",1,&dims,hdata[0]);
H5LTmake_dataset_float(h5_out,"/profile1/profile_u_bird",1,&dims,udata[0]);
H5LTmake_dataset_float(h5_out,"/profile1/profile_v_bird",1,&dims,vdata[0]);
H5LTmake_dataset_float(h5_out,"/profile1/profile_w_bird",1,&dims,wdata[0]);
H5LTmake_dataset_float(h5_out,"/profile1/profile_speed",1,&dims,ff[0]);
H5LTmake_dataset_float(h5_out,"/profile1/profile_direction",1,&dims,dd[0]);
H5LTmake_dataset_float(h5_out,"/profile1/profile_radial_stddev",1,&dims,xdata[0]);
H5LTmake_dataset_float(h5_out,"/profile1/profile_radial_stddev_wind",1,&dims,xdata[2]);
H5LTmake_dataset_float(h5_out,"/profile1/profile_radial_stddev_wind_closerange",1,&dims,xdata[3]);
H5LTmake_dataset_float(h5_out,"/profile1/profile_reflectivity",1,&dims,zdata[0]);
H5LTmake_dataset_float(h5_out,"/profile1/profile_bird_reflectivity",1,&dims,etabird);
H5LTmake_dataset_float(h5_out,"/profile1/profile_bird_density",1,&dims,rhobird);
H5LTmake_dataset_float(h5_out,"/profile1/profile_fraction_bird",1,&dims,fracbird);
H5LTmake_dataset_float(h5_out,"/profile1/profile_fraction_nonbird",1,&dims,fracrain);
H5LTmake_dataset_float(h5_out,"/profile1/profile_fraction_fringe",1,&dims,fracfringe);
H5LTmake_dataset_float(h5_out,"/profile1/profile_fraction_clutter",1,&dims,fracclut);
H5LTmake_dataset_float(h5_out,"/profile1/profile_fraction_cellmap",1,&dims,cdata[2]);
H5LTmake_dataset_int(h5_out,"/profile1/profile_number",1,&dims,ndata[0]);
H5LTmake_dataset_int(h5_out,"/profile1/profile_dbz_number",1,&dims,nzdata[0]);

if (imgflag==1){
   for (n=1,m=0; n<=Nscanin ; n++) { 
      if (!flag[n]) continue;
      //Writing 8-bit zipped image.
      dimsv[0]=(vmeta[m].nazim);
      dimsv[1]=(vmeta[m].nrang);
      dimsz[0]=(zmeta[m].nazim);
      dimsz[1]=(zmeta[m].nrang);

      sprintf(object,"/scan%d/scan_cellmap_data",n);
      H5LTmake_dataset_zip(h5_out,object,2,dimsv,H5T_NATIVE_INT,cellmap[m]);
      H5LTset_attribute_string(h5_out,object,"CLASS","IMAGE");
      H5LTset_attribute_string(h5_out,object,"IMAGE_VERSION","1.2");
      H5LTset_attribute_string(h5_out,object,"DISPLAY_ORIGIN","UL");
      H5Adelete_by_name(h5_out, object, "PALETTE",H5P_DEFAULT);
      H5IMlink_palette(h5_out, object, "/visualisation1/color_palette");
      //Writing calibration information.
      sprintf(object,"/scan%d/calibration/",n);
      sprintf(string,"GEO=%g*PV+0.0",1.0);
      H5LTset_attribute_string(h5_out,object,"calibration_cellmap_formulas",string);
      H5LTset_attribute_string(h5_out,object,"calibration_missing_data","0");

      sprintf(object,"/scan%d/scan_vtex_data",n);
      H5LTmake_dataset_zip(h5_out,object,2,dimsv,H5T_NATIVE_UCHAR,vtex[m]);
      H5LTset_attribute_string(h5_out,object,"CLASS","IMAGE");
      H5LTset_attribute_string(h5_out,object,"IMAGE_VERSION","1.2");
      H5LTset_attribute_string(h5_out,object,"DISPLAY_ORIGIN","UL");
      H5Adelete_by_name(h5_out, object, "PALETTE",H5P_DEFAULT);
      H5IMlink_palette(h5_out, object, "/visualisation1/color_palette");
      //Writing calibration information.
      sprintf(object,"/scan%d/calibration/",n);
      sprintf(string,"GEO=%g*PV+%g",STDEVSCALE,0.0);
      H5LTset_attribute_string(h5_out,object,"calibration_vtex_formulas",string);
      sprintf(string,"%i",vmeta[m].missing);
      H5LTset_attribute_string(h5_out,object,"calibration_missing_data",string);
      
      m++;
   }
}


//Allocation of memory for Cartesian PPI image data.

imageuz=(unsigned char *)malloc(4*zmeta[0].nrang*zmeta[0].nrang);
imagez=(unsigned char *)malloc(4*zmeta[0].nrang*zmeta[0].nrang);
imagev=(unsigned char *)malloc(4*vmeta[0].nrang*vmeta[0].nrang);
if (!imagez || !imageuz || !imagev) {
   printf("Requested memory could not be allocated!\n");
   exit(11);
}

//Calculation of Cartesian PPI image from scan data for lowest used elevation.
//Setting geographical projection parameters of Cartesian grid.
if (uzflag==1) l=3;
else l=2;
image=imagez; meta=(zmeta[0]); scan=zscan[0];
for (k=0; k<l; k++){
   if (k==1) {
      image=imagev; meta=vmeta[0]; scan=vscan[0];
   }
   if (k==2) {
      image=imageuz; meta=uzmeta[0]; scan=uzscan[0];
   }
   Nrows=2*meta.nrang;
   Ncols=2*meta.nrang;
   xoffset=-meta.nrang;
   yoffset=-meta.nrang;
   xscale=meta.rscale;
   yscale=-meta.rscale;
   for (j=0 ; j<Nrows ; j++) {
      for (i=0 ; i<Ncols ; i++) {
         range=sqrt(SQUARE((i+0.5+xoffset)*xscale)+SQUARE((j+0.5+yoffset)*yscale));
         azim=atan2((i+0.5+xoffset)*xscale,(j+0.5+yoffset)*yscale)*RAD2DEG;
         ir=(int)(range/meta.rscale);
         ia=(int)(azim/meta.ascale);
         ia=(ia+meta.nazim)%meta.nazim;
         if (ir<meta.nrang) image[i+j*Ncols]=scan[ir+ia*meta.nrang];
         else image[i+j*Ncols]=nodata;
      }
   }
}
//Reading, modifying, and writing product groupname, and creating product name.
H5LTget_attribute_string(h5_in,"/overview","product_group_name",groupname);
modify_groupname(groupname,3,"BRD");
H5LTset_attribute_string(h5_out,"/overview","product_group_name",groupname);
sprintf(sec4,"E%3.1f",vmeta[0].elev);
group2productname(groupname,sec4,prodname);

//Writing of Cartesian PPIs to HDF5 image groups.

strcpy(geopar,"REFLECTIVITY_[DBZ]");
write_image_group(h5_out,1,prodname,geopar,imagez,2*zmeta[0].nrang,2*zmeta[0].nrang,
	zmeta[0].missing,nodata,zmeta[0].zoffset,zmeta[0].zscale,zmeta[0].elev);
strcpy(geopar,"RADIAL_VELOCITY_[M/S]");
write_image_group(h5_out,2,prodname,geopar,imagev,2*vmeta[0].nrang,2*vmeta[0].nrang,
	vmeta[0].missing,nodata,vmeta[0].zoffset,vmeta[0].zscale,vmeta[0].elev);
if (uzflag==1){
   strcpy(geopar,"UNCORRECTED_REFLECTIVITY_[DBZ]");
   write_image_group(h5_out,3,prodname,geopar,imageuz,2*uzmeta[0].nrang,2*uzmeta[0].nrang,
        uzmeta[0].missing,nodata,uzmeta[0].zoffset,uzmeta[0].zscale,uzmeta[0].elev);
}

//Updating HDF5 geographic group.

H5LTset_attribute_int(h5_out,"/geographic","geo_number_columns",&Ncols,1);
H5LTset_attribute_int(h5_out,"/geographic","geo_number_rows",&Nrows,1);
H5LTset_attribute_float(h5_out,"/geographic","geo_pixel_size_x",&xscale,1);
H5LTset_attribute_float(h5_out,"/geographic","geo_pixel_size_y",&yscale,1);
H5LTset_attribute_float(h5_out,"/geographic","geo_column_offset",&xoffset,1);
H5LTset_attribute_float(h5_out,"/geographic","geo_row_offset",&yoffset,1);

//Updating HDF5 map_projection subgroup.

sprintf(proj4str,"+proj=aeqd +a=6378.137 +b=6356.752 +R_A +lat_0=%.3f +lon_0=%.3f +x_0=0 +y_0=0",vmeta[0].lat,vmeta[0].lon);
H5LTset_attribute_string(h5_out,"/geographic/map_projection","projection_indication","Y");
H5LTset_attribute_string(h5_out,"/geographic/map_projection","projection_name","AZIMUTHAL_EQUIDISTANCE");
H5LTset_attribute_string(h5_out,"/geographic/map_projection","projection_proj4_params",proj4str);

//Cleaning memory.

free(x);
free(y);
free(yfit);
for (id=0 ; id<Nscan ; id++) {
   free(vscan[id]);
   free(zscan[id]);
   free(cellmap[id]);
}

//free(imagez);
//free(imagev);
//free(imageuz);

/*Closing of files.*/

H5Fclose(h5_in);
H5Fclose(h5_out);

/*End of program.*/

exit(0);
}

/******************************************************************************/
/*LOCAL FUNCTIONS:                                                            */
/******************************************************************************/

/******************************************************************************/
/*This function prints the help message to stdout                             */
/******************************************************************************/
void printhelp(int argc,char *argv[])
{
printf("usage: %s <h5-volume> <h5-profile> [options]\n",argv[0]); 
printf("\n   Version 1.0.0 (23-Apr-2013)\n");
printf("\n   Common user options, with defaults in []:\n"); 
printf("      -h --help               show this message                 \n"); 
printf("      -verbose                Print rain/clutter cell properties to stdout [not set]\n");
printf("      -rmin<range>            Minimum range used in km [%3.1f]\n",RANGMIN); 
printf("      -rmax<range>            Maximum range used in km [%3.1f]\n",RANGMAX); 
printf("      -rminstdev<range>       Minimum range used in km for determination radial velocity standard deviation [%3.1f]\n",RANGMINSTDEV); 
printf("      -rmaxstdev<range>       Maximum range used in km for determination radial velocity standard deviation [%3.1f]\n",RANGMAXSTDEV); 
printf("      -amin<azimuth>          Minimum azimuth used in height layer reflectivity average in deg [%3.1f]\n",AZIMMIN); 
printf("      -amax<azimuth>          Maximum azimuth used in height layer reflectivity average in deg [%3.1f]\n",AZIMMAX); 
printf("      -rms<scan>              Exclude scan <scan> [not set]\n"); 
printf("      -rscan<scan>min<range>  Set minimum range for scan <scan> [rmin]\n"); 
printf("      -rscan<scan>max<range>  Set maximum range for scan <scan> [rmax]\n"); 
printf("      -img                    Output rainmask and texture fields into <h5-profile> [not set]\n"); 
printf("      -sigmabird              Bird radar cross section in cm^2 [%3.1f]\n",SIGMABIRD); 

printf("\n   Rain/clutter masking user options, with defaults in []:\n"); 
printf("      -cm <h5-cluttermap>  Assign static clutter map in KNMI hdf5 format [not set]\n"); 
printf("      -dbzclutter<x>       Reflectivity threshold in static cluttermap for excluding pixels as clutter in dBZ [%3.1f]\n",DBZCLUTTER); 
printf("      -vmin<x>             Minimum absolute value of velocity of pixels that are excluded as clutter in m/s [%3.1f]\n",VRADMIN); 
printf("      -dbzx<x>             Maximum pixel reflectivity to include in height layer averaged reflectivity in dBZ [%3.1f]\n",DBZMAX); 
printf("      -dbzrain<x>          Lower reflectivity threshold in dBZ for considering pixels for inclusion in clutter/rain map [%3.1f]\n",DBZRAIN); 
printf("      -dbzcell<x>          Lower threshold of cell-averaged reflectivity for valid raincells  [%3.1f]\n",DBZCELL); 
printf("                           Cell-averaged reflectivity calculated for clutter free areas (ref. -vmin and -cm flags) only\n"); 
printf("      -stdevcell<x>        Upper threshold of cell-averaged radial velocity standard deviation for valid raincells [%3.1f]\n",STDEVCELL); 
printf("                           Cell-averaged radial velocity calculated for clutter free areas (ref. -vmin and -cm flags) only\n"); 
printf("      -clutcell<x>         Always keep raincells in the cluttermap that have a higher fraction of clutter than this threshold. [%3.1f]\n",CLUTPERCCELL); 
printf("                           Guarantees that the central clutter area around the radar is never dropped from cluttermap\n");
printf("                           by -dbzcell/-stdevcell criteria\n"); 
printf("      -emask<x>            Width of fringe by which to extend the rainmask in km [%3.1f]\n",EMASKMAX); 
printf("      -cellarea<x>         Drop cells from cluttermap that have a smaller area than this threshold in no of pixels [%i]\n",AREACELL); 
printf("                           False detections of small raincells often occur within areas of birds.\n");
printf("                           Dropping these cells prevents that a disproportionately\n");
printf("		           large area would be masked as rainclutter after fringing these cells according to -emask criterium\n"); 
printf("      -rcellmax<x>         Maximum range over which to compute rainmask in km [rmax+5]\n"); 
printf("      -ntexbinazim<x>      Azimuthal length in number of pixels of the area over which to calculate texture field [%i]\n",NTEXBINAZIM); 
printf("      -ntexbinrang<x>      Range length in number of pixels of the area over which to calculate texture field [%i]\n",NTEXBINRANG); 
printf("      -ntexmin<x>          Minimum number of valid pixels for allowing texture field calculation [%i]\n",NTEXMIN); 

printf("\n   Radial velocity profiling user options, with defaults in []:\n"); 
printf("      -stdevbird    Radial velocity standard deviation threshold to decide on bird presence [%3.2f]\n",STDEVBIRD); 
printf("      -vdif<x>      Maximum allowed deviation from first fit in m/s (to drop outliers) [%3.1f]\n",VDIFMAX); 
printf("      -ngapmin<x>   Minimum required number of data points per %3.1f deg azimuth segment [%i]\n",360./NGAPBIN,NGAPMIN); 

printf("\n   Dual polarization user options, with defaults in []:\n"); 
printf("      -dualpol     Use dual-pol fields for rain masking [not set]\n"); 
printf("      -rhomin      Correlation coefficient threshold for inclusion in rainmap [%3.1f]\n",RHOMIN); 
printf("      -zdrmin      Differential reflectivity threshold for inclusion in rainmap (to mask insect echoes) [%3.1f]\n",ZDRMIN); 

exit(1);
} //printhelp

/******************************************************************************/
/*This function computes a texture parameter based on a block of (ntexrang x  */
/* ntexazim) pixels. The texture parameter equals the local standard deviation*/
/*in the velocity field.                                                      */
/******************************************************************************/
void texture(unsigned char *teximg,unsigned char *vimage, unsigned char *zimage,
		SCANMETA *vmeta,SCANMETA *zmeta,SCANMETA *texmeta,
		int ntexrang,int ntexazim,int ntexmin)
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
         index=ii+jj*nrang;
	 if (index >= nrang*nazim || index < 0) continue;
         if (vimage[ii+jj*nrang]==missing || zimage[ii+jj*nrang]==missing) continue;
	 else value=vimage[ii+jj*nrang];
         vmoment1+=(vmeta->zoffset)+(vmeta->zscale)*(value);
         vmoment2+=SQUARE((vmeta->zoffset)+(vmeta->zscale)*(value));
         dbz+=(zmeta->zoffset)+(zmeta->zscale)*(zimage[ii+jj*nrang]);
	 count++;
      }
      vmoment1/=count;
      vmoment2/=count;
      dbz/=count;
      /* when not enough neighbours, continue */
      if (count < ntexmin) teximg[i+j*nrang]=missing;
      else {
	 tex=sqrt(XABS(vmoment2-SQUARE(vmoment1)));
	 teximg[i+j*nrang]=ROUND((tex-texmeta->zoffset)/(texmeta->zscale));
      } //else
   } //for
} //for 
} //texture

/******************************************************************************/
/*This function detects the cells in 'image' using an integer threshold value */
/*of 'thres' and a non-recursive algorithm which looks for neighboring pixels */
/*above threshold. On return the marked cells are contained by 'cellmap'. The */
/*number of detected cells/highest index value is returned.                   */
/******************************************************************************/
int findcells(unsigned char *teximage,unsigned char *rhoimage, unsigned char *zdrimage,int *cellmap,
		SCANMETA *meta, SCANMETA *rhometa, SCANMETA *zdrmeta, float threstexmin,float thresrhomin,float threszdrmin,
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
	    if (!(rhoimage[i+j*nrang]!=missing && zdrimage[i+j*nrang]!=missing &&
                 teximage[i+j*nrang]!=missing && teximage[i+j*nrang]>=dbzmin &&
		(zdrimage[i+j*nrang]>threszdr||rhoimage[i+j*nrang]>thresrho))) continue; 
         /* count number of direct neighbours above threshold */
         count=0;
         for (k=0 ; k<9 ; k++) {
            ii=(i-1+k%3);
            jj=(nazim+(j-1+k/3))%nazim; /* periodic boundary condition azimuth */
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
}

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
nrang=vmeta->nrang;
nazim=vmeta->nazim;

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
for (ir=0 ; ir<vmeta->nrang ; ir++) {
   ij=ir+ia*vmeta->nrang;
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
      if (ia<cmmeta->nazim && ir<vmeta->nrang){
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

free(c);
  
//Printing of cell properties to stdout.
if (verbose==1){
   printf("#Cell analysis for elevation %f:\n",zmeta->elev);
   printf("#Minimum cell area in pixels   : %i\n",area);
   printf("#Threshold for mean dBZ cell   : %g dBZ\n",dbzcell);
   printf("#Threshold for mean stdev cel  : %g m/s\n",stdevcell);
   printf("#Valid cells                   : %i/%i\n#\n",NcellValid,Ncell);
   printf("cellinfo: NUM CellArea ClutArea AvgDbz AvgStdev CV MaxVal MaxIII MaxJJJ Dropped\n");
   for (n=0 ; n<Ncell ; n++) {
      if (c[n].area==0) continue;
      printf("cellinfo: %3d %8.1f %8.1f %6.2f %6.2f %6.2f %6.2f %6d %6d %6d\n",
         n+2,c[n].area,c[n].clutterarea,c[n].dbz,c[n].tex,c[n].cv,c[n].max,c[n].imax,c[n].jmax,c[n].drop);
   }
}
return NcellValid;
} //analysecells

/******************************************************************************/
/*This function updates the cellmap by dropping cells and reindexing the map  */
/*Leaving index 0 unused, which will be used for assigning cell fringes       */
/******************************************************************************/
int updatemap(int *cellmap,CELLPROP *c,int Ncell,int Npoints,int area)
{
int ij,n,NcellValid=Ncell;
for (ij=0 ; ij<Npoints ; ij++) {
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
}

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
	 if(cellmap[ii+jj*nrang]<1) edge=1; //STRICTLY THIS SHOULD BE edge<=1, BUT THEN TAKES MUCH MUCH LONGER
      }                                     //NOW ONLY CELL PIXELS WITHOUT ANY BORDERING FRINGE ARE 'FRINGED' (improvement welcome)
      if(!edge) continue;
      //search a limited block around pixel (i,j) only
      tmp=(XYMAX(0,i-rblock)*rscale*ascale*DEG2RAD);
      if (tmp<fringe/nazim) ablock=nazim;
      else ablock=ROUND(fringe/tmp);
      for (k=0 ; k<(2*rblock+1)*(2*ablock+1) ; k++) {
         ii=(i-rblock+k%(2*rblock+1));
         jj=(nazim+(j-ablock+k/(2*rblock+1)))%nazim; // periodic boundary condition azimuth
	 if (ii<0||ii>=nrang) continue;
	 //if not within range or already in cellmap or already a fringe, do nothing
         if (dist(i,j,ii,jj,rscale,ascale)>fringe || cellmap[ii+jj*nrang]>=1) continue;
	 //include pixel (ii,jj) in fringe
         cellmap[ii+jj*nrang]=1; //assign index 1 to fringes
      }
   }
} //for
return;
} //fringecells

/******************************************************************************/
/*This function reads radar scan data from a KNMI HDF5 file. The scan number  */
/*and the type (Z,U,V,W) should be selected. The metadata and scandata are    */
/*returned.                                                                   */
/******************************************************************************/
int read_h5_scan(hid_t file,int iscan,char type,SCANMETA *meta,unsigned char *data[])
{
char object[LSTR],string[LSTR];
float latlon[2];

/*Reading of metadata from radar group in file.*/
hid_t result,objectid;
meta->heig=0;
H5LTget_attribute_float(file,"/radar1","radar_height",&meta->heig);
meta->heig*=1e-3;
H5LTget_attribute_float(file,"/radar1","radar_location",latlon);
meta->lat=latlon[1];
meta->lon=latlon[0];

/*Reading of metadata from scan group in file.*/
sprintf(object,"/scan%d",iscan);
H5LTget_attribute_string(file,object,"scan_datetime",string);
string2datetime(string,&meta->date,&meta->time);
H5LTget_attribute_float(file,object,"scan_elevation",&meta->elev);
H5LTget_attribute_int(file,object,"scan_number_range",&meta->nrang);
if ((objectid=H5Gopen(file,object,H5P_DEFAULT))>=0){
   if (H5LTfind_attribute(objectid,"scan_number_range")) H5LTget_attribute_int(file,object,"scan_number_range",&meta->nrang);
   if (type=='Z' && H5LTfind_attribute(objectid,"scan_Z_number_range")) H5LTget_attribute_int(file,object,"scan_Z_number_range",&meta->nrang);
   if (type=='U' && H5LTfind_attribute(objectid,"scan_uZ_number_range")) H5LTget_attribute_int(file,object,"scan_uZ_number_range",&meta->nrang);
   if (type=='V' && H5LTfind_attribute(objectid,"scan_V_number_range")) H5LTget_attribute_int(file,object,"scan_V_number_range",&meta->nrang);
   H5Gclose(objectid);
}
H5LTget_attribute_int(file,object,"scan_number_azim",&meta->nazim);
H5LTget_attribute_float(file,object,"scan_range_bin",&meta->rscale);
H5LTget_attribute_float(file,object,"scan_azim_bin",&meta->ascale);
H5LTget_attribute_int(file,object,"scan_start_azim",&meta->azim0);
H5LTget_attribute_float(file,object,"scan_low_PRF",&meta->PRFl);
H5LTget_attribute_float(file,object,"scan_high_PRF",&meta->PRFh);
H5LTget_attribute_float(file,object,"scan_pulse_width",&meta->pulse);
H5LTget_attribute_float(file,object,"scan_radar_constant",&meta->radcnst);
H5LTget_attribute_float(file,object,"scan_TX_power_nom",&meta->txnom);
H5LTget_attribute_float(file,object,"scan_antenna_velocity",&meta->antvel);
/*Reading of metadata from scan/calibration subgroup in file.*/

sprintf(object,"/scan%d/calibration",iscan);
if (type=='Z') H5LTget_attribute_string(file,object,"calibration_Z_formulas",string);
if (type=='U') H5LTget_attribute_string(file,object,"calibration_uZ_formulas",string);
if (type=='V') H5LTget_attribute_string(file,object,"calibration_V_formulas",string);
if (type=='W') H5LTget_attribute_string(file,object,"calibration_W_formulas",string);
if (type=='M') H5LTget_attribute_string(file,object,"calibration_clutter_formulas",string);
if (type=='R') H5LTget_attribute_string(file,object,"calibration_Rho_formulas",string);
if (type=='D') H5LTget_attribute_string(file,object,"calibration_Zdr_formulas",string);
sscanf(string,"GEO=%f*PV+%f",&meta->zscale,&meta->zoffset);
result=H5LTget_attribute_int(file,object,"calibration_missing_data",&meta->missing);
if (result<0) meta->missing=0;

/*Reading of scan data.*/

*data=(unsigned char *)malloc(meta->nrang*meta->nazim*sizeof(unsigned char));
if (*data==NULL) {
   printf("Requested memory could not be allocated!\n");
   return 0;
}
if (type=='Z') sprintf(object,"/scan%d/scan_Z_data",iscan);
if (type=='U') sprintf(object,"/scan%d/scan_uZ_data",iscan);
if (type=='V') sprintf(object,"/scan%d/scan_V_data",iscan);
if (type=='W') sprintf(object,"/scan%d/scan_W_data",iscan);
if (type=='M') sprintf(object,"/scan%d/scan_clutter_data",iscan);
if (type=='R') sprintf(object,"/scan%d/scan_Rho_data",iscan);
if (type=='D') sprintf(object,"/scan%d/scan_Zdr_data",iscan);
result=H5LTread_dataset(file,object, H5T_NATIVE_UCHAR,*data);
/*Returning.*/

return !result;
}

/******************************************************************************/
/*This function converts a string according to the KNMI HDF5 specification to */
/*a date (yyyymmdd) and time (hhmmss).                                        */
/******************************************************************************/
void string2datetime(char *string,int *date,int *time)
{
char months[13][4]={"XXX","JAN","FEB","MAR","APR","MAY","JUN","JUL","AUG",
						"SEP","OCT","NOV","DEC"};
char month[4];
int yy=0,mon=0,dd=0,hh=0,mm=0,ss=0,m;

/*Extraction of year, month, day, etc from string.*/

sscanf(string,"%d-%[^-]-%d;%d:%d:%d",&dd,month,&yy,&hh,&mm,&ss);
for (m=1 ; m<=12 ; m++) {
   if (strncmp(month,months[m],3)==0) mon=m;
}

/*Returning date and time in integer format.*/

*date=10000*yy+100*mon+dd;
*time=10000*hh+100*mm+ss;
}

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
int azimuth_gap(float x[],int Ndx,int Npnt,int ngapmin)
{
int gap=0,Nsector[NGAPBIN],n,m;

/*Initialize histogram.*/

for (m=0 ; m<NGAPBIN ; m++) Nsector[m]=0;

/*Collect histogram.*/

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
/*Local HDF5 handling functions.                                              */
/******************************************************************************/

/******************************************************************************/
/* Function: H5LTmake_dataset                                                 */
/* Purpose: Creates and writes a dataset of a type tid                        */
/* Return: Success: 0, Failure: -1                                            */
/* Programmer: Pedro Vicente, pvn@ncsa.uiuc.edu                               */
/* Date: March 19, 2001                                                       */
/* Comments:                                                                  */
/* Modifications: Datasets are compressed  (Iwan Holleman, KNMI)              */
/******************************************************************************/

herr_t H5LTmake_dataset_zip(hid_t loc_id,char *dset_name,int rank,hsize_t *dims,hid_t tid,void *data) 
{
#define H5ZIPLEVEL (6)
hid_t did=-1,sid=-1,pid=-1;

/*Create the data space for the dataset.*/

if ((sid=H5Screate_simple(rank,dims,NULL))<0) return -1;

/*Create the property list for zipped datasets.*/

pid=H5Pcreate(H5P_DATASET_CREATE);
H5Pset_chunk(pid,rank,dims);
H5Pset_deflate(pid,H5ZIPLEVEL);

/*Create the dataset.*/

//if((did=H5Dcreate(loc_id,dset_name,tid,sid,H5P_DEFAULT,H5P_DEFAULT,H5P_DEFAULT))<0) goto out;
if((did=H5Dcreate(loc_id,dset_name,tid,sid,H5P_DEFAULT,pid,H5P_DEFAULT))<0) goto out;

/*Write the dataset only if there is data to write*/

if (data) {
	   if (H5Dwrite(did,tid,H5S_ALL,H5S_ALL,H5P_DEFAULT,data)<0) goto out;
}

/*End access to the dataset and release resources used by it.*/
	 
if (H5Dclose(did)<0) return -1;

/*Terminate access to the data space. */
	 
if (H5Sclose(sid)<0) return -1;

/*End access to the property list.*/
	 
if (H5Pclose(pid)<0) return -1;

return 0;

out:
   H5Dclose(did);
   H5Sclose(sid);
   H5Sclose(pid);
   return -1;
}

/******************************************************************************/
/*This function can be used to write a KNMI HDF5 image group to file.         */
/******************************************************************************/
void write_image_group(hid_t file,int num,char *prodname,char *geopar,
		unsigned char *image,int ncols,int nrows,int missing,
		int nodata,float offset,float scale,float elev)
{
char object[LSTR],string[LSTR];
int imin=255,imax=0,size,one=1,ij;
float fmin,fmax;
hsize_t dims[2];

/*Image group.*/

sprintf(object,"/image%d",num);
H5Gcreate(file,object,H5P_DEFAULT,H5P_DEFAULT,H5P_DEFAULT);
H5LTset_attribute_string(file,object,"image_product_name",prodname);
H5LTset_attribute_string(file,object,"image_geo_parameter",geopar);
size=ncols*nrows;
H5LTset_attribute_int(file,object,"image_size",&size,1);
H5LTset_attribute_int(file,object,"image_bytes_per_pixel",&one,1);
H5LTset_attribute_float(file,object,"image_elevation",&elev,1);

/*Writing 8-bit zipped image.*/

dims[0]=nrows;
dims[1]=ncols;
sprintf(object,"/image%d/image_data",num);
H5LTmake_dataset_zip(file,object,2,dims,H5T_NATIVE_UCHAR,image);
H5LTset_attribute_string(file,object,"CLASS","IMAGE");
H5LTset_attribute_string(file,object,"IMAGE_VERSION","1.2");
H5LTset_attribute_string(file,object,"DISPLAY_ORIGIN","UL");
 
/*Attach color palette */
H5Adelete_by_name(file, object, "PALETTE",H5P_DEFAULT);
switch (num) { 
  case 1:
    H5IMlink_palette(file, object, "/visualisation1/color_palette");
    break;
  case 2:
    H5IMlink_palette(file, object, "/visualisation2/color_palette");
    break;
  default:
    H5IMlink_palette(file, object, "/visualisation1/color_palette");
} //switch

/*Calibration subgroup.*/

sprintf(object,"/image%d/calibration",num);
H5Gcreate(file,object,H5P_DEFAULT,H5P_DEFAULT,H5P_DEFAULT);
H5LTset_attribute_int(file,object,"calibration_missing_data",&missing,1);
H5LTset_attribute_int(file,object,"calibration_out_of_image",&nodata,1);
sprintf(string,"GEO = %g * PV + %g",scale,offset);
H5LTset_attribute_string(file,object,"calibration_formulas",string);
H5LTset_attribute_string(file,object,"calibration_flag","Y");

/*Statistics subgroup.*/

for (ij=0 ; ij<ncols*nrows ; ij++) {
   if (image[ij]==nodata) continue;
   if (image[ij]>imax) imax=image[ij];
   if (image[ij]<imin) imin=image[ij];
}
fmin=scale*imin+offset;
fmax=scale*imax+offset;
sprintf(object,"/image%d/statistics",num);
H5Gcreate(file,object,H5P_DEFAULT,H5P_DEFAULT,H5P_DEFAULT);
H5LTset_attribute_float(file,object,"stat_min_value",&fmin,1);
H5LTset_attribute_float(file,object,"stat_max_value",&fmax,1);

/*End of function.*/

return;
}

/******************************************************************************/
/*This functions replaces a section in the KNMI HDF5 groupname string.        */
/*The groupname string consists of 4 or 5 sections separated by a `_` and     */
/*numbered from 1 to 4/5.                                                     */
/******************************************************************************/
void modify_groupname(char *groupname,int isec,char *section)
{
char tmpstr[128],*strptr;
int nsec=0;

/*Make copy of groupname string.*/

strcpy(tmpstr,groupname);

/*Scanning sections in groupname separated by a `_` .*/

strptr=strtok(tmpstr,"_");
if (nsec==isec-1) strcpy(groupname,section);
else strcpy(groupname,strptr);
nsec++;
while ((strptr=strtok(NULL,"_"))!=NULL) {
   strcat(groupname,"_");
   if (nsec==isec-1) strcat(groupname,section);
   else strcat(groupname,strptr);
   nsec++;
}

/*When number of sections is too small to insert the section.*/

if (isec>nsec) strcpy(groupname,section);
}

/******************************************************************************/
/*This function converts a KNMI product group name string to a product name   */
/*string. The product name string consists of four sections separated by a    */
/*`_` and the image name of five sections.                                    */
/******************************************************************************/
void group2productname(char *groupname,char *sec4,char *prodname)
{
char sec1[32],sec2[32],sec3[32],sec5[32];

/*Separate sections in groupname.*/

if (sscanf(groupname,"%[^_]_%[^_]_%[^_]_%[^_]",sec1,sec2,sec3,sec5)!=4) return;

/*Printing the 5 sections to the product name string.*/

sprintf(prodname,"%s_%s_%s_%s_%s",sec1,sec2,sec3,sec4,sec5);
}


