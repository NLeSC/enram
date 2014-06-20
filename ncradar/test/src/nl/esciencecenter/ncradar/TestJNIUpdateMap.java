package nl.esciencecenter.ncradar;

import static org.junit.Assert.assertEquals;
import org.junit.Before;
import org.junit.Test;


public class TestJNIUpdateMap extends JNIMethodsVol2Bird {

    private int[] iRangOfMax;
    private int[] iAzimOfMax;
    private float[] dbzAvg;
    private float[] texAvg;
    private float[] cv;
    private float[] clutterArea;
    private float[] dbzMax;
    private int nCells;
    private int nGlobal;
    private int nAzim;
    private int nRang;



    @Before
    public void setUp() throws Exception {

        nAzim = 5;
        nRang = 4;
        nGlobal = nAzim * nRang;

        iRangOfMax = new int[] { 12, 56, 176 };
        iAzimOfMax = new int[] { 6, 119, 243 };
        dbzAvg = new float[] { 89.5f, 2342.536f, 863.33f };
        texAvg = new float[] { 345.78f, 1223.54f, 8979.2f };
        cv = new float[] { 89435.543f, 232.5f, 974.23f };
        clutterArea = new float[] { 78f, 234.5f, 232.3f };
        dbzMax = new float[] { 9837.2f, 53.67f, 8765.23f };

        nCells = 3;

    }



    @Test
    public void testNativeUpdateMap1() throws Exception {

        // test with indices into cellProp starting at 0
        int[] cellImage = new int[] { 0, 0, -1, -1,
                0, 0, 0, -1,
                -1, 0, -1, -1,
                -1, -1, -1, 2,
                1, 1, -1, -1 };
        float[] area = new float[] { 6.0f, 2.0f, 1.0f };
        char[] drop = new char[] { 0, 0, 0 };
        int[] index = new int[] { 0, 1, 2 };
        int minCellArea = 0;

        int nCellsValidActual = updateMap(cellImage, iRangOfMax, iAzimOfMax, dbzAvg,
                texAvg, cv, area, clutterArea, dbzMax, index, drop,
                nCells, nGlobal, minCellArea);

        int nCellsValidExpected = 3;

        int[][] cellImageActual = reshapeTo2D(cellImage, nAzim, nRang);
        int[][] cellImageExpected = new int[][] { { 1, 1, -1, -1 },
                { 1, 1, 1, -1 },
                { -1, 1, -1, -1 },
                { -1, -1, -1, 3 },
                { 2, 2, -1, -1 } };

        int[] indexExpected = new int[] { 1, 2, 3 };

        runTests(nCellsValidActual, nCellsValidExpected,
                cellImageActual, cellImageExpected,
                index, indexExpected);

    }



    @Test
    public void testNativeUpdateMap2() throws Exception {

        // test with indices into cellProp starting at 2
        int[] cellImage = new int[] { 2, 2, -1, -1,
                2, 2, 2, -1,
                -1, 2, -1, -1,
                -1, -1, -1, 4,
                3, 3, -1, -1 };
        float[] area = new float[] { 6.0f, 2.0f, 1.0f };
        char[] drop = new char[] { 0, 0, 0 };
        int[] index = new int[] { 2, 3, 4 };
        int minCellArea = 0;

        int nCellsValidActual = updateMap(cellImage, iRangOfMax, iAzimOfMax, dbzAvg,
                texAvg, cv, area, clutterArea, dbzMax, index, drop,
                nCells, nGlobal, minCellArea);

        int nCellsValidExpected = 3;

        int[][] cellImageActual = reshapeTo2D(cellImage, nAzim, nRang);
        int[][] cellImageExpected = new int[][] { { 1, 1, -1, -1 },
                { 1, 1, 1, -1 },
                { -1, 1, -1, -1 },
                { -1, -1, -1, 3 },
                { 2, 2, -1, -1 } };

        int[] indexExpected = new int[] { 1, 2, 3 };

        runTests(nCellsValidActual, nCellsValidExpected,
                cellImageActual, cellImageExpected,
                index, indexExpected);

    }



    @Test
    public void testNativeUpdateMap3() throws Exception {

        // test with indices into cellProp starting at 2 but with
        // out-of-order areas
        int[] cellImage = new int[] { 3, 3, -1, -1,
                3, 3, 3, -1,
                -1, 3, -1, -1,
                -1, -1, -1, 2,
                4, 4, -1, -1 };
        float[] area = new float[] { 1.0f, 6.0f, 2.0f };
        char[] drop = new char[] { 0, 0, 0 };
        int[] index = new int[] { 2, 3, 4 };
        int minCellArea = 0;

        int nCellsValidActual = updateMap(cellImage, iRangOfMax, iAzimOfMax, dbzAvg,
                texAvg, cv, area, clutterArea, dbzMax, index, drop,
                nCells, nGlobal, minCellArea);

        int nCellsValidExpected = 3;

        int[][] cellImageActual = reshapeTo2D(cellImage, nAzim, nRang);
        int[][] cellImageExpected = new int[][] { { 1, 1, -1, -1 },
                { 1, 1, 1, -1 },
                { -1, 1, -1, -1 },
                { -1, -1, -1, 3 },
                { 2, 2, -1, -1 } };

        int[] indexExpected = new int[] { 2, 3, 1 };

        runTests(nCellsValidActual, nCellsValidExpected,
                cellImageActual, cellImageExpected,
                index, indexExpected);

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



    private void runTests(int nCellsValidActual, int nCellsValidExpected,
            int[][] cellImageActual, int[][] cellImageExpected,
            int[] indexActual, int[] indexExpected) {

        // first test the number of valid cells
        assertEquals(nCellsValidActual, nCellsValidExpected);

        // then test the contents of the altered cellImage
        for (int iAzim = 0; iAzim < nAzim; iAzim++) {
            for (int iRang = 0; iRang < nRang; iRang++) {
                int a = cellImageActual[iAzim][iRang];
                int e = cellImageExpected[iAzim][iRang];
                assertEquals(a, e);
            }
        }

        // then see if the indices have been changed correctly
        for (int iCell = 0; iCell < nCells; iCell++) {
            assertEquals(indexActual[iCell], indexExpected[iCell]);
        }

    }
}