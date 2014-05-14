package nl.esciencecenter.ncradar;

import static org.junit.Assert.*;

import java.io.IOException;

import org.junit.Before;
import org.junit.Test;

public class TestRadarScanJava {

    private RadarScanJava rs;
    private int datasetIndex;
    private String directory;
    private String filename;
    private double delta;
    
    @Before
    public void setUp() throws IOException {
        
        datasetIndex = 3;
        directory = "/home/wbouten/tmp";
        filename = "T_PAGZ60_C_OKPR_20110815000447.hdf";
        rs = new RadarScanJava(directory,filename,datasetIndex);
        delta = 0.0000001;
    }    
    
    @Test
    public void testCalcPolygons() {
        fail("Not yet implemented");
    }

    @Test
    public void testCalcVerticesAndFaces() {
        
        int[][] faces;
        double[][] vertices;
        double[] facesValues;
        
        rs.calcVerticesAndFaces();
        faces = rs.getFaces();
        vertices = rs.getVertices();
        facesValues = rs.getFacesValues();
        
        
        fail("Not yet implemented");
    }

    @Test
    public void testConvertScanData() {
        fail("Not yet implemented");
    }

    @Test
    public void testGetDatasetIndex() {
        
        int expected = datasetIndex;
        int actual = rs.getDatasetIndex();
        assertEquals(expected,actual);
    }

    @Test
    public void testGetDatasetName() {
        String expected = "dataset4";
        String actual = rs.getDatasetName();
        assertEquals(expected,actual);
    }

    @Test
    public void testGetDirectory() {
        String expected = "/home/wbouten/tmp";
        String actual = rs.getDirectory();
        assertEquals(expected,actual);
    }

    @Test
    public void testGetElevationAngle() {
        double expected = 1.3;
        double actual = rs.getElevationAngle();
        assertEquals(expected,actual,delta);
    }

    @Test
    public void testGetEndDate() {
        String expected = "20110815";
        String actual = rs.getEndDate();
        assertEquals(expected,actual);
    }

    @Test
    public void testGetEndTime() {
        String expected = "000333";
        String actual = rs.getEndTime();
        assertEquals(expected,actual);
    }

    @Test
    public void testGetFaces() {
        fail("Not yet implemented");
    }

    @Test
    public void testGetFilename() {
        String expected = "T_PAGZ60_C_OKPR_20110815000447.hdf";
        String actual = rs.getFilename();
        assertEquals(expected,actual);

    }

    @Test
    public void testGetGain() {
        double expected = 0.5;
        double actual = rs.getGain();
        assertEquals(expected,actual,delta);
    }

    @Test
    public void testGetMissingValue() {
        int expected = 255;
        int actual = rs.getMissingValue();
        assertEquals(expected,actual);

    }

    @Test
    public void testGetNumberOfAzimuthBins() {
        long expected = 360L;
        long actual = rs.getNumberOfAzimuthBins();
        assertEquals(expected,actual);
    }

    @Test
    public void testGetNumberOfRangeBins() {
        long expected = 520L;
        long actual = rs.getNumberOfRangeBins();
        assertEquals(expected,actual);
    }

    @Test
    public void testGetOffset() {
        double expected = -32.0;
        double actual = rs.getOffset();
        assertEquals(expected,actual,delta);
    }

    @Test
    public void testGetPolygons() {
        fail("Not yet implemented");
    }

    @Test
    public void testGetRadarPositionHeight() {
        double expected = 916.0;
        double actual = rs.getRadarPositionHeight();
        assertEquals(expected,actual,delta);
    }

    @Test
    public void testGetRadarPositionLatitude() {
        double expected = 49.6583;
        double actual = rs.getRadarPositionLatitude();
        assertEquals(expected,actual,delta);
    }

    @Test
    public void testGetRadarPositionLongitude() {
        double expected = 13.8178;
        double actual = rs.getRadarPositionLongitude();
        assertEquals(expected,actual,delta);
    }

    @Test
    public void testGetScanData() {
        fail("Not yet implemented");
    }

    @Test
    public void testGetScanType() {
        String expected = "DBZH";
        String actual = rs.getScanType();
        assertEquals(expected,actual);
    }

    @Test
    public void testGetStartDate() {
        String expected = "20110815";
        String actual = rs.getStartDate();
        assertEquals(expected,actual);
    }

    @Test
    public void testGetStartTime() {
        String expected = "000311";
        String actual = rs.getStartTime();
        assertEquals(expected,actual);
    }

    @Test
    public void testGetVertices() {
        fail("Not yet implemented");
    }

    @Test
    public void testPrintAsGeoJSONToCSV() {
        fail("Not yet implemented");
    }

    @Test
    public void testPrintAsGeoJSONToCSVString() {
        fail("Not yet implemented");
    }

    @Test
    public void testPrintAsWKTToCSV() {
        fail("Not yet implemented");
    }

    @Test
    public void testPrintAsWKTToCSVString() {
        fail("Not yet implemented");
    }

    @Test
    public void testRadarScan() {
        fail("Not yet implemented");
    }

}
