package nl.esciencecenter.ncradar;

import org.junit.Before;
import org.junit.Test;


public class TestBirdDensityProfile {

    private RadarScanJava reflectivity;
    private RadarScanJava radialVelocity;
    private int iScan;
    private BirdDensityProfileJava birdDensityProfileJava;



    @Before
    public void setUp() throws Exception {

        iScan = 0;

        String userName = System.getProperty("user.name");
        String theDir;

        if (userName.equals("wbouten")) {
            theDir = System.getProperty("user.dir") + "/test/data";
        }
        else {
            theDir = System.getProperty("user.dir") + "/data";
        }

        reflectivity = new RadarScanJava(theDir, "T_PAGZ60_C_OKPR_20110815000447.hdf", iScan);
        radialVelocity = new RadarScanJava(theDir, "T_PAHZ60_C_OKPR_20110815000447.hdf", iScan);

        birdDensityProfileJava = new BirdDensityProfileJava(reflectivity, radialVelocity);

    }



    @Test
    public void testBirdDensityProfile() throws Exception {

        birdDensityProfileJava.calcTexture();

        int nCells = birdDensityProfileJava.findCells();

        int cluttermapFlag = 0;
        int verbosityFlag = 1;

        int nCellsValid = birdDensityProfileJava.analyzeCells(nCells, cluttermapFlag, verbosityFlag);

        birdDensityProfileJava.fringeCells();

        int iLayer = 4;
        float layerThickness = 200.0f;
        int iData = 0;
        
        birdDensityProfileJava.getListOfSelectedGates(iLayer, layerThickness, iData);
        

    }


}
