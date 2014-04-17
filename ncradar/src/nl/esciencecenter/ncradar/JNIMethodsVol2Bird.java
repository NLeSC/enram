package nl.esciencecenter.ncradar;

public class JNIMethodsVol2Bird {

    static {
        System.loadLibrary("calctexture");
        System.loadLibrary("findcells");
        System.loadLibrary("sortcells");
        System.loadLibrary("dist");
        System.loadLibrary("fringecells");
        }
         
    // Declare native methods
    final native static int[] calcTexture(int tMissing, int tnAzim, int tnRange, double tOffset, double tScale,
                            int[] vImage, int vMissing, int vnAzim, int vnRange, double vOffset, double vScale,
                            int[] zImage, int zMissing, int znAzim, int znRange, double zOffset, double zScale,
                            int nRangLocal, int nAzimLocal, int nCountMin, int textype);
    
    final native static int[] findCells(int[] cellImage, 
            int[] texImage, int texnRang, int texnAzim, double texOffset, double texScale, int texMissing, double texThresMin, 
            int[] rhoImage, int rhonRang, int rhonAzim, double rhoOffset, double rhoScale, int rhoMissing, double rhoThresMin,
            int[] zdrImage, int zdrnRang, int zdrnAzim, double zdrOffset, double zdrScale, int zdrMissing, double zdrThresMin, 
            double dbzmin, double rcellmax, int sign, double texrScale);
    
    final native static void sortCells(int[] cellPropIRangOfMax,
                                       int[] cellPropIAzimOfMax, 
                                       double[] cellPropDbz, 
                                       double[] cellPropTex, 
                                       double[] cellPropCv, 
                                       double[] cellPropArea, 
                                       double[] cellPropClutterArea, 
                                       double[] cellPropMax, 
                                       int[] cellPropIndex, 
                                       int[] cellPropDrop, 
                                       int nCells, 
                                       int method);
    
    final native static float dist(int range1, 
                                   int azim1,
                                   int range2,
                                   int azim2,
                                   float rscale,
                                   float ascale);
    
    
    
    final native static void fringecells(int[] cellImage, 
                                         int nRang,
                                         int nAzim,
                                         float aScale,
                                         float rScale,
                                         float fringe);
    
}
