
package nl.esciencecenter.ncradar;

import java.io.IOException;

public class BirdDensityProfileCalculator extends JNIMethodsVol2Bird {

    private RadarScanJava RadarScanReflectivity;
    private RadarScanJava RadarScanRadialVelocity;
    

    public BirdDensityProfileCalculator(RadarScanJava RadarScanReflectivity, RadarScanJava RadarScanRadialVelocity) throws IOException {

        this.RadarScanReflectivity = RadarScanReflectivity;
        this.RadarScanRadialVelocity = RadarScanRadialVelocity;

    }
    
    public static void main(String[] args) throws IOException{
        
        RadarScanJava rsz = new RadarScanJava("/home/wbouten/tmp","T_PAGZ60_C_OKPR_20110815000447.hdf",3);
        RadarScanJava rsv = new RadarScanJava("/home/wbouten/tmp","T_PAHZ60_C_OKPR_20110815000447.hdf",3);

        BirdDensityProfileCalculator birdprofile = new BirdDensityProfileCalculator(rsz,rsv);
        
        
        
        System.out.println("I have a BirdDensityProfileCalculator object.");
       
    }
    



}
