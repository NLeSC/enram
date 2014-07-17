package nl.esciencecenter.ncradar;

import static org.junit.Assert.assertEquals;
import org.junit.Test;


public class TestJNIFindNearbyGateIndex extends JNIMethodsVol2Bird {

    @Test
    public void testNativeFindNearbyGateIndex1() {

        // a 3x5 child array on top of a 6x7 parent array, upper left corner

        int nAzimParent = 6;
        int nRangParent = 7;
        int iParent = 0;

        int nAzimChild = 3;
        int nRangChild = 5;
        int iChild;
        int[] expected = { -5, -5, 35, 36, 37, -5, -5, 0, 1, 2, -5, -5, 7, 8, 9 };

        int actual;

        for (iChild = 0; iChild < nAzimChild * nRangChild; iChild++) {

            actual = findNearbyGateIndex(nAzimParent, nRangParent, iParent, nAzimChild, nRangChild, iChild);

            assertEquals(expected[iChild], actual);

        }

    }



    @Test
    public void testNativeFindNearbyGateIndex2() {

        // a 3x5 child array on top of a 6x7 parent array, upper right corner

        int nAzimParent = 6;
        int nRangParent = 7;
        int iParent = 6;

        int nAzimChild = 3;
        int nRangChild = 5;
        int iChild;
        int[] expected = { 39, 40, 41, -4, -4, 4, 5, 6, -4, -4, 11, 12, 13, -4, -4 };

        int actual;

        for (iChild = 0; iChild < nAzimChild * nRangChild; iChild++) {

            actual = findNearbyGateIndex(nAzimParent, nRangParent, iParent, nAzimChild, nRangChild, iChild);

            assertEquals(expected[iChild], actual);

        }

    }



    @Test
    public void testNativeFindNearbyGateIndex3() {

        // a 3x5 child array on top of a 6x7 parent array, lower left corner

        int nAzimParent = 6;
        int nRangParent = 7;
        int iParent = 35;

        int nAzimChild = 3;
        int nRangChild = 5;
        int iChild;
        int[] expected = { -5, -5, 28, 29, 30, -5, -5, 35, 36, 37, -5, -5, 0, 1, 2 };

        int actual;

        for (iChild = 0; iChild < nAzimChild * nRangChild; iChild++) {

            actual = findNearbyGateIndex(nAzimParent, nRangParent, iParent, nAzimChild, nRangChild, iChild);

            assertEquals(expected[iChild], actual);

        }

    }



    @Test
    public void testNativeFindNearbyGateIndex4() {

        // a 3x5 child array on top of a 6x7 parent array, lower right corner

        int nAzimParent = 6;
        int nRangParent = 7;
        int iParent = 41;

        int nAzimChild = 3;
        int nRangChild = 5;
        int iChild;
        int[] expected = { 32, 33, 34, -4, -4, 39, 40, 41, -4, -4, 4, 5, 6, -4, -4 };

        int actual;

        for (iChild = 0; iChild < nAzimChild * nRangChild; iChild++) {

            actual = findNearbyGateIndex(nAzimParent, nRangParent, iParent, nAzimChild, nRangChild, iChild);

            assertEquals(expected[iChild], actual);

        }

    }



    @Test
    public void testNativeFindNearbyGateIndex5() {

        // a 5x3 child array on top of a 6x7 parent array, upper left corner

        int nAzimParent = 6;
        int nRangParent = 7;
        int iParent = 0;

        int nAzimChild = 5;
        int nRangChild = 3;
        int iChild;
        int[] expected = { -5, 28, 29, -5, 35, 36, -5, 0, 1, -5, 7, 8, -5, 14, 15 };

        int actual;

        for (iChild = 0; iChild < nAzimChild * nRangChild; iChild++) {

            actual = findNearbyGateIndex(nAzimParent, nRangParent, iParent, nAzimChild, nRangChild, iChild);

            assertEquals(expected[iChild], actual);

        }

    }



    @Test
    public void testNativeFindNearbyGateIndex6() {

        // a 5x3 child array on top of a 6x7 parent array, upper right corner

        int nAzimParent = 6;
        int nRangParent = 7;
        int iParent = 6;

        int nAzimChild = 5;
        int nRangChild = 3;
        int iChild;
        int[] expected = { 33, 34, -4, 40, 41, -4, 5, 6, -4, 12, 13, -4, 19, 20, -4 };

        int actual;

        for (iChild = 0; iChild < nAzimChild * nRangChild; iChild++) {

            actual = findNearbyGateIndex(nAzimParent, nRangParent, iParent, nAzimChild, nRangChild, iChild);

            assertEquals(expected[iChild], actual);

        }

    }



    @Test
    public void testNativeFindNearbyGateIndex7() {

        // a 5x3 child array on top of a 6x7 parent array, lower left corner

        int nAzimParent = 6;
        int nRangParent = 7;
        int iParent = 35;

        int nAzimChild = 5;
        int nRangChild = 3;
        int iChild;
        int[] expected = { -5, 21, 22, -5, 28, 29, -5, 35, 36, -5, 0, 1, -5, 7, 8 };

        int actual;

        for (iChild = 0; iChild < nAzimChild * nRangChild; iChild++) {

            actual = findNearbyGateIndex(nAzimParent, nRangParent, iParent, nAzimChild, nRangChild, iChild);

            assertEquals(expected[iChild], actual);

        }

    }



    @Test
    public void testNativeFindNearbyGateIndex8() {

        // a 5x3 child array on top of a 6x7 parent array, lower right corner

        int nAzimParent = 6;
        int nRangParent = 7;
        int iParent = 41;

        int nAzimChild = 5;
        int nRangChild = 3;
        int iChild;
        int[] expected = { 26, 27, -4, 33, 34, -4, 40, 41, -4, 5, 6, -4, 12, 13, -4 };

        int actual;

        for (iChild = 0; iChild < nAzimChild * nRangChild; iChild++) {

            actual = findNearbyGateIndex(nAzimParent, nRangParent, iParent, nAzimChild, nRangChild, iChild);

            assertEquals(expected[iChild], actual);

        }

    }

}
