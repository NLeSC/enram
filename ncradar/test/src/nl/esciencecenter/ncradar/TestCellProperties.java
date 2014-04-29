package nl.esciencecenter.ncradar;

import static org.junit.Assert.*;

import org.junit.Test;

public class TestCellProperties {

    @Test(expected=java.lang.NullPointerException.class)
    public void testConstructorWithEmptyInput() {
         
        int[] iRangOfMax = {};
        int[] iAzimOfMax = {};
        float[] dbzAvg = {};
        float[] texAvg = {};
        float[] cv = {};
        float[] area = {};
        float[] clutterArea = {};
        float[] dbzMax = {};
        int[] index = {};
        char[] drop = {};
        
        CellProperties cellProp = new CellProperties(iRangOfMax, iAzimOfMax,
                dbzAvg, texAvg, cv, area, clutterArea, dbzMax, index, drop);
    }

}
