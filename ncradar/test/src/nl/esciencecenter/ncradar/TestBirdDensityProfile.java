package nl.esciencecenter.ncradar;

import org.junit.Before;
import org.junit.Test;


public class TestBirdDensityProfile {

    private RadarScanJava reflectivity;
    private RadarScanJava radialVelocity;
    private int iScan;
    private BirdDensityProfile birdProfile;



    @Before
    public void setUp() throws Exception {

        iScan = 3;
        reflectivity = new RadarScanJava("/home/wbouten/tmp", "T_PAGZ60_C_OKPR_20110815000447.hdf", iScan);
        radialVelocity = new RadarScanJava("/home/wbouten/tmp", "T_PAHZ60_C_OKPR_20110815000447.hdf", iScan);

        birdProfile = new BirdDensityProfile(reflectivity, radialVelocity);

        System.out.println("I have a BirdDensityProfile object.");

    }



    @Test
    public void testBirdDensityProfile() {

        int nRangNeighborhood = 3;
        int nAzimNeighborhood = 3;
        int nCountMin = 0;
        float texOffset = 0;
        float texScale = 1;
        int vradMissing = 255;

        birdProfile.calcTexture(nRangNeighborhood, nAzimNeighborhood,
                nCountMin, texOffset, texScale, vradMissing);

        // birdProfile.findCells();
        // birdProfile.analyseCells();
        // birdProfile.fringeCells();
        // birdProfile.classify();

        // TODO finish this stuff right here

    }

}
