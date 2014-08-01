package nl.esciencecenter.ncradar;

import org.junit.Before;
import org.junit.Test;


public class TestBirdDensityProfile {

    private RadarScanJava reflectivity;
    private RadarScanJava radialVelocity;
    private int iScan;
    private BirdDensityProfile birdDensityProfile;
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

        birdDensityProfile = new BirdDensityProfile(reflectivity, radialVelocity);

        nAzim = birdDensityProfile.getNumberOfAzimuthBins();
        nRang = birdDensityProfile.getNumberOfRangeBins();

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

        birdDensityProfile.calcTexture(nRangNeighborhood, nAzimNeighborhood, nCountMin,
                texOffset, texScale, vradMissing);

        int nGlobal = nAzim * nRang;
        int[] zeros = new int[nGlobal];
        int[] dbzImage = birdDensityProfile.getReflectivityRaw();
        int[] rhoImage = null;
        int[] zdrImage = null;
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
        int rhoMissing = missing;
        int rhonAzim = nAzim;
        int rhonRang = nRang;
        float rhoOffset = 0.0f;
        float rhoValueScale = 1.0f;
        float rhoThresMin = (float) this.parameterValues.getRHOMIN();
        int zdrMissing = missing;
        int zdrnAzim = nAzim;
        int zdrnRang = nRang;
        float zdrOffset = 0.0f;
        float zdrValueScale = 1.0f;
        float zdrThresMin = (float) this.parameterValues.getZDRMIN();
        // FIXME plus 5? magic number
        int rCellMax = (int) (this.parameterValues.getRANGMAX() + 5);
        int sign = -1;

        int nCells = BirdDensityProfile.findCells(dbzImage, rhoImage, zdrImage, cellImage,
                dbzMissing, dbznAzim, dbznRang, dbzOffset, dbzScale, dbzRangeScale, dbzThresMin,
                rhoMissing, rhonAzim, rhonRang, rhoOffset, rhoValueScale, rhoThresMin,
                zdrMissing, zdrnAzim, zdrnRang, zdrOffset, zdrValueScale, zdrThresMin,
                rCellMax, sign);

        int[] vradImage = birdDensityProfile.getRadialVelocityRaw();
        int[] texImage = birdDensityProfile.getTexture();
        int[] clutterImage = zeros.clone();
        float dbzElev = (float) this.reflectivity.getElevationAngle();
        float vradScale = (float) this.radialVelocity.getDataScale();
        float vradOffset = (float) this.radialVelocity.getDataOffset();
        float clutterScale = 1.0f;
        float clutterOffset = 0.0f;
        int areaMin = 0; // AREACELL?
        float cellDbzMin = 0; // DBZCELL?
        float cellStdDevMax = 0; // STDEVCELL?
        float cellClutterFraction = 0; // CLUTPERCCELL * 100 ?
        float vradMinValue = (float) parameterValues.getVRADMIN();
        float dbzClutterMin = 0; // DBZCLUTTER?
        int cmFlag = 0;
        int dualPolFlag = 0;
        int verbose = 0;

        int nCellsValid = BirdDensityProfile.analyzeCells(dbzImage, vradImage,
                texImage, clutterImage, cellImage,
                dbznRang, dbznAzim, dbzElev, dbzScale, dbzOffset,
                vradScale, vradOffset, clutterScale, clutterOffset,
                texScale, texOffset, nCells, areaMin, cellDbzMin, cellStdDevMax,
                cellClutterFraction, vradMinValue, dbzClutterMin, cmFlag,
                dualPolFlag, verbose);

        final float DEG2RAD = (float) (2 * Math.PI) / 360;
        float azimuthScale = (360.0f / nAzim) * DEG2RAD;
        // FIXME again, what are the units?
        float rangeScale = (float) this.radialVelocity.getRangeScale() / 1000;
        float fringeDist = (float) this.parameterValues.getEMASKMAX();

        BirdDensityProfile.fringeCells(cellImage, nRang, nAzim, azimuthScale,
                rangeScale, fringeDist);

        float dbzHeig = (float) this.reflectivity.getRadarPositionHeight();

        float dbzAzimScale = (360.0f / nAzim) * DEG2RAD;
        int rawReflMissing = missing;
        int[] rawReflImage = zeros.clone();
        // FIXME is zdata the profile information? (Then it should have multiple
        // pseudo-columns)
        float[] zdata = new float[] { 0, 0, 0, 0 };
        float rangeMin = (float) this.parameterValues.getRANGMIN();
        float rangeMax = (float) this.parameterValues.getRANGMAX();
        float HLAYER = (float) this.parameterValues.getHLAYER();
        float XOFFSET = 0.0f; // FIXME unclear what this does
        float XSCALE = 1.0f; // FIXME unclear what this does
        float XMEAN = 0.0f; // FIXME unclear what this does
        float height = 2000.0f;
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

        BirdDensityProfile.classify(dbznRang, dbznAzim, dbzRangeScale,
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

        // TODO finish this stuff right here

    }
}
