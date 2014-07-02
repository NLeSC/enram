package nl.esciencecenter.ncradar;

import org.junit.Test;


public class TestJNIFindCells extends JNIMethodsVol2Bird {

    @Test
    public void testNativeFindCells() throws Exception {

        int nAzim = 5;
        int nRang = 7;

        int missing = 255;

        int[] texImage = new int[] { 0, 54, 54, 45, 0, 0, 0,
                0, 45, 45, 45, 0, 0, 0,
                0, 54, 54, 54, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, };
        int[] rhoImage = null;
        int[] zdrImage = null;
        int[] cellImage = new int[] { 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0 };
        int texMissing = missing;
        int texnAzim = nAzim;
        int texnRang = nRang;
        float texValueOffset = 0.0f;
        float texValueScale = 1.0f;
        float texRangeScale = 10.0f;
        float texThresMin = 130.0f;
        int rhoMissing = missing;
        int rhonAzim = nAzim;
        int rhonRang = nRang;
        float rhoValueOffset = 0.0f;
        float rhoValueScale = 1.0f;
        float rhoThresMin = 0.0f;
        int zdrMissing = missing;
        int zdrnAzim = nAzim;
        int zdrnRang = nRang;
        float zdrValueOffset = 0.0f;
        float zdrValueScale = 1.0f;
        float zdrThresMin = 0.0f;
        float dbzThresMin = 0.0f;
        int rCellMax = (int) (nRang * texRangeScale);
        int sign = 1;

        int nCells = findCells(texImage, rhoImage, zdrImage, cellImage,
                texMissing, texnAzim, texnRang, texValueOffset, texValueScale, texRangeScale, texThresMin,
                rhoMissing, rhonAzim, rhonRang, rhoValueOffset, rhoValueScale, rhoThresMin,
                zdrMissing, zdrnAzim, zdrnRang, zdrValueOffset, zdrValueScale, zdrThresMin,
                dbzThresMin, rCellMax, sign);

        int[][] cellImageActual = reshapeTo2D(cellImage, nAzim, nRang);

        int[][] cellImageExpected = { { nCells } };

        // TODO test nCells
        // TODO test cellImage result

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