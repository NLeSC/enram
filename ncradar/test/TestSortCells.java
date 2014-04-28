package nl.esciencecenter.ncradar;

import java.io.IOException;


public class TestSortCells extends JNIMethodsVol2Bird {


    public TestSortCells() throws IOException {

        CellProperties cellProp;
        
        int[] cellPropIRangOfMax = new int[] {0,3,6,4};
        int[] cellPropIAzimOfMax = new int[] {2,5,2,4}; 
        float[] cellPropDbzAvg = new float[] {2,11,25,6}; 
        float[] cellPropTexAvg = new float[] {1,2,4,1}; 
        float[] cellPropCv = new float[] {2,3,1,5}; 
        float[] cellPropArea = new float[] {5,3,7,2}; 
        float[] cellPropClutterArea = new float[] {0,0,0,0}; 
        float[] cellPropMax  = new float[] {14,4,28,9}; 
        int[] cellPropIndex = new int[] {0,1,2,3}; 
        char[] cellPropDrop = new char[] {0,0,0,0}; 
        int nCells = 4; 
        int method = 2;

        sortCells(cellPropIRangOfMax, cellPropIAzimOfMax, cellPropDbzAvg, cellPropTexAvg, 
                  cellPropCv, cellPropArea, cellPropClutterArea, cellPropMax, 
                  cellPropIndex, cellPropDrop, nCells,method);
        
        int nElems = nCells;
        for (int iElem = 0;iElem<nElems;iElem++) {
            System.err.println("(Java) iElem "+iElem+": cellPropArea[iElem] = "+cellPropArea[iElem]+
                                                     "; cellPropDbzAvg[iElem] = "+cellPropDbzAvg[iElem]);
        }

    }


}
