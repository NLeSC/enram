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
    private float delta = 0.0000001f;

    
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
    
    @Test
    public void testGetiAzimOfMax(){


        int nCells = iAzimOfMax.length;
        
        for (int iCell = 0; iCell < nCells; iCell++) {
            
            int expected = iAzimOfMax[iCell];
            int actual = cellProp.getiAzimOfMax(iCell);
            assertEquals(expected, actual);

        }

    }
    
    @Test
    public void testGetDbzAvg(){


        int nCells = dbzAvg.length;
        
        for (int iCell = 0; iCell < nCells; iCell++) {
            
            float expected = dbzAvg[iCell];
            float actual = cellProp.getDbzAvg(iCell);
             
            assertEquals(expected, actual, delta);

        }

    }
    
    @Test
    public void testGetTexAvg(){


        int nCells = texAvg.length;
        
        for (int iCell = 0; iCell < nCells; iCell++) {
            
            float expected = texAvg[iCell];
            float actual = cellProp.getTexAvg(iCell);
            assertEquals(expected, actual, delta);

        }

    }
    
    @Test
    public void testGetCv(){


        int nCells = cv.length;
        
        for (int iCell = 0; iCell < nCells; iCell++) {
            
            float expected = cv[iCell];
            float actual = cellProp.getCv(iCell);
            assertEquals(expected, actual, delta);

        }

    }
    
    @Test
    public void testGetArea(){


        int nCells = area.length;
        
        for (int iCell = 0; iCell < nCells; iCell++) {
            
            float expected = area[iCell];
            float actual = cellProp.getArea(iCell);
            assertEquals(expected, actual, delta);

        }

    }
    
    @Test
    public void testGetClutterArea(){


        int nCells = clutterArea.length;
        
        for (int iCell = 0; iCell < nCells; iCell++) {
            
            float expected = clutterArea[iCell];
            float actual = cellProp.getClutterArea(iCell);
            assertEquals(expected, actual, delta);

        }

    }
    
    @Test
    public void testGetDbzMax(){


        int nCells = dbzMax.length;
        
        for (int iCell = 0; iCell < nCells; iCell++) {
            
            float expected = dbzMax[iCell];
            float actual = cellProp.getDbzMax(iCell);
            assertEquals(expected, actual, delta);

        }

    }
    
    @Test
    public void testGetIndex(){


        int nCells = index.length;
        
        for (int iCell = 0; iCell < nCells; iCell++) {
            
            int expected = index[iCell];
            int actual = cellProp.getIndex(iCell);
            assertEquals(expected, actual);

        }

    }
    
    @Test
    public void testGetDrop(){


        int nCells = drop.length;
        
        for (int iCell = 0; iCell < nCells; iCell++) {
            
            int expected = drop[iCell];
            int actual = cellProp.getDrop(iCell);
            assertEquals(expected, actual);

        }

    }
    
    @Test
    public void testClone(){

        CellProperties cellPropClone = cellProp.clone();

        int nCells = cellProp.getnCells();
        int nCellsClone = cellPropClone.getnCells();

        {
            int expected = nCells;
            int actual = nCellsClone;
            assertEquals(expected, actual);
        }
        
        float delta = 0.000001f;
        
        
        for (int iCell = 0; iCell < nCells; iCell++) {
            
            {
                int expected = cellProp.getiAzimOfMax(iCell);
                int actual = cellPropClone.getiAzimOfMax(iCell);
                assertEquals(expected, actual);
            }
            {
                int expected = cellProp.getiRangOfMax(iCell);
                int actual = cellPropClone.getiRangOfMax(iCell);
                assertEquals(expected, actual);
            }
            {
                float expected = cellProp.getDbzAvg(iCell);
                float actual = cellPropClone.getDbzAvg(iCell);
                assertEquals(expected, actual, delta);
            }
            {
                float expected = cellProp.getTexAvg(iCell);
                float actual = cellPropClone.getTexAvg(iCell);
                assertEquals(expected, actual, delta);
            }
            {
                float expected = cellProp.getCv(iCell);
                float actual = cellPropClone.getCv(iCell);
                assertEquals(expected, actual, delta);
            }
            {
                float expected = cellProp.getArea(iCell);
                float actual = cellPropClone.getArea(iCell);
                assertEquals(expected, actual, delta);
            }
            {
                float expected = cellProp.getClutterArea(iCell);
                float actual = cellPropClone.getClutterArea(iCell);
                assertEquals(expected, actual, delta);
            }
            {
                float expected = cellProp.getDbzMax(iCell);
                float actual = cellPropClone.getDbzMax(iCell);
                assertEquals(expected, actual, delta);
            }
            {
                int expected = cellProp.getIndex(iCell);
                int actual = cellPropClone.getIndex(iCell);
                assertEquals(expected, actual);
            }
            {
                int expected = cellProp.getDrop(iCell);
                int actual = cellPropClone.getDrop(iCell);
                assertEquals(expected, actual);
            }
            
        }

    }
    

}
