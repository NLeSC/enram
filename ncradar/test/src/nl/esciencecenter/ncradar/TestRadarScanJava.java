package nl.esciencecenter.ncradar;

import static org.junit.Assert.*;

import java.io.IOException;

import org.junit.Before;
import org.junit.Test;

public class TestRadarScanJava {

    private RadarScanJava rs;
    
    @Before
    public void setUp() throws IOException {
        rs = new RadarScanJava("/home/wbouten/tmp","T_PAGZ60_C_OKPR_20110815000447.hdf",3);
    }    
    
    @Test
    public void testRadarScan() {
        fail("Not yet implemented");
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
    public void testPrintAsWKTToCSV() {
        fail("Not yet implemented");
    }

    @Test
    public void testPrintAsWKTToCSVString() {
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
    public void testConvertScanData() {
        fail("Not yet implemented");
    }

    @Test
    public void testGetDirectory() {
        fail("Not yet implemented");
    }

    @Test
    public void testGetFilename() {
        fail("Not yet implemented");
    }

    @Test
    public void testGetNumberOfRangeBins() {
        fail("Not yet implemented");
    }

    @Test
    public void testGetElevationAngle() {
        fail("Not yet implemented");
    }

    @Test
    public void testGetScanData() {
        fail("Not yet implemented");
    }

    @Test
    public void testGetPolygons() {
        fail("Not yet implemented");
    }

    @Test
    public void testGetDatasetIndex() {
        fail("Not yet implemented");
    }

    @Test
    public void testGetScanType() {
        fail("Not yet implemented");
    }

    @Test
    public void testGetStartDate() {
        fail("Not yet implemented");
    }

    @Test
    public void testGetStartTime() {
        fail("Not yet implemented");
    }

    @Test
    public void testGetEndDate() {
        fail("Not yet implemented");
    }

    @Test
    public void testGetEndTime() {
        fail("Not yet implemented");
    }

    @Test
    public void testGetRadarPositionLatitude() {
        fail("Not yet implemented");
    }

    @Test
    public void testGetRadarPositionLongitude() {
        fail("Not yet implemented");
    }

    @Test
    public void testGetDatasetName() {
        fail("Not yet implemented");
    }

    @Test
    public void testGetRadarPositionHeight() {
        fail("Not yet implemented");
    }

    @Test
    public void testGetGain() {
        fail("Not yet implemented");
    }

    @Test
    public void testGetOffset() {
        fail("Not yet implemented");
    }

    @Test
    public void testGetMissingValue() {
        fail("Not yet implemented");
    }

    @Test
    public void testGetNumberOfAzimuthBins() {
        fail("Not yet implemented");
    }

    @Test
    public void testGetFaces() {
        fail("Not yet implemented");
    }

    @Test
    public void testGetVertices() {
        fail("Not yet implemented");
    }

}
