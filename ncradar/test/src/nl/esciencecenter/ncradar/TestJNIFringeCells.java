package nl.esciencecenter.ncradar;

import static org.junit.Assert.assertEquals;
import org.junit.Before;
import org.junit.Test;


public class TestJNIFringeCells extends JNIMethodsVol2Bird {

    private int nRang;
    private int nAzim;
    private float azimuthScale;
    private float rangeScale;
    private float fringe;
    private double DEG2RAD;



    @Before
    public void setUp() throws Exception {

        DEG2RAD = 0.017453293;
        nRang = 4;
        nAzim = 5;
        azimuthScale = (float) (1.0f / DEG2RAD);
        rangeScale = 1.0f;
        fringe = 1.0f;

    }



    @Test
    public void testNativeFringeCells1() throws Exception {

        int[] cellImage = new int[] { 2, 0, 0, 0,
                0, 0, 0, 0,
                0, 0, 0, 0,
                0, 0, 0, 0,
                0, 0, 0, 0 };

        fringeCells(cellImage, nRang, nAzim, azimuthScale, rangeScale, fringe);

        int[][] actual = reshapeTo2D(cellImage, nAzim, nRang);

        int[][] expected = reshapeTo2D(new int[] { 2, 1, 0, 0,
                1, 1, 0, 0,
                0, 0, 0, 0,
                0, 0, 0, 0,
                1, 1, 0, 0 }, nAzim, nRang);

        for (int iAzim = 0; iAzim < nAzim; iAzim++) {
            for (int iRang = 0; iRang < nRang; iRang++) {

                assertEquals(actual[iAzim][iRang], expected[iAzim][iRang]);

            }
        }

    }



    @Test
    public void testNativeFringeCells2() throws Exception {

        int[] cellImage = new int[] { 0, 0, 0, 2,
                0, 0, 0, 0,
                0, 0, 0, 0,
                0, 0, 0, 0,
                0, 0, 0, 0 };

        fringeCells(cellImage, nRang, nAzim, azimuthScale, rangeScale, fringe);

        int[][] actual = reshapeTo2D(cellImage, nAzim, nRang);

        int[][] expected = reshapeTo2D(new int[] { 0, 0, 1, 2,
                0, 0, 1, 1,
                0, 0, 0, 0,
                0, 0, 0, 0,
                0, 0, 1, 1 }, nAzim, nRang);

        for (int iAzim = 0; iAzim < nAzim; iAzim++) {
            for (int iRang = 0; iRang < nRang; iRang++) {

                assertEquals(actual[iAzim][iRang], expected[iAzim][iRang]);

            }
        }

    }



    @Test
    public void testNativeFringeCells3() throws Exception {

        int[] cellImage = new int[] { 0, 0, 0, 0,
                0, 0, 0, 0,
                0, 0, 0, 0,
                0, 0, 0, 0,
                0, 0, 0, 2 };

        fringeCells(cellImage, nRang, nAzim, azimuthScale, rangeScale, fringe);

        int[][] actual = reshapeTo2D(cellImage, nAzim, nRang);

        int[][] expected = reshapeTo2D(new int[] { 0, 0, 1, 1,
                0, 0, 0, 0,
                0, 0, 0, 0,
                0, 0, 1, 1,
                0, 0, 1, 2 }, nAzim, nRang);

        for (int iAzim = 0; iAzim < nAzim; iAzim++) {
            for (int iRang = 0; iRang < nRang; iRang++) {

                assertEquals(actual[iAzim][iRang], expected[iAzim][iRang]);

            }
        }

    }



    @Test
    public void testNativeFringeCells4() throws Exception {

        int[] cellImage = new int[] { 0, 0, 0, 0,
                0, 0, 0, 0,
                0, 0, 0, 0,
                0, 0, 0, 0,
                2, 0, 0, 0 };

        fringeCells(cellImage, nRang, nAzim, azimuthScale, rangeScale, fringe);

        int[][] actual = reshapeTo2D(cellImage, nAzim, nRang);

        int[][] expected = reshapeTo2D(new int[] { 1, 1, 0, 0,
                0, 0, 0, 0,
                0, 0, 0, 0,
                1, 1, 0, 0,
                2, 1, 0, 0 }, nAzim, nRang);

        for (int iAzim = 0; iAzim < nAzim; iAzim++) {
            for (int iRang = 0; iRang < nRang; iRang++) {

                assertEquals(actual[iAzim][iRang], expected[iAzim][iRang]);

            }
        }

    }



    @Test
    public void testNativeFringeCells5() throws Exception {

        int[] cellImage = new int[] { 0, 0, 0, 0,
                0, 67, 0, 0,
                0, 0, 0, 0,
                0, 5, 0, 0,
                0, 0, 0, 0 };

        fringeCells(cellImage, nRang, nAzim, azimuthScale, rangeScale, fringe);

        int[][] actual = reshapeTo2D(cellImage, nAzim, nRang);

        int[][] expected = reshapeTo2D(new int[] { 1, 1, 1, 0,
                1, 67, 1, 0,
                1, 1, 1, 0,
                1, 5, 1, 0,
                1, 1, 1, 0 }, nAzim, nRang);

        for (int iAzim = 0; iAzim < nAzim; iAzim++) {
            for (int iRang = 0; iRang < nRang; iRang++) {
                assertEquals(actual[iAzim][iRang], expected[iAzim][iRang]);
            }
        }
    }



    private int[][] reshapeTo2D(int[] inputArray, int nRows, int nCols) {

        int[][] outputArray = new int[nRows][nCols];
        int iGlobal = 0;
        for (int iRow = 0; iRow < nRows; iRow++) {
            for (int iCol = 0; iCol < nCols; iCol++) {

                outputArray[iRow][iCol] = inputArray[iGlobal];
                iGlobal += 1;

            }
        }

        return outputArray;

    }

}