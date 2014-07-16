package nl.esciencecenter.ncradar;

import static org.junit.Assert.assertEquals;
import org.junit.Test;


public class TestJNIDist extends JNIMethodsVol2Bird {

    double delta = 0.0001;



    @Test
    public void testNativeDist2() {

        // test the same input numbers as in test 1, but now compare to how I
        // think the
        // distance calculation in polar coordinates is done.

        int iRang1 = 10;
        int iAzim1 = 90;
        int iRang2 = 20;
        int iAzim2 = iAzim1 + 180;

        float rangeScale = 5.0f; // km per gate
        float azimuthScaleDeg = 1.0f; // degree per gate

        float theDist;

        theDist = calcDist(iRang1, iAzim1, iRang2, iAzim2, rangeScale, azimuthScaleDeg);

        float range1 = iRang1 * rangeScale;
        float range2 = iRang2 * rangeScale;

        float DEG2RAD = (float) (2 * Math.PI / 360);

        float azimuth1 = iAzim1 * azimuthScaleDeg * DEG2RAD;
        float azimuth2 = iAzim2 * azimuthScaleDeg * DEG2RAD;

        double expected = Math.sqrt(Math.pow(range1, 2) +
                Math.pow(range2, 2) -
                2 * (range1 * range2) * Math.cos(azimuth1 - azimuth2));

        double actual = theDist;

        assertEquals(expected, actual, delta);

    }



    @Test
    public void testNativeDist3() {

        // this test uses the new method of calculating the distance in a
        // polar plot for a different set of input numbers than test2

        int iRang1 = 10;
        int iAzim1 = 0;
        int iRang2 = 10;
        int iAzim2 = iAzim1 + 90;

        float rangeScale = 5.0f; // km per gate
        float azimuthScaleDeg = 1.0f; // degree per gate

        float theDist;

        theDist = calcDist(iRang1, iAzim1, iRang2, iAzim2, rangeScale, azimuthScaleDeg);

        float range1 = iRang1 * rangeScale;
        float range2 = iRang2 * rangeScale;

        float DEG2RAD = (float) (2 * Math.PI / 360);

        float azimuth1 = iAzim1 * azimuthScaleDeg * DEG2RAD;
        float azimuth2 = iAzim2 * azimuthScaleDeg * DEG2RAD;

        double expected = Math.sqrt(Math.pow(range1, 2) +
                Math.pow(range2, 2) -
                2 * (range1 * range2) * Math.cos(azimuth1 - azimuth2));

        double actual = theDist;

        assertEquals(expected, actual, delta);

    }



    @Test
    public void testNativeDist4() {

        // this test uses the new method of calculating the distance in a
        // polar plot for a different set of input numbers than test2 and 3
        // also, touching a different quadrant of the unit circle

        int iRang1 = 10;
        int iAzim1 = 180 + 45;
        int iRang2 = 10;
        int iAzim2 = iAzim1 + 90;

        float rangeScale = 5.0f; // km per gate
        float azimuthScaleDeg = 1.0f; // degree per gate

        float theDist;

        theDist = calcDist(iRang1, iAzim1, iRang2, iAzim2, rangeScale, azimuthScaleDeg);

        float range1 = iRang1 * rangeScale;
        float range2 = iRang2 * rangeScale;

        float DEG2RAD = (float) (2 * Math.PI / 360);

        float azimuth1 = iAzim1 * azimuthScaleDeg * DEG2RAD;
        float azimuth2 = iAzim2 * azimuthScaleDeg * DEG2RAD;

        double expected = Math.sqrt(Math.pow(range1, 2) +
                Math.pow(range2, 2) -
                2 * (range1 * range2) * Math.cos(azimuth1 - azimuth2));

        double actual = theDist;

        assertEquals(expected, actual, delta);

    }



    @Test
    public void testNativeDist5() {

        // this test uses the new method of calculating the distance in a
        // polar plot for a different set of input numbers than test4

        int iRang1 = 10;
        int iAzim1 = (int) (Math.round(Math.random() * 360));
        int iRang2 = 10;
        int iAzim2 = iAzim1 + 90;

        float rangeScale = 5.0f; // km per gate
        float azimuthScaleDeg = 1.0f; // degree per gate

        float theDist;

        theDist = calcDist(iRang1, iAzim1, iRang2, iAzim2, rangeScale, azimuthScaleDeg);

        float range1 = iRang1 * rangeScale;
        float range2 = iRang2 * rangeScale;

        float DEG2RAD = (float) (2 * Math.PI / 360);

        float azimuth1 = iAzim1 * azimuthScaleDeg * DEG2RAD;
        float azimuth2 = iAzim2 * azimuthScaleDeg * DEG2RAD;

        double expected = Math.sqrt(Math.pow(range1, 2) +
                Math.pow(range2, 2) -
                2 * (range1 * range2) * Math.cos(azimuth1 - azimuth2));

        double actual = theDist;

        assertEquals(expected, actual, delta);

    }

}
