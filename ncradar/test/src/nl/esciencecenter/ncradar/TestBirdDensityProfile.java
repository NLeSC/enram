package nl.esciencecenter.ncradar;

import org.junit.Before;
import org.junit.Test;


public class TestBirdDensityProfile {

    private RadarScanJava reflectivity;
    private RadarScanJava radialVelocity;
    private int iScan;
    private BirdDensityProfileJava birdDensityProfileJava;
    private ParameterValues parameterValues;



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

        System.out.println("I have a BirdDensityProfile object.");

        this.parameterValues = new ParameterValues();

    }



    @Test
    public void testBirdDensityProfile() throws Exception {

        birdDensityProfileJava.calcTexture();

        int nCells = birdDensityProfileJava.findCells();

        int cluttermapFlag = 0; // FIXME
        int verbosityFlag = 0; // FIXME

        int nCellsValid = birdDensityProfileJava.analyzeCells(nCells, cluttermapFlag, verbosityFlag);

        final float DEG2RAD = (float) (2 * Math.PI) / 360;
//        float azimuthScale = (360.0f / nAzim) * DEG2RAD;
//        // FIXME again, what are the units?
//        float rangeScale = (float) this.radialVelocity.getRangeScale();
//        float fringeDist = (float) this.parameterValues.getEMASKMAX();
//
//        birdDensityProfileJava.fringeCells(cellImage, nRang, nAzim, azimuthScale,
//                rangeScale, fringeDist);
//
//        float dbzHeig = (float) this.reflectivity.getRadarPositionHeight();
//        float dbzAzimScale = (360.0f / nAzim) * DEG2RAD;
//        int rawReflMissing = missing;
//        int[] rawReflImage = zeros.clone();
//        // FIXME is zdata the profile information? (Then it should have multiple
//        // pseudo-columns)
//        float[] zdata = new float[1000];
//        float rangeMin = (float) this.parameterValues.getRANGMIN();
//        float rangeMax = (float) this.parameterValues.getRANGMAX();
//        float HLAYER = (float) this.parameterValues.getHLAYER();
//        float XOFFSET = 0.0f; // FIXME unclear what this does
//        float XSCALE = 1.0f; // FIXME unclear what this does
//        float XMEAN = 0.0f; // FIXME unclear what this does
//        float height = 600.0f;
//        float azimMin = (float) this.parameterValues.getAZIMMIN();
//        float azimMax = (float) this.parameterValues.getAZIMMAX();
//        float vradMin = (float) this.parameterValues.getVRADMIN();
//        float dbzClutter = (float) this.parameterValues.getDBZCLUTTER();
//        float dbzMin = (float) this.parameterValues.getDBZMIN();
//        float dBZx = (float) this.parameterValues.getDBZMAX();
//        float DBZNOISE = (float) this.parameterValues.getDBZNOISE();
//        int NGAPMIN = this.parameterValues.getNGAPMIN();
//        int NGAPBIN = this.parameterValues.getNGAPBIN();
//        int NDBZMIN = this.parameterValues.getNDBZMIN();
//
//        // TODO this is how far I got with mapping this.parameterValues to the
//        // variables used here
//
//        int layer = 0;
//        int np = 0;
//        int nPointsPtr = 0;
//        int nPointsAllPtr = 0;
//        int nPointsClutterPtr = 0;
//        int nPointsRainPtr = 0;
//        int nPointsRainNoFringePtr = 0;
//        int clutterFlagInt = 1;
//        int rawReflFlagInt = 0;
//        int xflagInt = 0;
//
//        // FIXME it seems zdata should be bigger, probably NDATA * llayer or
//        // something to accommodate all the stuff that classify() wants to put
//        // in it.
//
//        birdDensityProfileJava.classify(dbznRang, dbznAzim, dbzRangeScale,
//                dbzElev, dbzHeig, dbzScale, dbzOffset,
//                dbzAzimScale, dbzMissing, vradScale, vradOffset, vradMissing,
//                rawReflMissing,
//                clutterScale, clutterOffset, cellImage, dbzImage, vradImage,
//                rawReflImage, clutterImage, zdata, rangeMin, rangeMax,
//                HLAYER, XOFFSET, XSCALE, XMEAN, height, azimMin, azimMax, vradMin,
//                dbzClutter,
//                dbzMin, dBZx, DBZNOISE, NGAPMIN, NGAPBIN, NDBZMIN, layer, np,
//                nPointsPtr,
//                nPointsAllPtr, nPointsClutterPtr, nPointsRainPtr,
//                nPointsRainNoFringePtr,
//                clutterFlagInt, rawReflFlagInt, xflagInt);
//
//        System.err.print("Done\n");
//        // TODO finish this stuff right here

    }



    public ParameterValues getParameterValues() {

        return parameterValues;
    }

}
