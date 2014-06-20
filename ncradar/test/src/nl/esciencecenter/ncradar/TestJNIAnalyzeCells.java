package nl.esciencecenter.ncradar;

import org.junit.Before;
import org.junit.Test;


public class TestJNIAnalyzeCells extends JNIMethodsVol2Bird {

    private int[] dbzImage;
    private int[] vradImage;
    private int[] texImage;
    private int[] clutterImage;
    private int[] cellImage;
    private int dbznRang;
    private int dbznAzim;
    private float dbzElev;
    private float dbzValueScale;
    private float dbzValueOffset;
    private float vradValueScale;
    private float vradValueOffset;
    private float clutterValueScale;
    private float clutterValueOffset;
    private float texValueScale;
    private float texValueOffset;
    private int nCells;
    private int areaMin;
    private float cellDbzMin;
    private float cellStdDevMax;
    private float cellClutterFraction;
    private float vradMinValue;
    private float dbzClutterMin;
    private int cmFlagInt;
    private int dualPolFlagInt;
    private int verboseInt;



    @Before
    public void setUp() throws Exception {

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

        // TODO initialize all variables needed for calling analyzeCells

    }



    @Test
    public void testNativeAnalyzeCells() throws Exception {

        int nCells = analyseCells(dbzImage, vradImage, texImage, clutterImage, cellImage,
                dbznRang, dbznAzim,
                dbzElev, dbzValueScale, dbzValueOffset,
                vradValueScale, vradValueOffset,
                clutterValueScale, clutterValueOffset,
                texValueScale, texValueOffset,
                nCells, areaMin, cellDbzMin, cellStdDevMax, cellClutterFraction,
                vradMinValue, dbzClutterMin, cmFlagInt, dualPolFlagInt, verboseInt);

        int nAzim = 5;
        int nRang = 4;

        int[][] actual = reshapeTo2D(cellImage, nAzim, nRang);

        int[][] expected = {};

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