package nl.esciencecenter.ncradar;

import java.io.IOException;


public class BirdDensityProfile extends JNIMethodsVol2Bird {

    private final RadarScanJava RadarScanReflectivity;
    private final RadarScanJava RadarScanRadialVelocity;



    // private byte[][] texture;

    public BirdDensityProfile(RadarScanJava RadarScanReflectivity,
            RadarScanJava RadarScanRadialVelocity) throws IOException {

        this.RadarScanReflectivity = RadarScanReflectivity;
        this.RadarScanRadialVelocity = RadarScanRadialVelocity;

    }

}
