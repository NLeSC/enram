package nl.esciencecenter.ncradar;

import static org.junit.Assert.*;

import org.junit.Test;
import org.junit.Before;

public class TestCellProperties {

    
    private int[] iRangOfMax = {12,56,176};
    private int[] iAzimOfMax = {6,119,243};
    private float[] dbzAvg = {89663.5f,2342.536f,863.33f};
    private float[] texAvg = {345.78f,1223.54f,8979.2f};
    private float[] cv = {89435.543f,232.5f,974.23f};
    private float[] area = {456f,323.64f,6776.32f};
    private float[] clutterArea = {78f,234.5f,232.3f};
    private float[] dbzMax = {9837.2f,53.67f,8765.23f};
    private int[] index = {2,7,96};
    private char[] drop = {0,0,1};
    private CellProperties cellProp;

    
    @Before
    public void setUp() throws Exception  {
        
        cellProp = new CellProperties(iRangOfMax, iAzimOfMax, dbzAvg, texAvg,
                                      cv, area, clutterArea, dbzMax, index, drop);
        
    }

    @Test
    public void testGetiRangOfMax(){


        int nCells = iRangOfMax.length;
        
        for (int iCell = 0; iCell < nCells; iCell++) {
            
            int expected = iRangOfMax[iCell];
            int actual = cellProp.getiRangOfMax(iCell);
            assertEquals(expected, actual);

        }

    }

}
