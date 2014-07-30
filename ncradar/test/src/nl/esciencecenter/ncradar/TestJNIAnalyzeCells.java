package nl.esciencecenter.ncradar;

import static org.junit.Assert.assertEquals;
import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.Scanner;
import org.junit.Test;


public class TestJNIAnalyzeCells extends JNIMethodsVol2Bird {

    private void print(int[] inputArray, int nRows, int nCols) {

        int[][] outputArray = reshapeTo2D(inputArray, nRows, nCols);

        for (int iRow = 0; iRow < nRows; iRow++) {
            for (int iCol = 0; iCol < nCols; iCol++) {

                System.out.printf("%4d", outputArray[iRow][iCol]);

            }
            System.out.print("\n");

        }

    }



    private int[] readDataFromFile(String filename) throws IOException {

        List<Integer> arrList = new ArrayList<Integer>();

        BufferedReader reader = new BufferedReader(new FileReader(filename));

        String line;

        while ((line = reader.readLine()) != null) {

            Scanner scanner = new Scanner(line);
            while (scanner.hasNext()) {
                int nextInt = scanner.nextInt();
                arrList.add(nextInt);
            }

        }
        reader.close();

        int nElems = arrList.size();
        int[] outputArray = new int[nElems];
        for (int iElem = 0; iElem < nElems; iElem++) {
            outputArray[iElem] = arrList.get(iElem).intValue();
        }

        return outputArray;
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



    @Test
    public void testNativeAnalyzeCells1() throws Exception {

        int nAzim = 12;
        int nRang = 11;

        String userName = System.getProperty("user.name");
        String startDir;

        if (userName.equals("wbouten")) {
            startDir = System.getProperty("user.dir") + "/test";
        }
        else {
            startDir = System.getProperty("user.dir");
        }
        System.err.println(startDir);

        int[] dbzImage = readDataFromFile(startDir + "/data/case1/testdata-12x11-pattern-dbz.txt");
        int[] vradImage = readDataFromFile(startDir + "/data/case1/testdata-12x11-pattern-vrad.txt");
        int[] texImage = readDataFromFile(startDir + "/data/case1/testdata-12x11-pattern-tex.txt");
        int[] clutterImage = readDataFromFile(startDir + "/data/case1/testdata-12x11-pattern0.txt");
        int[] cellImage = readDataFromFile(startDir + "/data/case1/testdata-12x11-pattern-cell.txt");
        int dbznRang = nRang;
        int dbznAzim = nAzim;
        float dbzElev = 23.4f;
        float dbzValueScale = 1.0f;
        float dbzValueOffset = 0.0f;
        float vradValueScale = 1.0f;
        float vradValueOffset = 0.0f;
        float clutterValueScale = 1.0f;
        float clutterValueOffset = 0.0f;
        float texValueScale = 1.0f;
        float texValueOffset = 0.0f;
        int nCells = 4;
        int areaMin = 0;
        float cellDbzMin = 0;
        float cellStdDevMax = 0;
        float cellClutterFraction = 0;
        float vradMinValue = 0;
        float dbzClutterMin = 0;
        int cmFlag = 0;
        int dualPolFlag = 0;
        int verbose = 0;

        int nCellsValidExpected = 433; // FIXME not sure what the right anwer
                                       // is;

        int nCellsValid = analyzeCells(dbzImage, vradImage, texImage, clutterImage, cellImage,
                dbznRang, dbznAzim, dbzElev, dbzValueScale, dbzValueOffset,
                vradValueScale, vradValueOffset, clutterValueScale, clutterValueOffset,
                texValueScale, texValueOffset, nCells, areaMin, cellDbzMin, cellStdDevMax,
                cellClutterFraction, vradMinValue, dbzClutterMin, cmFlag, dualPolFlag, verbose);

        int[][] actual = reshapeTo2D(cellImage, nAzim, nRang);

        int[][] expected = {};

        print(cellImage, nAzim, nRang);

        // TODO complete the unit test with verification of cellImage

        assertEquals(nCellsValid, nCellsValidExpected);

    }



    @Test
    public void testNativeAnalyzeCells2() throws Exception {

        // test for when the input arrays are all zeros

        int nAzim = 12;
        int nRang = 11;

        String startDir = System.getProperty("user.dir");
        System.err.println(startDir);

        int[] zeros = new int[nAzim * nRang];
        int[] dbzImage = zeros.clone();
        int[] vradImage = zeros.clone();
        int[] texImage = zeros.clone();
        int[] clutterImage = zeros.clone();
        int[] cellImage = zeros.clone();

        int dbznRang = nRang;
        int dbznAzim = nAzim;
        float dbzElev = 23.4f;
        float dbzValueScale = 1.0f;
        float dbzValueOffset = 0.0f;
        float vradValueScale = 1.0f;
        float vradValueOffset = 0.0f;
        float clutterValueScale = 1.0f;
        float clutterValueOffset = 0.0f;
        float texValueScale = 1.0f;
        float texValueOffset = 0.0f;
        int nCells = 0;
        int areaMin = 0;
        float cellDbzMin = 0;
        float cellStdDevMax = 0;
        float cellClutterFraction = 0;
        float vradMinValue = 0;
        float dbzClutterMin = 0;
        int cmFlag = 0;
        int dualPolFlag = 0;
        int verbose = 0;

        int nCellsValid = analyzeCells(dbzImage, vradImage, texImage, clutterImage, cellImage,
                dbznRang, dbznAzim, dbzElev, dbzValueScale, dbzValueOffset,
                vradValueScale, vradValueOffset, clutterValueScale, clutterValueOffset,
                texValueScale, texValueOffset, nCells, areaMin, cellDbzMin, cellStdDevMax,
                cellClutterFraction, vradMinValue, dbzClutterMin, cmFlag, dualPolFlag, verbose);

        int[][] actual = reshapeTo2D(cellImage, nAzim, nRang);

        int expected = -1;
        for (int iAzim = 0; iAzim < nAzim; iAzim++) {
            for (int iRang = 0; iRang < nRang; iRang++) {

                assertEquals(actual[iAzim][iRang], expected);

            }
        }

        print(cellImage, nAzim, nRang);

        // test whether the number of identified cells equals zero.
        assertEquals(nCellsValid, 0);

    }

}