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
/*Definition of standard parameters.                                          */
/******************************************************************************/

#define DEG2RAD    (0.017453293)  /*Degrees to radians.*/

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


#define NEIGHBOURS  (5)      /*Minimum number of directly neighouring pixels  */
                             /*with dBZ>DBZRAIN [1-8]                         */
#define NDATA       (3)      /*Data dimension height layers.                  */
#define PI          (3.14159265358979323846)
#define TRUE        (1)
#define FALSE       (0)


///******************************************************************************/
///*Structure for containing SCAN metadata:                                     */
///******************************************************************************/

struct scanmeta {
    float heig;              // Height of radar antenna in km.
    float elev;              // Elevation of scan in deg.
    int nRang;               // Number of range bins in scan.
    int nAzim;               // Number of azimuth rays in scan.
    float rangeScale;        // Size of range bins in scan in km.
    float azimScale;         // Size of azimuth steps in scan in deg.
    float valueOffset;       // Offset value of quantity contained by scan.
    float valueScale;        // Scale of value of quantity contained by scan.
    int missing;             // Missing value of quantity contained by scan.
};

struct cellprop {
    int iRangOfMax;
    int iAzimOfMax;
    float dbzAvg;
    float texAvg;
    float cv;
    int area;
    int clutterArea;
    float dbzMax;
    int index;
    int drop;
};

typedef struct scanmeta SCANMETA;
typedef struct cellprop CELLPROP;




// *****************************************************************************
// Function prototypes
// *****************************************************************************

int analyzeCells(const unsigned char *dbzImage, const unsigned char *vradImage,
        const unsigned char *texImage, const unsigned char *clutterImage, int *cellImage,
        const SCANMETA *dbzMeta, const SCANMETA *vradMeta, const SCANMETA *texMeta, const SCANMETA *clutterMeta,
        const int nCells, const int areaMin, const float cellDbzMin, const float cellStdDevMax, const float cellClutterFraction,
        const float vradMinValue, const float clutterValueMax, const unsigned char cmFlag,
        const unsigned char verbose);

float calcDist(int range1, int azim1,int range2,int azim2,float rscale,float ascale);

void calcTexture(unsigned char *texImage, const unsigned char *vradImage, const unsigned char *dbzImage,
        const SCANMETA *texMeta, const SCANMETA *vradMeta, const SCANMETA *dbzMeta,
        const unsigned char nRangNeighborhood, const unsigned char nAzimNeighborhood,
        const unsigned char nCountMin);

int getListOfSelectedGates(SCANMETA vradMeta, unsigned char *vradImage, float *points, float *yObs,
        int *c, int *cellmap,
        float rangeMin, float rangeMax, float layerThickness, float heightInputPar,
        float vradMin, int iData, int layer, int nPoints);

void classify(SCANMETA dbzMeta, SCANMETA vradMeta, SCANMETA uzmeta,
        SCANMETA clutterMeta, int *cellImage,
        unsigned char *dbzImage, unsigned char *vradImage,
        unsigned char *uzscan, unsigned char *clutterImage,
        float *zdata,
        float rangeMin, float rangeMax, float layerThickness, float XOFFSET,
        float XSCALE, float XMEAN, float height,
        float azimMin, float azimMax, float vradMin, float dbzClutter, float dbzMin,
        float dBZx, float DBZNOISE, int NGAPMIN, int NDBZMIN,
        int layer, int *np, int *nPointsPtr, int *nPointsAllPtr, int *nPointsClutterPtr,
        int *nPointsRainPtr, int *nPointsRainNoFringePtr,
        unsigned char clutterFlag, unsigned char uzflag, unsigned char xflag);

int findCells(const unsigned char *dbzImage,
              int *cellImage,
              const SCANMETA *dbzMeta,
              const float dbzThresMin,
              const float rCellMax);

int findNearbyGateIndex(const int nAzimParent, const int nRangParent, const int iParent,
                        const int nAzimChild,  const int nRangChild,  const int iChild);

void fringeCells(int *cellImage,int nRang, int nAzim, float aScale, float rScale, float fringe);

void sortCells(CELLPROP *cellProp,int nCells);

int updateMap(int *cellImage, const int nGlobal, CELLPROP *cellProp, const int nCells, const int minCellArea);





