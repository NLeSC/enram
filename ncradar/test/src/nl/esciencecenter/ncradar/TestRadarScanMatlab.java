package nl.esciencecenter.ncradar;

import static org.junit.Assert.assertArrayEquals;
import static org.junit.Assert.assertEquals;
import java.io.IOException;
import org.junit.Before;
import org.junit.Test;


public class TestRadarScanMatlab {

    private RadarScanMatlab rs;
    private int datasetIndex;
    private String directory;
    private String filename;
    private double delta;



    @Before
    public void setUp() throws IOException {

        String userName = System.getProperty("user.name");
        String startDir;

        if (userName.equals("wbouten")) {
            startDir = System.getProperty("user.dir") + "/test";
        }
        else {
            startDir = System.getProperty("user.dir");
        }

        datasetIndex = 3;
        directory = startDir + "/data/";
        filename = "T_PAGZ60_C_OKPR_20110815000447.hdf";
        rs = new RadarScanMatlab(directory, filename, datasetIndex);
        delta = 1e-4;
    }



    @Test
    public void testGetDatasetIndex() {

        double expected = datasetIndex;
        double actual = rs.getDatasetIndex();
        assertEquals(expected, actual, delta);
    }



    @Test
    public void testGetDatasetName() {

        char[][] expected = { { 'd', 'a', 't', 'a', 's', 'e', 't', '4' } };
        char[][] actual = rs.getDatasetName();
        assertArrayEquals(expected, actual);
    }



    @Test
    public void testGetDirectory() {

        int nChars = this.directory.length();
        char[][] expected = new char[1][nChars];

        for (int iChar = 0; iChar < nChars; iChar++) {
            expected[0][iChar] = this.directory.charAt(iChar);
        }
        char[][] actual = rs.getDirectory();
        assertArrayEquals(expected, actual);
    }



    @Test
    public void testGetElevationAngle() {

        double expected = 1.3;
        double actual = rs.getElevationAngle();
        assertEquals(expected, actual, delta);
    }



    @Test
    public void testGetEndDate() {

        char[][] expected = { { '2', '0', '1', '1', '0', '8', '1', '5' } };
        char[][] actual = rs.getEndDate();
        assertArrayEquals(expected, actual);
    }



    @Test
    public void testGetEndTime() {

        char[][] expected = { { '0', '0', '0', '3', '3', '3' } };
        char[][] actual = rs.getEndTime();
        assertArrayEquals(expected, actual);
    }



    @Test
    public void testGetFilename() {

        char[][] expected = { { 'T', '_', 'P', 'A', 'G', 'Z', '6', '0', '_', 'C', '_', 'O', 'K', 'P', 'R', '_', '2', '0', '1', '1', '0', '8', '1', '5', '0', '0', '0', '4', '4', '7', '.', 'h', 'd', 'f' } };
        char[][] actual = rs.getFilename();
        assertArrayEquals(expected, actual);

    }



    @Test
    public void testGetGain() {

        double expected = 0.5;
        double actual = rs.getDataScale();
        assertEquals(expected, actual, delta);
    }



    @Test
    public void testGetMissingValueValue() {

        double expected = 255.0;
        double actual = rs.getMissingValueValue();
        assertEquals(expected, actual, delta);

    }



    @Test
    public void testGetNumberOfAzimuthBins() {

        double expected = 360.0;
        double actual = rs.getNumberOfAzimuthBins();
        assertEquals(expected, actual, delta);
    }



    @Test
    public void testGetNumberOfRangeBins() {

        double expected = 520;
        double actual = rs.getNumberOfRangeBins();
        assertEquals(expected, actual, delta);
    }



    @Test
    public void testGetOffset() {

        double expected = -32.0;
        double actual = rs.getDataOffset();
        assertEquals(expected, actual, delta);
    }



    @Test
    public void testGetRadarPositionHeight() {

        double expected = 916.0;
        double actual = rs.getRadarPositionHeight();
        assertEquals(expected, actual, delta);
    }



    @Test
    public void testGetRadarPositionLatitude() {

        double expected = 49.6583;
        double actual = rs.getRadarPositionLatitude();
        assertEquals(expected, actual, delta);
    }



    @Test
    public void testGetRadarPositionLongitude() {

        double expected = 13.8178;
        double actual = rs.getRadarPositionLongitude();
        assertEquals(expected, actual, delta);
    }



    @Test
    public void testGetRangeOffset() {

        double expected = 0.0;
        double actual = rs.getRangeOffset();
        assertEquals(expected, actual, delta);
    }



    @Test
    public void testGetRangeScale() {

        double expected = 500.0;
        double actual = rs.getRangeScale();
        assertEquals(expected, actual, delta);
    }



    @Test
    public void testGetScanStartsAtAzimuthBins() {

        double expected = 238.0;
        double actual = rs.getiAzimFirstRay();
        assertEquals(expected, actual, delta);
    }



    @Test
    public void testGetScanType() {

        char[][] expected = { { 'D', 'B', 'Z', 'H' } };
        char[][] actual = rs.getScanType();
        assertArrayEquals(expected, actual);
    }



    @Test
    public void testGetStartDate() {

        char[][] expected = { { '2', '0', '1', '1', '0', '8', '1', '5' } };
        char[][] actual = rs.getStartDate();
        assertArrayEquals(expected, actual);
    }



    @Test
    public void testGetStartTime() {

        char[][] expected = { { '0', '0', '0', '3', '1', '1' } };
        char[][] actual = rs.getStartTime();
        assertArrayEquals(expected, actual);
    }

}
