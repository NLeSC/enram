package nl.esciencecenter.ncradar;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.Scanner;
import org.junit.Test;


public class TestJNIVvp extends JNIMethodsVol2Bird {

    private int[] texImage;
    private int[] dbzImage;



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
    public void testNativeVvp1() throws Exception {

        int nAzim = 12;
        int nRang = 11;
        int nGlobal = nAzim * nRang;
        int nDims = 2;
        float rangeScale = 5.0f;
        float azimuthScale = 360.0f / nAzim;
        float elevAngle = 0.3f;
        int missing = 255;
        float radarHeight = 345.6f;
        float valueOffset = 0.0f;
        float valueScale = 1.0f;
        int[] vradImageInt = readDataFromFile("/home/wbouten/enram/ncradar/test/data/case1/testdata-12x11-pattern-vrad.txt");
        float[] points = new float[nGlobal * nDims];
        float[] yObs = new float[nGlobal];
        int[] c = new int[nGlobal];
        int[] cellImage = new int[nGlobal];
        int nPointsMaxPtr = 0;
        float rangeMin = 0;
        float rangeMax = nRang * rangeScale;
        float HLAYER = 10000.0f;
        float heightInputPar = 100.0f;
        float vradMin = 0.0f;
        int iData = 0;
        int layer = 0;
        int nPointsPtr = 0;

        vvp(nRang, nAzim, rangeScale, azimuthScale, elevAngle, missing, radarHeight, valueOffset,
                valueScale, vradImageInt, points, yObs, c, cellImage, nDims, nPointsMaxPtr,
                rangeMin, rangeMax, HLAYER, heightInputPar, vradMin, iData, layer,
                nPointsPtr);

        // TODO points array is only zeros somehow, even though the JNI
        // counterpart has nonzero elements

        for (int iPoint = 0; iPoint < nPointsMaxPtr; iPoint++) {
            for (int iDim = 0; iDim < nDims; iDim++) {
                System.out.printf("points[%d][%d] = ", iPoint, iDim, points[iPoint * nDims + iDim]);
            }
            System.out.print("\n");
        }

    }
}
