
package nl.esciencecenter.ncradar;

import java.io.IOException;

public class BirdDensityProfileCalculator extends JNIMethodsVol2Bird {

    private RadarScanJava rsz;
    private RadarScanJava rsv;
    

    public BirdDensityProfileCalculator(RadarScanJava rsz, RadarScanJava rsv) throws IOException {

        this.rsz = rsz;
        this.rsv = rsv;

    }
    
    public static void main(String[] args) throws IOException{

        
        RadarScanJava rsz = new RadarScanJava("/home/wbouten/tmp","T_PAGZ60_C_OKPR_20110815000447.hdf",3);
        RadarScanJava rsv = new RadarScanJava("/home/wbouten/tmp","T_PAHZ60_C_OKPR_20110815000447.hdf",3);

        BirdDensityProfileCalculator birdprofile = new BirdDensityProfileCalculator(rsz,rsv);
        
        
        System.out.println("I have a BirdDensityProfileCalculator object.");
        
        
        
       
    }
    



}
