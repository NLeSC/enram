package nl.esciencecenter.ncradar;

import static org.junit.Assert.assertEquals;
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

        String startDir = System.getProperty("user.dir");
        // System.err.println(startDir);

        datasetIndex = 3;
        directory = startDir + "/test" + "/data/";
        filename = "T_PAGZ60_C_OKPR_20110815000447.hdf";
        rs = new RadarScanJava(directory, filename, datasetIndex);
        delta = 0.0000001;
    }



    @Test
    public void testGetDatasetIndex() {

        int expected = datasetIndex;
        int actual = rs.getDatasetIndex();
        assertEquals(expected, actual);
    }



    @Test
    public void testGetDatasetName() {

        String expected = "dataset4";
        String actual = rs.getDatasetName();
        assertEquals(expected, actual);
    }



    @Test
    public void testGetDirectory() {

        String expected = "/home/wbouten/tmp";
        String actual = rs.getDirectory();
        assertEquals(expected, actual);
    }



    @Test
    public void testGetElevationAngle() {

        double expected = 1.3;
        double actual = rs.getElevationAngle();
        assertEquals(expected, actual, delta);
    }



    @Test
    public void testGetEndDate() {

        String expected = "20110815";
        String actual = rs.getEndDate();
        assertEquals(expected, actual);
    }



    @Test
    public void testGetEndTime() {

        String expected = "000333";
        String actual = rs.getEndTime();
        assertEquals(expected, actual);
    }



    @Test
    public void testGetFilename() {

        String expected = "T_PAGZ60_C_OKPR_20110815000447.hdf";
        String actual = rs.getFilename();
        assertEquals(expected, actual);

    }



    @Test
    public void testGetGain() {

        double expected = 0.5;
        double actual = rs.getDataScale();
        assertEquals(expected, actual, delta);
    }



    @Test
    public void testGetMissingValue() {

        int expected = 255;
        int actual = rs.getMissingValueValue(); // FIXME data type error?
        assertEquals(expected, actual);

    }



    @Test
    public void testGetNumberOfAzimuthBins() {

        long expected = 360L;
        long actual = rs.getNumberOfAzimuthBins();
        assertEquals(expected, actual);
    }



    @Test
    public void testGetNumberOfRangeBins() {

        long expected = 520L;
        long actual = rs.getNumberOfRangeBins();
        assertEquals(expected, actual);
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

        long expected = 238L;
        long actual = rs.getiAzimFirstRay();
        assertEquals(expected, actual);
    }



    @Test
    public void testGetScanType() {

        String expected = "DBZH";
        String actual = rs.getScanType();
        assertEquals(expected, actual);
    }



    @Test
    public void testGetStartDate() {

        String expected = "20110815";
        String actual = rs.getStartDate();
        assertEquals(expected, actual);
    }



    @Test
    public void testGetStartTime() {

        String expected = "000311";
        String actual = rs.getStartTime();
        assertEquals(expected, actual);
    }



    @Test
    public void testGetVerticesAndFaces() {

        rs.calcVerticesAndFaces();
        int[][] actualFaces = rs.getFaces();
        int[][] expectedFaces = { { 0, 1, 361 }, { 0, 361, 360 } };

        int nFaces = expectedFaces.length;

        for (int iFace = 0; iFace < nFaces; iFace++) {
            for (int iVert = 0; iVert < 3; iVert++) {
                assertEquals(expectedFaces[iFace][iVert], actualFaces[iFace][iVert]);
            }
        }

        // TODO
        // double[][] actualVertices = rs.getVertices();
        // double[][] expectedVertices = { {}, {} };
        //
        // int nVerts = 4;
        //
        // for (int iVert = 0; iVert < nVerts; iVert++) {
        // assertEquals(expectedVertices[iVert], actualVertices[iVert]);
        // }
    }
}
