package nl.esciencecenter.ncradar;

import static org.junit.Assert.*;

import org.junit.Test;
import org.junit.Before;

public class TestCellPropertiesConstructorEmpty {

    
    private int[] iRangOfMax = {0,0,0,0};
    private int[] iAzimOfMax = {0,0,0,0};
    private float[] dbzAvg = {0.0f,0.0f,0.0f,0.0f};
    private float[] texAvg = {0.0f,0.0f,0.0f,0.0f};
    private float[] cv = {0.0f,0.0f,0.0f,0.0f};
    private float[] area = {0.0f,0.0f,0.0f,0.0f};
    private float[] clutterArea = {0.0f,0.0f,0.0f,0.0f};
    private float[] dbzMax = {0.0f,0.0f,0.0f,0.0f};
    private int[] index = {0,0,0,0};
    private char[] drop = {0,0,0,0};
    
    private int nCells = drop.length;
    
    private CellProperties cellProp;
    private float delta = 0.0000001f;

    
    
    @Before
    public void setUp() throws Exception  {
        
        cellProp = new CellProperties(nCells);
        
    }

    @Test
    public void testGetiRangOfMax(){


        for (int iCell = 0; iCell < nCells; iCell++) {

            {
                float expected = area[iCell];
                float actual = cellProp.getArea(iCell);
                assertEquals(expected, actual, delta);
            }
            {
                float expected = clutterArea[iCell];
                float actual = cellProp.getClutterArea(iCell);
                assertEquals(expected, actual, delta);
            }
            {
                float expected = cv[iCell];
                float actual = cellProp.getCv(iCell);
                assertEquals(expected, actual, delta);
            }
            {
                float expected = dbzAvg[iCell];
                float actual = cellProp.getDbzAvg(iCell);
                assertEquals(expected, actual, delta);
            }
            {
                float expected = dbzMax[iCell];
                float actual = cellProp.getDbzMax(iCell);
                assertEquals(expected, actual, delta);
            }
            {
                char expected = drop[iCell];
                char actual = cellProp.getDrop(iCell);
                assertEquals(expected, actual);
            }
            {
                int expected = iAzimOfMax[iCell];
                int actual = cellProp.getiAzimOfMax(iCell);
                assertEquals(expected, actual);
            }
            {
                int expected = index[iCell];
                int actual = cellProp.getIndex(iCell);
                assertEquals(expected, actual);
            }
            {
                int expected = iRangOfMax[iCell];
                int actual = cellProp.getiRangOfMax(iCell);
                assertEquals(expected, actual);
            }
            {
                float expected = texAvg[iCell];
                float actual = cellProp.getTexAvg(iCell);
                assertEquals(expected, actual, delta);
            }


        }

    }


}
