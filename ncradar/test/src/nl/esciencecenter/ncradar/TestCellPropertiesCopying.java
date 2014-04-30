package nl.esciencecenter.ncradar;

import static org.junit.Assert.*;

import org.junit.Before;
import org.junit.Test;

public class TestCellPropertiesCopying {

    private int[] iRangOfMaxIn = {12,56,176};
    private int[] iAzimOfMaxIn = {6,119,243};
    private float[] dbzAvgIn = {89663.5f,2342.536f,863.33f};
    private float[] texAvgIn = {345.78f,1223.54f,8979.2f};
    private float[] cvIn = {89435.543f,232.5f,974.23f};
    private float[] areaIn = {456f,323.64f,6776.32f};
    private float[] clutterAreaIn = {78f,234.5f,232.3f};
    private float[] dbzMaxIn = {9837.2f,53.67f,8765.23f};
    private int[] indexIn = {2,7,96};
    private char[] dropIn = {0,1,0};
    private float delta = 0.0000001f;
    
    private int nCells = dropIn.length;
    
    private CellProperties cellProp;
    
    private int[] iRangOfMaxOut = new int[nCells];
    private int[] iAzimOfMaxOut = new int[nCells];
    private float[] dbzAvgOut = new float[nCells];
    private float[] texAvgOut = new float[nCells];
    private float[] cvOut = new float[nCells];
    private float[] areaOut = new float[nCells];
    private float[] clutterAreaOut = new float[nCells];
    private float[] dbzMaxOut = new float[nCells];
    private int[] indexOut = new int[nCells];
    private char[] dropOut = new char[nCells];

    
    
    @Before
    public void setUp() throws Exception {
        
        cellProp = new CellProperties(iRangOfMaxIn, iAzimOfMaxIn, dbzAvgIn, texAvgIn, 
                cvIn, areaIn, clutterAreaIn, dbzMaxIn, indexIn, dropIn);

        cellProp.copyCellPropertiesTo(iRangOfMaxOut, iAzimOfMaxOut, dbzAvgOut, texAvgOut, 
                cvOut, areaOut, clutterAreaOut, dbzMaxOut, indexOut, dropOut);
        

    }


    
    @Test
    public void testCopyingIRangOfMax() {

        for (int iCell = 0; iCell < nCells; iCell++) {
            
            int expected = iRangOfMaxIn[iCell];
            int actual = iRangOfMaxOut[iCell];
            assertEquals(expected, actual);
        }
    }

    
    
    @Test
    public void testCopyingIAzimOfMax() {

        for (int iCell = 0; iCell < nCells; iCell++) {
            
            int expected = iAzimOfMaxIn[iCell];
            int actual = iAzimOfMaxOut[iCell];
            assertEquals(expected, actual);
        }
    }
        
    @Test
    public void testCopyingDbzAvg() {
    
        for (int iCell = 0; iCell < nCells; iCell++) {
            
            float expected = dbzAvgIn[iCell];
            float actual = dbzAvgOut[iCell];
            assertEquals(expected, actual, delta);
        }
    }   
    

    @Test
    public void testCopyingTexAvg() {

        for (int iCell = 0; iCell < nCells; iCell++) {

            float expected = texAvgIn[iCell];
            float actual = texAvgOut[iCell];
            assertEquals(expected, actual, delta);
        }
    }   
    

    @Test
    public void testCopyingCv() {

        for (int iCell = 0; iCell < nCells; iCell++) {

            float expected = cvIn[iCell];
            float actual = cvOut[iCell];
            assertEquals(expected, actual, delta);
        }
    }   
    

    @Test
    public void testCopyingArea() {

        for (int iCell = 0; iCell < nCells; iCell++) {

            float expected = areaIn[iCell];
            float actual = areaOut[iCell];
            assertEquals(expected, actual, delta);
        }
    }   
    

    @Test
    public void testCopyingClutterArea() {

        for (int iCell = 0; iCell < nCells; iCell++) {

            float expected = clutterAreaIn[iCell];
            float actual = clutterAreaOut[iCell];
            assertEquals(expected, actual, delta);
        }
    }   
    
    

    @Test
    public void testCopyingDbzMax() {

        for (int iCell = 0; iCell < nCells; iCell++) {

            float expected = dbzMaxIn[iCell];
            float actual = dbzMaxOut[iCell];
            assertEquals(expected, actual, delta);
        }
    }   
    

    @Test
    public void testCopyingIndex() {

        for (int iCell = 0; iCell < nCells; iCell++) {

            int expected = indexIn[iCell];
            int actual = indexOut[iCell];
            assertEquals(expected, actual);
        }
    }   
    


    @Test
    public void testCopyingDrop() {

        for (int iCell = 0; iCell < nCells; iCell++) {

            int expected = dropIn[iCell];
            int actual = dropOut[iCell];
            assertEquals(expected, actual);
        }
    }   
    
    
    
    
    
    
}
