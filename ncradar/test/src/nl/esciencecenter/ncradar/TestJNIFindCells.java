package nl.esciencecenter.ncradar;

import org.junit.Before;
import org.junit.Test;


public class TestJNIFindCells extends JNIMethodsVol2Bird {

    private int[] texImage;
    private int[] rhoImage;
    private int[] zdrImage;
    private int[] cellImage;
    private int texMissing;
    private int texnAzim;
    private int texnRang;
    private float texValueOffset;
    private float texRangeScale;
    private float texValueScale;
    private float texThresMin;
    private int rhoMissing;
    private int rhonAzim;
    private int rhonRang;
    private float rhoValueOffset;
    private float rhoValueScale;
    private float rhoThresMin;
    private int zdrMissing;
    private int zdrnAzim;
    private int zdrnRang;
    private float zdrValueOffset;
    private float zdrValueScale;
    private float zdrThresMin;
    private float reflThresMin;
    private float rCellMax;
    private int sign;
    private int nAzim;
    private int nRang;



    @Before
    public void setUp() throws Exception {

        nAzim = 5;
        nRang = 4;

        int missing = 255;

        texImage = new int[] { 0, 0, 0, 0,
                0, 0, 0, 0,
                0, 0, 0, 0,
                0, 0, 0, 0,
                0, 0, 0, 0 };
        rhoImage = new int[] { 0, 0, 0, 0,
                0, 0, 0, 0,
                0, 0, 0, 0,
                0, 0, 0, 0,
                0, 0, 0, 0 };
        zdrImage = new int[] { 0, 0, 0, 0,
                0, 0, 0, 0,
                0, 0, 0, 0,
                0, 0, 0, 0,
                0, 0, 0, 0 };
        cellImage = new int[] { 0, 0, 0, 0,
                0, 0, 0, 0,
                0, 0, 0, 0,
                0, 0, 0, 0,
                0, 0, 0, 0 };
        texMissing = missing;
        texnAzim = nAzim;
        texnRang = nRang;
        texValueOffset = 0.0f;
        texRangeScale = 1.0f;
        texValueScale = 1.0f;
        texThresMin = 0.0f;
        rhoMissing = missing;
        rhonAzim = nAzim;
        rhonRang = nRang;
        rhoValueOffset = 0.0f;
        rhoValueScale = 1.0f;
        rhoThresMin = 0.0f;
        zdrMissing = missing;
        zdrnAzim = nAzim;
        zdrnRang = nRang;
        zdrValueOffset = 0.0f;
        zdrValueScale = 1.0f;
        zdrThresMin = 0.0f;
        reflThresMin = 0.0f;
        rCellMax = 0;
        sign = 0;

    }



    @Test
    public void testNativeFindCells() throws Exception {

        // TODO this call sometimes hangs...ask somebody

        int nCells = findCells(texImage, rhoImage, zdrImage, cellImage,
                texMissing, texnAzim, texnRang, texValueOffset, texRangeScale, texValueScale, texThresMin,
                rhoMissing, rhonAzim, rhonRang, rhoValueOffset, rhoValueScale, rhoThresMin,
                zdrMissing, zdrnAzim, zdrnRang, zdrValueOffset, zdrValueScale, zdrThresMin,
                reflThresMin, rCellMax, sign);

        int[][] actual = reshapeTo2D(cellImage, nAzim, nRang);

        int[][] expected = {};

        // FIXME test nCells

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