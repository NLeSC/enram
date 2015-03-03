//
// Copyright 2013 Netherlands eScience Center
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//



// ****************************************************************************
// Definition of standard parameters.
// ****************************************************************************

#define DEG2RAD    (0.017453293)  // Degrees to radians.
#define RAD2DEG    (57.29578)     // Radians to degrees.

// ****************************************************************************
// Definition of general macros:
// ****************************************************************************

#define XABS(x)    (((x)<0)?(-(x)):(x))
#define ROUND(x)   (((x)>0)?(int)((x)+0.5):(int)((x)-0.5))
#define SQUARE(x)  ((x)*(x))

// ****************************************************************************
// Defined (default) parameters.
// ****************************************************************************


#ifndef PI
#define PI          (3.14159265358979323846)
#endif

#ifndef TRUE
#define TRUE        1
#endif

#ifndef FALSE
#define FALSE       0
#endif



// ****************************************************************************
//  Structure for containing SCAN metadata:
// ****************************************************************************


struct cellprop {
    int iRangOfMax;
    int iAzimOfMax;
    float dbzAvg;
    float texAvg;
    float cv;
    int nGates;
    int nGatesClutter;
    float dbzMax;
    int index;
    int drop;
};

struct scanmeta {
    float heig;              // Height of radar antenna in km.
    float elev;              // Elevation of scan in deg.
    int nRang;               // Number of range bins in scan.
    int nAzim;               // Number of azimuth rays in scan.
    float rangeScale;        // Size of range bins in scan in km.
    float azimScale;         // Size of azimuth steps in scan in deg.
    float valueOffset;       // Offset value of quantity contained by scan.
    float valueScale;        // Scale of value of quantity contained by scan.
    unsigned char missing;   // Missing value of quantity contained by scan.
};


typedef struct cellprop CELLPROP;
typedef struct scanmeta SCANMETA;

// *****************************************************************************
// Function prototypes
// *****************************************************************************

static int analyzeCells(const unsigned char *dbzImage, const unsigned char *vradImage,
                        const unsigned char *texImage, const unsigned char *clutterImage, int *cellImage,
                        const SCANMETA *dbzMeta, const SCANMETA *vradMeta, const SCANMETA *texMeta, const SCANMETA *clutterMeta,
                        const int nCells, const int useStaticClutterData);

static float calcDist(const int range1, const int azim1, const int range2, const int azim2, const float rscale, const float ascale);

static void calcTexture(unsigned char *texImage, const unsigned char *vradImage, const unsigned char *dbzImage,
                        const SCANMETA *texMeta, const SCANMETA *vradMeta, const SCANMETA *dbzMeta);

static void classifyGatesSimple(void);

static int constructorInt(SCANMETA* meta, int* image, PolarScan_t* scan, const int nGlobal, const int initValue);

static int constructorUChar(SCANMETA* meta, unsigned char* image, PolarScan_t* scan, const int nGlobal, const unsigned char initValue);

static void constructPointsArray(PolarVolume_t* volume);

static int detNumberOfGates(const int iLayer, const float rangeScale, const float elevAngle,
                            const int nRang, const int nAzim, const float radarHeight);

static int detSvdfitArraySize(PolarVolume_t* volume);

static int findWeatherCells(const unsigned char *dbzImage, int *cellImage, const SCANMETA *dbzMeta);

static int findNearbyGateIndex(const int nAzimParent, const int nRangParent, const int iParent,
                               const int nAzimChild,  const int nRangChild,  const int iChild);

static void fringeCells(int *cellImage,int nRang, int nAzim, float aScale, float rScale);

static int getListOfSelectedGates(const SCANMETA* vradMeta, const unsigned char *vradImage,
                                  const SCANMETA* dbzMeta, const unsigned char *dbzImage,
                                  const int *cellImage,
                                  const float altitudeMin, const float altitudeMax,
                                  float* points, int iPoint);

static int hasAzimuthGap(const float *points, const int nPoints);

static int includeGate(const int iProfileType, const int gateCode);

static int mapDataFromRave(PolarScan_t* scan, SCANMETA *meta, 
                           unsigned char *values, char *paramStr);

static void mapDataToRave(void);

static void printGateCode(char* flags, const int gateCode);

static int printMeta(const SCANMETA* meta, const char* varName);

static void sortCells(CELLPROP *cellProp, const int nCells);

static void updateFlagFieldsInPointsArray(const float* yObs, const float* yFitted, const int* includedIndex, 
                                          const int nPointsIncluded, float* points);
static int updateMap(int *cellImage, const int nGlobal, CELLPROP *cellProp, const int nCells);

void vol2birdCalcProfiles();

void vol2birdPrintImageInt(const SCANMETA* meta, const int* imageInt);

void vol2birdPrintImageUChar(const SCANMETA* meta, const unsigned char* imageUChar);

void vol2birdPrintIndexArrays(void);

void vol2birdPrintOptions(void);

void vol2birdPrintPointsArray(void);

void vol2birdPrintProfile(void);
    
int vol2birdSetUp(PolarVolume_t* volume);

void vol2birdTearDown();
