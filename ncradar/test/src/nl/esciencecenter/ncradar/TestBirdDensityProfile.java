package nl.esciencecenter.ncradar;

import static org.junit.Assert.fail;
import org.junit.Before;
import org.junit.Test;


public class TestBirdDensityProfile {

    private RadarScanJava radarScanReflectivity;
    private RadarScanJava radarScanRadialVelocity;
    private int iScan;
    private BirdDensityProfile birdProfile;



    @Before
    public void setUp() throws Exception {

        iScan = 3;
        radarScanReflectivity = new RadarScanJava("/home/wbouten/tmp",
                "T_PAGZ60_C_OKPR_20110815000447.hdf", iScan);
        radarScanRadialVelocity = new RadarScanJava("/home/wbouten/tmp",
                "T_PAHZ60_C_OKPR_20110815000447.hdf", iScan);

        birdProfile = new BirdDensityProfile(radarScanReflectivity,
                radarScanRadialVelocity);

        System.out.println("I have a BirdDensityProfile object.");

    }



    @Test
    public void testBirdDensityProfile() {

        byte nRangNeighborhood = 3;
        byte nAzimNeighborhood = 3;
        byte nCountMin = 0;
        byte texType = 1;
        float texOffset = 0;
        float texScale = 1;

        birdProfile.calcTexture(nRangNeighborhood, nAzimNeighborhood,
                nCountMin, texType, texOffset, texScale);

        birdProfile.findCells();

        fail("Not yet complete");

    }
}
