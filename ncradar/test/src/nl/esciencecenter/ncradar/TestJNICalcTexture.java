package nl.esciencecenter.ncradar;

import static org.junit.Assert.assertEquals;
import org.junit.Before;
import org.junit.Test;


public class TestJNICalcTexture extends JNIMethodsVol2Bird {

    private byte[] texImage;
    private byte[] reflImage;
    private byte[] vradImage;
    private byte nRangNeighborhood;
    private byte nAzimNeighborhood;
    private byte nCountMin;
    private byte texType;
    private float texOffset;
    private float texScale;
    private float reflOffset;
    private float reflScale;
    private float vradOffset;
    private float vradScale;
    private int nRang;
    private int nAzim;



    @Before
    public void setUp() throws Exception {

        texImage = new byte[] { 0, 0, 0, 0,
                0, 0, 0, 0,
                0, 0, 0, 0,
                0, 0, 0, 0,
                0, 0, 0, 0 };
        reflImage = new byte[] { 3, 46, 56, 7,
                84, 6, 78, 72,
                3, 5, 23, 56,
                67, 23, 5, 67,
                89, 7, 2, 5 };
        vradImage = new byte[] { 8, 94, 56, 65,
                23, 2, 46, 73,
                5, 6, 34, 53,
                0, 89, 42, 3,
                92, 5, 9, 79 };
        nRangNeighborhood = 3;
        nAzimNeighborhood = 3;
        nCountMin = 0;
        texType = 2; // 2 = STDDEV of VRAD
        texOffset = 0.0f;
        texScale = 1.0f;
        reflOffset = 2.0f;
        reflScale = -34.0f;
        vradOffset = 0.0f;
        vradScale = 1.0f;
        nRang = 4;
        nAzim = 5;

    }



    @Test
    public void testNativeCalcTexture1() throws Exception {

        // this test reproduces calculation of standard deviation as used
        // in the C code (which is not obviously correct)

        calcTexture(texImage,
                reflImage,
                vradImage,
                nRangNeighborhood,
                nAzimNeighborhood,
                nCountMin,
                texType,
                texOffset,
                texScale,
                reflOffset,
                reflScale,
                vradOffset,
                vradScale,
                nRang,
                nAzim);

        byte[][] actual = reshapeTo2D(texImage.clone(), nAzim, nRang);

        byte[][] texImage2D = new byte[nAzim][nRang];
        byte[][] vradImage2D = reshapeTo2D(vradImage, nAzim, nRang);

        for (int iAzim = 0; iAzim < nAzim; iAzim++) {
            for (int iRang = 0 + nRangNeighborhood / 2; iRang < nRang - nRangNeighborhood / 2; iRang++) {

                double vmoment1 = 0;
                double vmoment2 = 0;
                float vRadDiff;

                for (int iAzimNeighborhood = 0; iAzimNeighborhood < nAzimNeighborhood; iAzimNeighborhood++) {
                    for (int iRangNeighborhood = 0; iRangNeighborhood < nRangNeighborhood; iRangNeighborhood++) {

                        int iAzimDelta = iAzimNeighborhood - (nAzimNeighborhood / 2);
                        int iRangDelta = iRangNeighborhood - (nRangNeighborhood / 2);
                        int iAzimLocal = (nAzim + iAzim + iAzimDelta) % nAzim;
                        int iRangLocal = iRang + iRangDelta;

                        vRadDiff = vradOffset + vradScale *
                                (vradImage2D[iAzim][iRang] - vradImage2D[iAzimLocal][iRangLocal]);

                        vmoment1 += vRadDiff;
                        vmoment2 += Math.pow(vRadDiff, 2);

                    }
                }

                vmoment1 /= (nAzimNeighborhood * nRangNeighborhood);
                vmoment2 /= (nAzimNeighborhood * nRangNeighborhood);

                double tex = Math.sqrt(Math.abs(vmoment2 - Math.pow(vmoment1, 2)));
                texImage2D[iAzim][iRang] = (byte) Math.round((tex - texOffset) / texScale);

            }
        }

        byte[][] expected = texImage2D.clone();

        int differsByOne = 0;
        for (int iAzim = 0; iAzim < nAzim; iAzim++) {
            for (int iRang = 1; iRang < nRang - 1; iRang++) {

                assertEquals(expected[iAzim][iRang], actual[iAzim][iRang], 1);

                if (expected[iAzim][iRang] != actual[iAzim][iRang]) {

                    differsByOne += 1;

                }

            }
        }

        System.out.println("A total of " + differsByOne + " array entries differed by 1 from the C result.");

    }



    @Test
    public void testNativeCalcTexture2() throws Exception {

        // this test should reproduce calculation of standard deviation

        calcTexture(texImage,
                reflImage,
                vradImage,
                nRangNeighborhood,
                nAzimNeighborhood,
                nCountMin,
                texType,
                texOffset,
                texScale,
                reflOffset,
                reflScale,
                vradOffset,
                vradScale,
                nRang,
                nAzim);

        byte[][] actual = reshapeTo2D(texImage.clone(), nAzim, nRang);

        byte[][] texImage2D = new byte[nAzim][nRang];
        byte[][] vradImage2D = reshapeTo2D(vradImage, nAzim, nRang);

        for (int iAzim = 0; iAzim < nAzim; iAzim++) {
            for (int iRang = 0 + nRangNeighborhood / 2; iRang < nRang - nRangNeighborhood / 2; iRang++) {

                int iLocal = 0;
                double[] neighborhoodData = new double[nAzimNeighborhood * nRangNeighborhood];

                for (int iAzimNeighborhood = 0; iAzimNeighborhood < nAzimNeighborhood; iAzimNeighborhood++) {
                    for (int iRangNeighborhood = 0; iRangNeighborhood < nRangNeighborhood; iRangNeighborhood++) {

                        int iAzimDelta = iAzimNeighborhood - (nAzimNeighborhood / 2);
                        int iRangDelta = iRangNeighborhood - (nRangNeighborhood / 2);
                        int iAzimLocal = (nAzim + iAzim + iAzimDelta) % nAzim;
                        int iRangLocal = iRang + iRangDelta;

                        neighborhoodData[iLocal] = vradOffset + vradScale * vradImage2D[iAzimLocal][iRangLocal];
                        iLocal += 1;

                    }
                }

                double stdDev = calcStdDev(neighborhoodData);

                texImage2D[iAzim][iRang] = (byte) Math.round(((stdDev - texOffset) / texScale));

            }
        }

        byte[][] expected = texImage2D.clone();

        int differsByOne = 0;
        for (int iAzim = 0; iAzim < nAzim; iAzim++) {
            for (int iRang = 1; iRang < nRang - 1; iRang++) {

                assertEquals(expected[iAzim][iRang], actual[iAzim][iRang], 1);

                if (expected[iAzim][iRang] != actual[iAzim][iRang]) {
                    differsByOne += 1;
                }
            }
        }

        System.out.println("A total of " + differsByOne + " array entries differed by 1 from the C result.");

    }



    private byte[][] reshapeTo2D(byte[] inputArray, int nRows, int nCols) {

        byte[][] outputArray = new byte[nRows][nCols];
        int iGlobal = 0;
        for (int iRow = 0; iRow < nRows; iRow++) {
            for (int iCol = 0; iCol < nCols; iCol++) {

                outputArray[iRow][iCol] = inputArray[iGlobal];
                iGlobal += 1;

            }
        }

        return outputArray;

    }



    private double calcSum(double[] array) {

        int nSamples = array.length;
        double sum = 0;

        for (int iSample = 0; iSample < nSamples; iSample++) {
            sum += array[iSample];
        }
        return sum;

    }



    private double calcMean(double[] array) {

        int nSamples = array.length;
        return calcSum(array) / nSamples;
    }



    private double calcStdDev(double[] array) {

        int nSamples = array.length;
        double[] squaredDevs = new double[nSamples];

        double mean = calcMean(array);

        for (int iSample = 0; iSample < nSamples; iSample++) {
            squaredDevs[iSample] = Math.pow(array[iSample] - mean, 2);
        }

        double stdDev = Math.sqrt(calcMean(squaredDevs));
        return stdDev;

    }
}
