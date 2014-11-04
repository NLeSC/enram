package nl.esciencecenter.ncradar;



public class BirdDensityProfileMatlab {

    private final BirdDensityProfileJava birdDensityProfileJava;



    public BirdDensityProfileMatlab(RadarScanJava reflectivity, RadarScanJava radialVelocity) throws Exception {

        this.birdDensityProfileJava = new BirdDensityProfileJava(reflectivity, radialVelocity);

    }

}
