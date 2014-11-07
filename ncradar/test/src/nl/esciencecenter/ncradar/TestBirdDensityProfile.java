package nl.esciencecenter.ncradar;

import org.junit.Before;
import org.junit.Test;


public class TestBirdDensityProfile {

    private RadarScanJava reflectivity;
    private RadarScanJava radialVelocity;
    private int iScan;
    private BirdDensityProfileJava birdDensityProfileJava;
    private int nAzim;
    private int nRang;
    private ParameterValues parameterValues;



    @Before
    public void setUp() throws Exception {

        iScan = 3;

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

        nAzim = birdDensityProfileJava.getNumberOfAzimuthBins();
        nRang = birdDensityProfileJava.getNumberOfRangeBins();

        System.out.println("I have a BirdDensityProfile object.");

        this.parameterValues = new ParameterValues();

    }



    @Test
    public void testBirdDensityProfile() throws Exception {

        int missing = 255;
        int nRangNeighborhood = this.parameterValues.getNTEXBINRANG();
        int nAzimNeighborhood = this.parameterValues.getNTEXBINAZIM();
        int nCountMin = this.parameterValues.getNTEXMIN();
        float texOffset = 0;
        float texScale = (float) this.parameterValues.getSTDEVSCALE();
        int vradMissing = missing;

        birdDensityProfileJava.calcTexture(nRangNeighborhood, nAzimNeighborhood, nCountMin,
                texOffset, texScale, vradMissing);

        int nGlobal = nAzim * nRang;
        int[] zeros = new int[nGlobal];
        int[] dbzImage = birdDensityProfileJava.getReflectivityRaw();
        int[] cellImage = zeros.clone();
        int dbzMissing = missing;
        int dbznAzim = nAzim;
        int dbznRang = nRang;
        float dbzOffset = (float) this.reflectivity.getDataOffset();
        float dbzScale = (float) this.reflectivity.getDataScale();
        // FIXME dbzRangeScale seems to be stored as a number in [m], Adriaan
        // assumes [km]?
        float dbzRangeScale = (float) this.reflectivity.getRangeScale() / 1000;
        float dbzThresMin = (float) this.parameterValues.getDBZRAIN() * dbzScale + dbzOffset;
        // FIXME plus 5? magic number
        int rCellMax = (int) (this.parameterValues.getRANGMAX() + 5);
        int sign = -1;

        int nCells = birdDensityProfileJava.findCells(dbzImage, cellImage,
                dbzMissing, dbznAzim, dbznRang, dbzOffset, dbzScale, dbzRangeScale, dbzThresMin,
                rCellMax, sign);

        int[] vradImage = birdDensityProfileJava.getRadialVelocityRaw();
        int[] texImage = birdDensityProfileJava.getTexture();
        int[] clutterImage = zeros.clone();
        float dbzElev = (float) this.reflectivity.getElevationAngle();
        float vradScale = (float) this.radialVelocity.getDataScale();
        float vradOffset = (float) this.radialVelocity.getDataOffset();
        float clutterScale = 1.0f; // FIXME
        float clutterOffset = 0.0f; // FIXME
        int areaMin = 0; // FIXME AREACELL?
        float cellDbzMin = 0; // FIXME DBZCELL?
        float cellStdDevMax = 0; // FIXME STDEVCELL?
        float cellClutterFraction = 0; // FIXME CLUTPERCCELL * 100 ?
        float vradMinValue = (float) parameterValues.getVRADMIN();
        float dbzClutterMin = 0; // FIXME DBZCLUTTER?
        int cmFlag = 0; // FIXME
        int verbose = 0; // FIXME

        int nCellsValid = birdDensityProfileJava.analyzeCells(dbzImage, vradImage,
                texImage, clutterImage, cellImage,
                dbznRang, dbznAzim, dbzElev, dbzScale, dbzOffset,
                vradScale, vradOffset, clutterScale, clutterOffset,
                texScale, texOffset, nCells, areaMin, cellDbzMin, cellStdDevMax,
                cellClutterFraction, vradMinValue, dbzClutterMin, cmFlag,
                verbose);

        final float DEG2RAD = (float) (2 * Math.PI) / 360;
        float azimuthScale = (360.0f / nAzim) * DEG2RAD;
        // FIXME again, what are the units?
        float rangeScale = (float) this.radialVelocity.getRangeScale() / 1000;
        float fringeDist = (float) this.parameterValues.getEMASKMAX();

        birdDensityProfileJava.fringeCells(cellImage, nRang, nAzim, azimuthScale,
                rangeScale, fringeDist);

        float dbzHeig = (float) this.reflectivity.getRadarPositionHeight();
        float dbzAzimScale = (360.0f / nAzim) * DEG2RAD;
        int rawReflMissing = missing;
        int[] rawReflImage = zeros.clone();
        // FIXME is zdata the profile information? (Then it should have multiple
        // pseudo-columns)
        float[] zdata = new float[1000];
        float rangeMin = (float) this.parameterValues.getRANGMIN();
        float rangeMax = (float) this.parameterValues.getRANGMAX();
        float HLAYER = (float) this.parameterValues.getHLAYER();
        float XOFFSET = 0.0f; // FIXME unclear what this does
        float XSCALE = 1.0f; // FIXME unclear what this does
        float XMEAN = 0.0f; // FIXME unclear what this does
        float height = 600.0f;
        float azimMin = (float) this.parameterValues.getAZIMMIN();
        float azimMax = (float) this.parameterValues.getAZIMMAX();
        float vradMin = (float) this.parameterValues.getVRADMIN();
        float dbzClutter = (float) this.parameterValues.getDBZCLUTTER();
        float dbzMin = (float) this.parameterValues.getDBZMIN();
        float dBZx = (float) this.parameterValues.getDBZMAX();
        float DBZNOISE = (float) this.parameterValues.getDBZNOISE();
        int NGAPMIN = this.parameterValues.getNGAPMIN();
        int NGAPBIN = this.parameterValues.getNGAPBIN();
        int NDBZMIN = this.parameterValues.getNDBZMIN();

        // TODO this is how far I got with mapping this.parameterValues to the
        // variables used here

        int layer = 0;
        int np = 0;
        int nPointsPtr = 0;
        int nPointsAllPtr = 0;
        int nPointsClutterPtr = 0;
        int nPointsRainPtr = 0;
        int nPointsRainNoFringePtr = 0;
        int clutterFlagInt = 1;
        int rawReflFlagInt = 0;
        int xflagInt = 0;

        // FIXME it seems zdata should be bigger, probably NDATA * llayer or
        // something to accommodate all the stuff that classify() wants to put
        // in it.

        birdDensityProfileJava.classify(dbznRang, dbznAzim, dbzRangeScale,
                dbzElev, dbzHeig, dbzScale, dbzOffset,
                dbzAzimScale, dbzMissing, vradScale, vradOffset, vradMissing,
                rawReflMissing,
                clutterScale, clutterOffset, cellImage, dbzImage, vradImage,
                rawReflImage, clutterImage, zdata, rangeMin, rangeMax,
                HLAYER, XOFFSET, XSCALE, XMEAN, height, azimMin, azimMax, vradMin,
                dbzClutter,
                dbzMin, dBZx, DBZNOISE, NGAPMIN, NGAPBIN, NDBZMIN, layer, np,
                nPointsPtr,
                nPointsAllPtr, nPointsClutterPtr, nPointsRainPtr,
                nPointsRainNoFringePtr,
                clutterFlagInt, rawReflFlagInt, xflagInt);

        System.err.print("Done\n");
        // TODO finish this stuff right here

    }
}
