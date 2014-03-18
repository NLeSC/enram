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


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h> 
#include "hdf5.h"
#include "vol2bird_routines.h"

/******************************************************************************/
/*Definition of standard parameters.                                          */
/******************************************************************************/

#define DEG2RAD    (0.017453293)  /*Degrees to radians.*/
#define RAD2DEG    (57.29578)     /*Radians to degrees.*/
#define LSTR       (128)          /*Length of all strings used.*/
#define NSCANX     (64)           /*Maximum number of elevation scans.*/
#define RADIUS43   (8495.0)       /*Earth radius used for height calculations.*/

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

#define TEXCV       (1)
#define TEXSTDEV    (2)
#define NEIGHBOURS  (5)      /*Minimum number of directly neighouring pixels  */
		             /*with dBZ>DBZRAIN [1-8]                         */
#define NLAYER      (30)     /*Number of stacked height layers.               */
#define NDATA       (3)      /*Data dimension height layers.                  */

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
/*Basis functions for Singular Value Decomposition linear fitting.            */
/******************************************************************************/

int svd_vad1func(float x[],int Ndx,float afunc[],int Npar);
int svd_vad2func(float x[],int Ndx,float afunc[],int Npar);
int svd_vvp1func(float x[],int Ndx,float afunc[],int Npar);
int svd_vvp2func(float x[],int Ndx,float afunc[],int Npar);
int svd_vvp3func(float x[],int Ndx,float afunc[],int Npar);

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
		SCANMETA *texmeta,SCANMETA *vmeta,SCANMETA *zmeta,
		unsigned char ntexrang,unsigned char ntexazim,
		unsigned char ntexmin,unsigned char textype);
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
     unsigned char cmflag,unsigned char uzflag,unsigned char xflag);
