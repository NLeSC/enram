package nl.esciencecenter.ncradar;

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
    private int[] cellImageInt;
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
    private int id;
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

        int dbznRang;
        int dbznAzim;
        float dbzRangeScale;
        float dbzElev;
        float dbzHeig;
        float dbzValueScale;
        float dbzValueOffset;
        float dbzAzimScale;
        int dbzMissing;
        float vradValueScale;
        float vradValueOffset;
        int vradMissing;
        int rawReflMissing;
        float clutterValueScale;
        float clutterValueOffset;
        int[] cellImageInt;
        int[] dbzImageInt;
        int[] vradImageInt;
        int[] rawReflImageInt;
        int[] clutterImageInt;
        float[] zdata;
        int[] nzdata;
        float rangeMin;
        float rangeMax;
        float HLAYER;
        float XOFFSET;
        float XSCALE;
        float XMEAN;
        float height;
        float azimMin;
        float azimMax;
        float vradMin;
        float dbzClutter;
        float dbzMin;
        float dBZx;
        float DBZNOISE;
        int NGAPMIN;
        int NGAPBIN;
        int NDBZMIN;
        int layer;
        int id;
        int np;
        int nPointsPtr;
        int nPointsAllPtr;
        int nPointsClutterPtr;
        int nPointsRainPtr;
        int nPointsRainNoFringePtr;
        int clutterFlagInt;
        int rawReflFlagInt;
        int xflagInt;

    }



    @Test
    public void testNativeClassify() throws Exception {

        classify(dbznRang, dbznAzim, dbzRangeScale, dbzElev, dbzHeig, dbzValueScale, dbzValueOffset,
                dbzAzimScale, dbzMissing, vradValueScale, vradValueOffset, vradMissing, rawReflMissing,
                clutterValueScale, clutterValueOffset, cellImageInt, dbzImageInt, vradImageInt,
                rawReflImageInt, clutterImageInt, zdata, nzdata, rangeMin, rangeMax,
                HLAYER, XOFFSET, XSCALE, XMEAN, height, azimMin, azimMax, vradMin, dbzClutter,
                dbzMin, dBZx, DBZNOISE, NGAPMIN, NGAPBIN, NDBZMIN, layer, id, np, nPointsPtr,
                nPointsAllPtr, nPointsClutterPtr, nPointsRainPtr, nPointsRainNoFringePtr,
                clutterFlagInt, rawReflFlagInt, xflagInt);

    }

}