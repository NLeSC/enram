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



    @Before
    public void setUp() throws Exception {

        iScan = 3;
        reflectivity = new RadarScanJava("/home/wbouten/tmp", "T_PAGZ60_C_OKPR_20110815000447.hdf", iScan);
        radialVelocity = new RadarScanJava("/home/wbouten/tmp", "T_PAHZ60_C_OKPR_20110815000447.hdf", iScan);

        birdDensityProfile = new BirdDensityProfile(reflectivity, radialVelocity);

        nAzim = birdDensityProfile.getNumberOfAzimuthBins();
        nRang = birdDensityProfile.getNumberOfRangeBins();

        System.out.println("I have a BirdDensityProfile object.");

    }



    @Test
    public void testBirdDensityProfile() throws Exception {

        int missing = 255;
        int nGlobal = nAzim * nRang;

        // offsets
        float dbzOffset = 0.0f;
        float vradOffset = 0.0f;
        float zdrOffset = 0.0f;
        float rhoOffset = 0.0f;
        float texOffset = 0;
        float clutterOffset = 0.0f;

        // scaling
        float dbzScale = 1.0f;
        float vradScale = 1.0f;
        float rhoValueScale = 1.0f;
        float zdrValueScale = 1.0f;
        float texScale = 1.0f;
        float clutterScale = 1.0f;

        // missing
        int dbzMissing = missing;
        int rawReflMissing = missing;
        int vradMissing = missing;
        int rhoMissing = missing;
        int zdrMissing = missing;

        // nRang
        int dbznRang = nRang;
        int rhonRang = nRang;
        int zdrnRang = nRang;

        // nAzim
        int dbznAzim = nAzim;
        int rhonAzim = nAzim;
        int zdrnAzim = nAzim;

        // thresMin
        float dbzThresMin = 130.0f;
        float rhoThresMin = 0.0f;
        float zdrThresMin = 0.0f;

        // loose ends
        final float DEG2RAD = (float) (2 * Math.PI) / 360;
        int nRangNeighborhood = 3;
        int nAzimNeighborhood = 3;
        int nCountMin = 0;
        float dbzRangeScale = 10.0f;
        float dbzElev = 23.4f;
        float dbzClutterMin = 0;

        int rCellMax = (int) (nRang * dbzRangeScale);
        int sign = -1;
        int nCellsValid;

        int nCells;
        int areaMin = 0;
        float cellDbzMin = 0;
        float cellStdDevMax = 0;
        float cellClutterFraction = 0;
        float vradMinValue = 0;

        int cmFlag = 0;
        int dualPolFlag = 0;
        int verbose = 0;
        float azimuthScale = 1.0f / DEG2RAD;
        float rangeScale = 1.0f;
        float fringeDist = 1.0f;

        int[] zeros = new int[nGlobal];
        int[] dbzImage = birdDensityProfile.getReflectivityRaw();
        int[] vradImage = birdDensityProfile.getRadialVelocityRaw();
        int[] rawReflImage = zeros.clone();
        int[] rhoImage = null;
        int[] zdrImage = null;
        int[] texImage = zeros.clone();
        int[] cellImage = zeros.clone();
        int[] clutterImage = zeros.clone();

        float dbzHeig = 0.0f;
        float dbzAzimScale = (float) (2 * Math.PI / 360);

        float[] zdata = new float[] { 0, 0, 0, 0 };
        int[] nzdata = new int[] { 0, 0, 0 };
        float rangeMin = 0.0f;
        float rangeMax = 10000.0f;
        float HLAYER = 0.0f;
        float XOFFSET = 0.0f;
        float XSCALE = 0.0f;
        float XMEAN = 0.0f;
        float height = 0.0f;
        float azimMin = 0.0f;
        float azimMax = (float) (2 * Math.PI);
        float vradMin = 0.0f;
        float dbzClutter = 100.0f;
        float dbzMin = 0.0f;
        float dBZx = 0.0f;
        float DBZNOISE = 0.0f;
        int NGAPMIN = 0;
        int NGAPBIN = 0;
        int NDBZMIN = 0;
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

        birdDensityProfile.calcTexture(nRangNeighborhood, nAzimNeighborhood, nCountMin,
                texOffset, texScale, vradMissing);

        texImage = birdDensityProfile.getTexture();

        nCells = BirdDensityProfile.findCells(dbzImage, rhoImage, zdrImage, cellImage,
                dbzMissing, dbznAzim, dbznRang, dbzOffset, dbzScale, dbzRangeScale, dbzThresMin,
                rhoMissing, rhonAzim, rhonRang, rhoOffset, rhoValueScale, rhoThresMin,
                zdrMissing, zdrnAzim, zdrnRang, zdrOffset, zdrValueScale, zdrThresMin,
                rCellMax, sign);

        System.out.println("past findcells");

        nCellsValid = BirdDensityProfile.analyzeCells(dbzImage, vradImage, texImage, clutterImage, cellImage,
                dbznRang, dbznAzim, dbzElev, dbzScale, dbzOffset,
                vradScale, vradOffset, clutterScale, clutterOffset,
                texScale, texOffset, nCells, areaMin, cellDbzMin, cellStdDevMax,
                cellClutterFraction, vradMinValue, dbzClutterMin, cmFlag, dualPolFlag, verbose);

        System.out.println("past analyzecells");

        BirdDensityProfile.fringeCells(cellImage, nRang, nAzim, azimuthScale, rangeScale, fringeDist);

        System.out.println("past fringecells");

        BirdDensityProfile.classify(dbznRang, dbznAzim, dbzRangeScale, dbzElev, dbzHeig, dbzScale, dbzOffset,
                dbzAzimScale, dbzMissing, vradScale, vradOffset, vradMissing, rawReflMissing,
                clutterScale, clutterOffset, cellImage, dbzImage, vradImage,
                rawReflImage, clutterImage, zdata, rangeMin, rangeMax,
                HLAYER, XOFFSET, XSCALE, XMEAN, height, azimMin, azimMax, vradMin, dbzClutter,
                dbzMin, dBZx, DBZNOISE, NGAPMIN, NGAPBIN, NDBZMIN, layer, np, nPointsPtr,
                nPointsAllPtr, nPointsClutterPtr, nPointsRainPtr, nPointsRainNoFringePtr,
                clutterFlagInt, rawReflFlagInt, xflagInt);

        System.out.println("past classify");

        // TODO finish this stuff right here

    }
}
