package nl.esciencecenter.ncradar;

public class JNIMethodsVol2Bird {

    static {
        System.loadLibrary("sayhello");
        System.loadLibrary("addintegers");
        System.loadLibrary("calctexture");
        }
         
    // Declare native methods
    final native void sayHello();
    final native int addIntegers(int a, int b);
    final native int[] calcTexture(int tMissing, int tnAzim, int tnRange, double tOffset, double tScale,
                     int[] vImage, int vMissing, int vnAzim, int vnRange, double vOffset, double vScale,
                     int[] zImage, int zMissing, int znAzim, int znRange, double zOffset, double zScale,
                     int nRangLocal, int nAzimLocal, int nCountMin, int textype);
	
}
