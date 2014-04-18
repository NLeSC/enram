package nl.esciencecenter.ncradar;

public class JNIMethodsVol2Bird {

    static {
        //System.loadLibrary("calctexture");
        System.loadLibrary("dist");
        System.loadLibrary("fringecells");
        System.loadLibrary("updatemap");
        System.loadLibrary("sortcells");
        System.loadLibrary("findcells");

        


        }
         
//    // Declare native methods
//    final native static int[] calcTexture(int tMissing, int tnAzim, int tnRange, double tOffset, double tScale,
//                            int[] vImage, int vMissing, int vnAzim, int vnRange, double vOffset, double vScale,
//                            int[] zImage, int zMissing, int znAzim, int znRange, double zOffset, double zScale,
//                            int nRangLocal, int nAzimLocal, int nCountMin, int textype);
//    
//    final native static int[] findCells(int[] cellImage, 
//            int[] texImage, int texnRang, int texnAzim, double texOffset, double texScale, int texMissing, double texThresMin, 
//            int[] rhoImage, int rhonRang, int rhonAzim, double rhoOffset, double rhoScale, int rhoMissing, double rhoThresMin,
//            int[] zdrImage, int zdrnRang, int zdrnAzim, double zdrOffset, double zdrScale, int zdrMissing, double zdrThresMin, 
//            double dbzmin, double rcellmax, int sign, double texrScale);
//
//    
    // new interface method starts here:
    
    final native static float dist(int range1, 
                                   int azim1,
                                   int range2,
                                   int azim2,
                                   float rscale,
                                   float ascale);

    
    
    final native static void fringeCells(int[] cellImage, 
                                         int nRang,
                                         int nAzim,
                                         float aScale,
                                         float rScale,
                                         float fringe);
    
    
    final native static int updateMap(int[] cellImage,
                                      int[] cellPropIRangOfMax,
                                      int[] cellPropIAzimOfMax,
                                      float[] cellPropDbzAvg,
                                      float[] cellPropTexAvg,
                                      float[] cellPropCv,
                                      float[] cellPropArea,
                                      float[] cellPropClutterArea,
                                      float[] cellPropDbzMax,
                                      int[] cellPropIndex,
                                      char[] cellPropDrop,
                                      int nCells,
                                      int nGlobal,
                                      int minCellArea);
    
    
    final native static void sortCells(int[] cellPropIRangOfMax,
                                       int[] cellPropIAzimOfMax, 
                                       float[] cellPropDbzAvg, 
                                       float[] cellPropTexAvg, 
                                       float[] cellPropCv, 
                                       float[] cellPropArea, 
                                       float[] cellPropClutterArea, 
                                       float[] cellPropDbzMax, 
                                       int[] cellPropIndex, 
                                       char[] cellPropDrop, 
                                       int nCells, 
                                       int method);
    
    final native static int findCells(int[] cellImage,
                                      char[] texImage,
                                      char[] rhoImage,
                                      char[] zdrImage,
                                      float dbzThresMin,
                                      int texMissing,
                                      int texnAzim,
                                      int texnRang,
                                      float texValueOffset,
                                      float texRangeScale,
                                      float texValueScale,
                                      float texThresMin,
                                      int rhoMissing,
                                      int rhonAzim,
                                      int rhonRang,
                                      float rhoValueOffset,
                                      float rhoValueScale,
                                      float rhoThresMin,
                                      int zdrMissing,
                                      int zdrnAzim,
                                      int zdrnRang,
                                      float zdrValueOffset,
                                      float zdrValueScale,
                                      float zdrThresMin,
                                      float rCellMax,
                                      char sign);
   
}