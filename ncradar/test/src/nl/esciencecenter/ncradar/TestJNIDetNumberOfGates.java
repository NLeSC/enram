package nl.esciencecenter.ncradar;

import static org.junit.Assert.assertEquals;
import org.junit.Test;


public class TestJNIDetNumberOfGates extends JNIMethodsVol2Bird {


    @Test
    public void testNativeDetNumberOfGates1() {


        int nGatesActual;
        int nGatesExpected;
        int iLayer;
        float layerThickness;
        float rangeMin;
        float rangeMax;
        float rangeScale;
        float[] elevAngles;
        int nElevAngles;
        int nRang;
        int nAzim;
        float radarHeight;

        
        iLayer = 0;
        layerThickness = 200.0f;       // m
        rangeMin = 10000.0f;           // m
        rangeMax = 11000.0f;           // m
        rangeScale = 500.0f;           // m
        elevAngles = new float[] {0.0f};    // degrees
        nElevAngles = elevAngles.length;
        
        nRang = 520;
        nAzim = 360;
        radarHeight = 0.0f;      // m

        nGatesActual = detNumberOfGates(iLayer, layerThickness,
                                        rangeMin, rangeMax,
                                        rangeScale, elevAngles, nElevAngles,
                                        nRang, nAzim,
                                        radarHeight);

        nGatesExpected = 2*360;

        assertEquals(nGatesActual, nGatesExpected);

    }


    
    @Test
    public void testNativeDetNumberOfGates2() {


        int nGatesActual;
        int nGatesExpected;
        int iLayer;
        float layerThickness;
        float rangeMin;
        float rangeMax;
        float rangeScale;
        float[] elevAngles;
        int nElevAngles;
        int nRang;
        int nAzim;
        float radarHeight;

        
        iLayer = 10;
        layerThickness = 1000.0f; // m
        rangeScale = (float) Math.sqrt(2.0)*100.0f;      // m
        rangeMin = 100 * rangeScale;     // m
        rangeMax = 110 * rangeScale;     // m

        elevAngles = new float[] {45.0f};       // degrees
        nElevAngles = elevAngles.length;
        nRang = 520;
        nAzim = 360;
        radarHeight = 0.0f;      // m

        nGatesActual = detNumberOfGates(iLayer, layerThickness,
                                        rangeMin, rangeMax,
                                        rangeScale, elevAngles, nElevAngles,
                                        nRang, nAzim,
                                        radarHeight);

        nGatesExpected = 10*360;

        assertEquals(nGatesActual, nGatesExpected);

    }
    
    
    
    @Test
    public void testNativeDetNumberOfGates3() {


        int nGatesActual;
        int nGatesExpected;
        int iLayer;
        float layerThickness;
        float rangeMin;
        float rangeMax;
        float rangeScale;
        float[] elevAngles;
        int nElevAngles;
        int nRang;
        int nAzim;
        float radarHeight;

        
        iLayer = 10;
        layerThickness = 1000.0f;                        // m
        rangeScale = (float) Math.sqrt(2.0)*100.0f;      // m
        rangeMin = 100 * rangeScale;                     // m
        rangeMax = 110 * rangeScale;                     // m

        elevAngles = new float[] {45.0f,45.0f};           // degrees
        nElevAngles = elevAngles.length;
        nRang = 520;
        nAzim = 360;
        radarHeight = 0.0f;                              // m

        nGatesActual = detNumberOfGates(iLayer, layerThickness,
                                        rangeMin, rangeMax,
                                        rangeScale, elevAngles, nElevAngles,
                                        nRang, nAzim,
                                        radarHeight);

        nGatesExpected = nElevAngles * 10 * 360;

        assertEquals(nGatesActual, nGatesExpected);

    }    

    
    
}
