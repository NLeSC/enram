package nl.esciencecenter.ncradar;

import static org.junit.Assert.*;
import nl.esciencecenter.ncradar.ScanMeta;

import org.junit.Test;
import org.junit.Before;

public class TestScanMeta {
    
    private int date = 942378;
    private int time = 2873423;
    private float radarHeight = 123.4f;
    private float elevationAngle = 45.0f;
    private int numberOfRangeBins = 12;
    private int numberOfAzimuthBins = 15;
    private float binSizeRange = 67.8f;
    private float binSizeAzimuth = 34.5f;
    private int iAzimFirstRay = 5;
    private float valueOffset = 9623.6f;
    private float valueScale = 535.4f;
    private int missingValueValue = 255;
    private float pulseRepeatFrequencyHigh = 737.2f;
    private float pulseRepeatFrequencyLow = 30.4f;
    private float pulseLength = 863.4f;
    private float radarConstant = 2578.6f;
    private float nominalMaxTransmissionPower = 85637.5f;
    private float radialVelocityAntenna = 678.2f;
    private ScanMeta scanMeta;
    
    @Before
    public void setUp() {
        
        scanMeta = new ScanMeta(date, time, radarHeight, elevationAngle, 
                numberOfRangeBins, numberOfAzimuthBins, binSizeRange, 
                binSizeAzimuth, iAzimFirstRay, valueOffset, 
                valueScale, missingValueValue, pulseRepeatFrequencyHigh,
                pulseRepeatFrequencyLow, pulseLength, radarConstant, 
                nominalMaxTransmissionPower, radialVelocityAntenna);
        
    }
    
    @Test
    public void testScanMeta() {
        fail("Not yet implemented");
    }

    @Test
    public void testGetDate() {
        
        int expected = 942378;
        int actual = scanMeta.getDate();
        assertEquals(expected, actual);
    }

    @Test
    public void testGetTime() {
        fail("Not yet implemented");
    }

    @Test
    public void testGetRadarHeight() {
        fail("Not yet implemented");
    }

    @Test
    public void testGetElevationAngle() {
        fail("Not yet implemented");
    }

    @Test
    public void testGetNumberOfRangeBins() {
        fail("Not yet implemented");
    }

    @Test
    public void testGetNumberOfAzimuthBins() {
        fail("Not yet implemented");
    }

    @Test
    public void testGetBinSizeRange() {
        fail("Not yet implemented");
    }

    @Test
    public void testGetBinSizeAzimuth() {
        fail("Not yet implemented");
    }

    @Test
    public void testGetiAzimFirstRay() {
        fail("Not yet implemented");
    }

    @Test
    public void testGetValueOffset() {
        fail("Not yet implemented");
    }

    @Test
    public void testGetValueScale() {
        fail("Not yet implemented");
    }

    @Test
    public void testGetMissingValueValue() {
        fail("Not yet implemented");
    }

    @Test
    public void testGetPulseRepeatFrequencyHigh() {
        fail("Not yet implemented");
    }

    @Test
    public void testGetPulseRepeatFrequencyLow() {
        fail("Not yet implemented");
    }

    @Test
    public void testGetPulseLength() {
        fail("Not yet implemented");
    }

    @Test
    public void testGetRadarConstant() {
        fail("Not yet implemented");
    }

    @Test
    public void testGetNominalMaxTransmissionPower() {
        fail("Not yet implemented");
    }

    @Test
    public void testGetRadialVelocityAntenna() {
        fail("Not yet implemented");
    }

}
