package nl.esciencecenter.ncradar;

public class JNIMethodsVol2Bird {

    static {
        System.loadLibrary("calctexture");
        }
         
    // Declare native methods
    final native static int[] calcTexture(int tMissing, int tnAzim, int tnRange, double tOffset, double tScale,
                     int[] vImage, int vMissing, int vnAzim, int vnRange, double vOffset, double vScale,
                     int[] zImage, int zMissing, int znAzim, int znRange, double zOffset, double zScale,
                     int nRangLocal, int nAzimLocal, int nCountMin, int textype);
	
}
