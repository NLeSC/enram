package nl.esciencecenter.ncradar;

import static org.junit.Assert.*;

import java.io.IOException;

import org.junit.Before;
import org.junit.Test;

public class TestRadarScanMatlab {

    private RadarScanMatlab rs;
    
    @Before
    public void setUp() throws IOException {
        rs = new RadarScanMatlab("/home/wbouten/tmp","T_PAGZ60_C_OKPR_20110815000447.hdf",3);
    }    

    @Test
    public void testGetDatasetName() {

        rs.getDatasetName();
        
        fail("Not yet implemented");
    }

}
