package nl.esciencecenter.ncradar;

import static org.junit.Assert.assertEquals;
import org.junit.Before;
import org.junit.Test;


public class TestJNIClassify extends JNIMethodsVol2Bird {

    private int dbznRang;
    private int dbznAzim;
    private float dbzRangeScale;
    private float dbzElev;
    private float dbzHeig;
    private float dbzValueScale;
    private float dbzValueOffset;
    private float dbzAzimScale;
    private int dbzMissing;
    private float vradValueScale;
    private float vradValueOffset;
    private int vradMissing;
    private int rawReflMissing;
    private float clutterValueScale;
    private float clutterValueOffset;
    private int[] cellImage;
    private int[] dbzImageInt;
    private int[] vradImageInt;
    private int[] rawReflImageInt;
    private int[] clutterImageInt;
    private float[] zdata;
    private int[] nzdata;
    private float rangeMin;
    private float rangeMax;
    private float HLAYER;
    private float XOFFSET;
    private float XSCALE;
    private float XMEAN;
    private float height;
    private float azimMin;
    private float azimMax;
    private float vradMin;
    private float dbzClutter;
    private float dbzMin;
    private float dBZx;
    private float DBZNOISE;
    private int NGAPMIN;
    private int NGAPBIN;
    private int NDBZMIN;
    private int layer;
    private int np;
    private int nPointsPtr;
    private int nPointsAllPtr;
    private int nPointsClutterPtr;
    private int nPointsRainPtr;
    private int nPointsRainNoFringePtr;
    private int clutterFlagInt;
    private int rawReflFlagInt;
    private int xflagInt;



    @Before
    public void setUp() throws Exception {

        // TODO fill in values

        dbznRang = 4;
        dbznAzim = 5;
        dbzRangeScale = 1.0f;
        dbzElev = 0.0f;
        dbzHeig = 0.0f;
        dbzValueScale = 1.0f;
        dbzValueOffset = 0.0f;
        dbzAzimScale = (float) (2 * Math.PI / 360);
        dbzMissing = 255;
        vradValueScale = 1.0f;
        vradValueOffset = 0.0f;
        vradMissing = 255;
        rawReflMissing = 255;
        clutterValueScale = 10.0f;
        clutterValueOffset = 100.0f;
        cellImage = new int[] { 0, 1, 2, 3, 4, 5, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
        dbzImageInt = new int[] { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
        vradImageInt = new int[] { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
        rawReflImageInt = new int[] { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
        clutterImageInt = new int[] { 0, 0, 43, 4, 22, 0, 112, 0, 0, 45, 0, 23, 0, 0, 12, 0, 0, 0, 0, 0 };
        zdata = new float[] { 0, 0, 0, 0 };
        nzdata = new int[] { 0, 0, 0 };
        rangeMin = 0.0f;
        rangeMax = 10000.0f;
        HLAYER = 0.0f;
        XOFFSET = 0.0f;
        XSCALE = 0.0f;
        XMEAN = 0.0f;
        height = 0.0f;
        azimMin = 0.0f;
        azimMax = (float) (2 * Math.PI);
        vradMin = 0.0f;
        dbzClutter = 100.0f;
        dbzMin = 0.0f;
        dBZx = 0.0f;
        DBZNOISE = 0.0f;
        NGAPMIN = 0;
        NGAPBIN = 0;
        NDBZMIN = 0;
        layer = 0;
        np = 0;
        nPointsPtr = 0;
        nPointsAllPtr = 0;
        nPointsClutterPtr = 0;
        nPointsRainPtr = 0;
        nPointsRainNoFringePtr = 0;
        clutterFlagInt = 1;
        rawReflFlagInt = 0;
        xflagInt = 0;

    }



    @Test
    public void testNativeClassify() throws Exception {

        int[] cellImageOrig = cellImage.clone();
        int nGlobal = cellImage.length;

        classify(dbznRang, dbznAzim, dbzRangeScale, dbzElev, dbzHeig, dbzValueScale, dbzValueOffset,
                dbzAzimScale, dbzMissing, vradValueScale, vradValueOffset, vradMissing, rawReflMissing,
                clutterValueScale, clutterValueOffset, cellImage, dbzImageInt, vradImageInt,
                rawReflImageInt, clutterImageInt, zdata, rangeMin, rangeMax,
                HLAYER, XOFFSET, XSCALE, XMEAN, height, azimMin, azimMax, vradMin, dbzClutter,
                dbzMin, dBZx, DBZNOISE, NGAPMIN, NGAPBIN, NDBZMIN, layer, np, nPointsPtr,
                nPointsAllPtr, nPointsClutterPtr, nPointsRainPtr, nPointsRainNoFringePtr,
                clutterFlagInt, rawReflFlagInt, xflagInt);

        for (int iGlobal = 0; iGlobal < nGlobal; iGlobal++) {

            assertEquals(cellImage[iGlobal], cellImageOrig[iGlobal]);

        }

    }

}