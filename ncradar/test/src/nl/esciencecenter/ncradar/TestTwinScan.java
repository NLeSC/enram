package nl.esciencecenter.ncradar;

import org.junit.Before;
import org.junit.Test;


public class TestTwinScan {

    private RadarScanJava reflectivity;
    private RadarScanJava radialVelocity;
    private ParameterValues parameterValues;
    private int iScan;
    private TwinScan twinScan;


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
        parameterValues = new ParameterValues();
        
        twinScan = new TwinScan(reflectivity, radialVelocity, parameterValues);

    }



    @Test
    public void testTwinScan() throws Exception {

        twinScan.calcTexture();

        int nCells = twinScan.findCells();

        int cluttermapFlag = 0;
        int verbosityFlag = 1;

        int nCellsValid = twinScan.analyzeCells(nCells, cluttermapFlag, verbosityFlag);

        twinScan.fringeCells();

        int iLayer = 4;
        int iData = 0;
        
        twinScan.getListOfSelectedGates(iLayer, iData);
        

    }


}
