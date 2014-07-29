package nl.esciencecenter.ncradar;

import static org.junit.Assert.assertEquals;
import org.junit.Before;
import org.junit.Test;


public class TestJNISortCells extends JNIMethodsVol2Bird {

    private int[] iRangOfMax;
    private int[] iRangOfMaxOrig;
    private int[] iAzimOfMax;
    private int[] iAzimOfMaxOrig;
    private float[] dbzAvg;
    private float[] dbzAvgOrig;
    private float[] texAvg;
    private float[] texAvgOrig;
    private float[] cv;
    private float[] cvOrig;
    private float[] area;
    private float[] areaOrig;
    private float[] clutterArea;
    private float[] clutterAreaOrig;
    private float[] dbzMax;
    private float[] dbzMaxOrig;
    private int[] index;
    private int[] indexOrig;
    private char[] drop;
    private char[] dropOrig;
    private int nCells;
    private float delta;



    @Before
    public void setUp() throws Exception {

        iRangOfMax = new int[] { 12, 56, 176, 34, 6 };
        iRangOfMaxOrig = iRangOfMax.clone();
        iAzimOfMax = new int[] { 6, 119, 243, 189, 6723 };
        iAzimOfMaxOrig = iAzimOfMax.clone();
        dbzAvg = new float[] { 89.5f, 2342.536f, 863.33f, 478.54f, 5403.78f };
        dbzAvgOrig = dbzAvg.clone();
        texAvg = new float[] { 345.78f, 1223.54f, 8979.2f, 6239.2f, 4623.455f };
        texAvgOrig = texAvg.clone();
        cv = new float[] { 89435.543f, 232.5f, 974.23f, 23.562f, 8943.793f };
        cvOrig = cv.clone();
        area = new float[] { 456f, 676.32f, 32.64f, 3543.9f, 6739.08984f };
        areaOrig = area.clone();
        clutterArea = new float[] { 78f, 234.5f, 232.3f, 95.3f, 78356.23f };
        clutterAreaOrig = clutterArea.clone();
        dbzMax = new float[] { 9837.2f, 53.67f, 8765.23f, 2433.878f, 7893.353f };
        dbzMaxOrig = dbzMax.clone();
        index = new int[] { 2, 7, 96, 5483, 723 };
        indexOrig = index.clone();

        nCells = 5;
        delta = 0.0000001f;

    }



    @Test
    public void testNativeSortCells1() throws Exception {

        // sort by descending area of the cell (cellProp[iElem].area)
        // without dropping any cells

        drop = new char[] { 0, 0, 0, 0, 0 };
        dropOrig = drop.clone();

        sortCells(iRangOfMax, iAzimOfMax, dbzAvg, texAvg, cv, area, clutterArea,
                dbzMax, index, drop, nCells);

        int[] rightOrder = new int[] { 4, 3, 1, 0, 2 };

        verifyOrder(rightOrder);

    }



    @Test
    public void testNativeSortCells3() throws Exception {

        // sort by descending area of the cell (cellProp[iElem].area)
        // dropping two cells

        drop = new char[] { 1, 0, 0, 1, 0 };
        dropOrig = drop.clone();

        sortCells(iRangOfMax, iAzimOfMax, dbzAvg, texAvg, cv, area, clutterArea,
                dbzMax, index, drop, nCells);

        int[] rightOrder = new int[] { 4, 1, 2, 0, 3 };

        verifyOrder(rightOrder);

    }



    public void verifyOrder(int[] origArray, int[] changedArray, int[] rightOrder) {

        int nElems = origArray.length;

        for (int iElem = 0; iElem < nElems; iElem++) {
            assertEquals(origArray[iElem], changedArray[rightOrder[iElem]]);

        }
    }



    public void verifyOrder(float[] origArray, float[] changedArray, int[] rightOrder, float delta) {

        int nElems = origArray.length;

        for (int iElem = 0; iElem < nElems; iElem++) {
            assertEquals(origArray[iElem], changedArray[rightOrder[iElem]], delta);

        }
    }



    public void verifyOrder(char[] origArray, char[] changedArray, int[] rightOrder) {

        int nElems = origArray.length;

        for (int iElem = 0; iElem < nElems; iElem++) {
            assertEquals(origArray[iElem], changedArray[rightOrder[iElem]]);

        }
    }



    public void verifyOrder(int[] rightOrder) {

        verifyOrder(iRangOfMax, iRangOfMaxOrig, rightOrder);
        verifyOrder(iAzimOfMax, iAzimOfMaxOrig, rightOrder);
        verifyOrder(dbzAvg, dbzAvgOrig, rightOrder, delta);
        verifyOrder(texAvg, texAvgOrig, rightOrder, delta);
        verifyOrder(cv, cvOrig, rightOrder, delta);
        verifyOrder(area, areaOrig, rightOrder, delta);
        verifyOrder(clutterArea, clutterAreaOrig, rightOrder, delta);
        verifyOrder(dbzMax, dbzMaxOrig, rightOrder, delta);
        verifyOrder(index, indexOrig, rightOrder);
        verifyOrder(drop, dropOrig, rightOrder);

    }

}