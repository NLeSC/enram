package nl.esciencecenter.ncradar;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertTrue;
import org.junit.Before;
import org.junit.Test;


public class TestBirdDensityProfileSortCells {

    private final int[] iRangOfMax = { 12, 56, 176 };
    private final int[] iAzimOfMax = { 6, 119, 243 };
    private final float[] dbzAvg = { 83.5f, 2342.536f, 863.33f };
    private final float[] texAvg = { 345.78f, 1223.54f, 8979.2f };
    private final float[] cv = { 89435.543f, 232.5f, 974.23f };
    private final float[] area = { 456f, 323.64f, 6776.32f };
    private final float[] clutterArea = { 78f, 234.5f, 232.3f };
    private final float[] dbzMax = { 9837.2f, 53.67f, 8765.23f };
    private final int[] index = { 2, 7, 96 };
    private final char[] drop = { 0, 0, 0 };
    private CellProperties cellPropUnsorted;
    private final float delta = 0.0000001f;



    @Before
    public void setUp() throws Exception {

        cellPropUnsorted = new CellProperties(iRangOfMax, iAzimOfMax, dbzAvg, texAvg,
                cv, area, clutterArea, dbzMax, index, drop);

    }



    @Test
    public void testSortCellsByArea() throws Exception {

        // test if the elements of the cell array are in the right order

        int sortByMethod = 1; // 1 = by (decreasing ?) area

        CellProperties cellPropSorted = sortCells(cellPropUnsorted, sortByMethod);

        int[] correctOrder = { 1, 2, 0 };
        int iCellSorted;

        int nCells = cellPropUnsorted.getnCells();

        for (int iCellUnsorted = 0; iCellUnsorted < nCells; iCellUnsorted++) {

            iCellSorted = correctOrder[iCellUnsorted];
            {
                float expected = cellPropUnsorted.getArea(iCellUnsorted);
                float actual = cellPropSorted.getArea(iCellSorted);
                assertEquals(expected, actual, delta);
            }
            {
                float expected = cellPropUnsorted.getClutterArea(iCellUnsorted);
                float actual = cellPropSorted.getClutterArea(iCellSorted);
                assertEquals(expected, actual, delta);
            }
            {
                float expected = cellPropUnsorted.getCv(iCellUnsorted);
                float actual = cellPropSorted.getCv(iCellSorted);
                assertEquals(expected, actual, delta);
            }
            {
                float expected = cellPropUnsorted.getDbzAvg(iCellUnsorted);
                float actual = cellPropSorted.getDbzAvg(iCellSorted);
                assertEquals(expected, actual, delta);
            }
            {
                float expected = cellPropUnsorted.getDbzMax(iCellUnsorted);
                float actual = cellPropSorted.getDbzMax(iCellSorted);
                assertEquals(expected, actual, delta);
            }
            {
                char expected = cellPropUnsorted.getDrop(iCellUnsorted);
                char actual = cellPropSorted.getDrop(iCellSorted);
                assertEquals(expected, actual);
            }
            {
                int expected = cellPropUnsorted.getiAzimOfMax(iCellUnsorted);
                int actual = cellPropSorted.getiAzimOfMax(iCellSorted);
                assertEquals(expected, actual);
            }
            {
                int expected = cellPropUnsorted.getIndex(iCellUnsorted);
                int actual = cellPropSorted.getIndex(iCellSorted);
                assertEquals(expected, actual);
            }
            {
                int expected = cellPropUnsorted.getiRangOfMax(iCellUnsorted);
                int actual = cellPropSorted.getiRangOfMax(iCellSorted);
                assertEquals(expected, actual);
            }
            {
                float expected = cellPropUnsorted.getTexAvg(iCellUnsorted);
                float actual = cellPropSorted.getTexAvg(iCellSorted);
                assertEquals(expected, actual, delta);
            }
        }

    }



    @Test
    public void testSortCellsByDbzAvg() throws Exception {

        // test if the elements of the cell array are in the right order

        int sortByMethod = 2; // 2 = by (decreasing ?) avg dbz

        CellProperties cellPropSorted = sortCells(cellPropUnsorted, sortByMethod);

        int[] correctOrder = { 2, 0, 1 };
        int iCellSorted;

        int nCells = cellPropUnsorted.getnCells();

        for (int iCellUnsorted = 0; iCellUnsorted < nCells; iCellUnsorted++) {

            iCellSorted = correctOrder[iCellUnsorted];
            {
                float expected = cellPropUnsorted.getArea(iCellUnsorted);
                float actual = cellPropSorted.getArea(iCellSorted);
                assertEquals(expected, actual, delta);
            }
            {
                float expected = cellPropUnsorted.getClutterArea(iCellUnsorted);
                float actual = cellPropSorted.getClutterArea(iCellSorted);
                assertEquals(expected, actual, delta);
            }
            {
                float expected = cellPropUnsorted.getCv(iCellUnsorted);
                float actual = cellPropSorted.getCv(iCellSorted);
                assertEquals(expected, actual, delta);
            }
            {
                float expected = cellPropUnsorted.getDbzAvg(iCellUnsorted);
                float actual = cellPropSorted.getDbzAvg(iCellSorted);
                assertEquals(expected, actual, delta);
            }
            {
                float expected = cellPropUnsorted.getDbzMax(iCellUnsorted);
                float actual = cellPropSorted.getDbzMax(iCellSorted);
                assertEquals(expected, actual, delta);
            }
            {
                char expected = cellPropUnsorted.getDrop(iCellUnsorted);
                char actual = cellPropSorted.getDrop(iCellSorted);
                assertEquals(expected, actual);
            }
            {
                int expected = cellPropUnsorted.getiAzimOfMax(iCellUnsorted);
                int actual = cellPropSorted.getiAzimOfMax(iCellSorted);
                assertEquals(expected, actual);
            }
            {
                int expected = cellPropUnsorted.getIndex(iCellUnsorted);
                int actual = cellPropSorted.getIndex(iCellSorted);
                assertEquals(expected, actual);
            }
            {
                int expected = cellPropUnsorted.getiRangOfMax(iCellUnsorted);
                int actual = cellPropSorted.getiRangOfMax(iCellSorted);
                assertEquals(expected, actual);
            }
            {
                float expected = cellPropUnsorted.getTexAvg(iCellUnsorted);
                float actual = cellPropSorted.getTexAvg(iCellSorted);
                assertEquals(expected, actual, delta);
            }
        }

    }



    @Test
    public void testSortCellsByAreaCheckIfDecreasingArea() throws Exception {

        // test if the area property of the cell array are in fact decreasing in
        // value

        int sortByMethod = 1; // 1 = by (decreasing ?) area

        CellProperties cellPropSorted = sortCells(cellPropUnsorted, sortByMethod);

        int nCells = cellPropUnsorted.getnCells();

        float lastArea = cellPropSorted.getArea(0);

        for (int iCell = 0; iCell < nCells; iCell++) {

            float thisArea = cellPropSorted.getArea(iCell);
            boolean condition = lastArea >= thisArea;

            assertTrue(condition);

            lastArea = thisArea;

        }

    }



    @Test
    public void testSortCellsByAreaCheckIfDecreasingDbzAvg() throws Exception {

        // test if the area property of the cell array are in fact decreasing in
        // value

        int sortByMethod = 2; // 1 = by (decreasing ?) dbzavg

        CellProperties cellPropSorted = sortCells(cellPropUnsorted, sortByMethod);

        int nCells = cellPropUnsorted.getnCells();

        float lastDbzAvg = cellPropSorted.getDbzAvg(0);

        for (int iCell = 0; iCell < nCells; iCell++) {

            float thisDbzAvg = cellPropSorted.getDbzAvg(iCell);
            boolean condition = lastDbzAvg >= thisDbzAvg;

            assertTrue(condition);

            lastDbzAvg = thisDbzAvg;

        }

    }

}
