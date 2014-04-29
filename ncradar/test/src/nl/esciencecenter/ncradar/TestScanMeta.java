package nl.esciencecenter.ncradar;

import static org.junit.Assert.*;
import nl.esciencecenter.ncradar.ScanMeta;

import org.junit.Test;
import org.junit.Before;

public class TestScanMeta {
    
    private int date = 942378;
    private int time = 2873423;
    private float radarHeight = 123.4f;
    private float radarElevationAngle = 45.0f;
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
        
        scanMeta = new ScanMeta(date, time, radarHeight, radarElevationAngle, 
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
        
        int expected = date;
        int actual = scanMeta.getDate();
        assertEquals(expected, actual);
    }

    @Test
    public void testGetTime() {
        
        int expected = time;
        int actual = scanMeta.getTime();
        assertEquals(expected, actual);
    }

    @Test
    public void testGetRadarHeight() {

        float expected = radarHeight;
        float actual = scanMeta.getRadarHeight();
        float delta = 0.00001f;
        assertEquals(expected, actual, delta);

    }

    @Test
    public void testGetElevationAngle() {

        float expected = radarElevationAngle;
        float actual = scanMeta.getRadarElevationAngle();
        float delta = 0.00001f;
        assertEquals(expected, actual, delta);

    }

    @Test
    public void testGetNumberOfRangeBins() {

        int expected = numberOfRangeBins;
        int actual = scanMeta.getNumberOfRangeBins();
        assertEquals(expected, actual);

    }

    @Test
    public void testGetNumberOfAzimuthBins() {

        int expected = numberOfAzimuthBins;
        int actual = scanMeta.getNumberOfAzimuthBins();
        assertEquals(expected, actual);

    }

    @Test
    public void testGetBinSizeRange() {

        float expected = binSizeRange;
        float actual = scanMeta.getBinSizeRange();
        float delta = 0.00001f;
        assertEquals(expected, actual, delta);

    }

    @Test
    public void testGetBinSizeAzimuth() {

        float expected = binSizeAzimuth;
        float actual = scanMeta.getBinSizeAzimuth();
        float delta = 0.00001f;
        assertEquals(expected, actual, delta);

    }

    @Test
    public void testGetiAzimFirstRay() {
        
        int expected = iAzimFirstRay;
        int actual = scanMeta.getiAzimFirstRay();
        assertEquals(expected, actual);

    }

    @Test
    public void testGetValueOffset() {

        float expected = valueOffset;
        float actual = scanMeta.getValueOffset();
        float delta = 0.00001f;
        assertEquals(expected, actual, delta);

        
    }

    @Test
    public void testGetValueScale() {

        float expected = valueScale;
        float actual = scanMeta.getValueScale();
        float delta = 0.00001f;
        assertEquals(expected, actual, delta);

    }

    @Test
    public void testGetMissingValueValue() {

        int expected = missingValueValue;
        int actual = scanMeta.getMissingValueValue();
        assertEquals(expected, actual);

    }

    @Test
    public void testGetPulseRepeatFrequencyHigh() {

        float expected = pulseRepeatFrequencyHigh;
        float actual = scanMeta.getPulseRepeatFrequencyHigh();
        float delta = 0.00001f;
        assertEquals(expected, actual, delta);

    }

    @Test
    public void testGetPulseRepeatFrequencyLow() {
        
        float expected = pulseRepeatFrequencyLow;
        float actual = scanMeta.getPulseRepeatFrequencyLow();
        float delta = 0.00001f;
        assertEquals(expected, actual, delta);

    }

    @Test
    public void testGetPulseLength() {

        float expected = pulseLength;
        float actual = scanMeta.getPulseLength();
        float delta = 0.00001f;
        assertEquals(expected, actual, delta);

    }

    @Test
    public void testGetRadarConstant() {

        float expected = radarConstant;
        float actual = scanMeta.getRadarConstant();
        float delta = 0.00001f;
        assertEquals(expected, actual, delta);

    }

    @Test
    public void testGetNominalMaxTransmissionPower() {

        float expected = nominalMaxTransmissionPower;
        float actual = scanMeta.getNominalMaxTransmissionPower();
        float delta = 0.00001f;
        assertEquals(expected, actual, delta);

    }

    @Test
    public void testGetRadialVelocityAntenna() {
        
        float expected = radialVelocityAntenna;
        float actual = scanMeta.getRadialVelocityAntenna();
        float delta = 0.00001f;
        assertEquals(expected, actual, delta);

        
    }

}
