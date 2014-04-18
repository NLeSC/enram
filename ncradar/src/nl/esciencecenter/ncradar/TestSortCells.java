package nl.esciencecenter.ncradar;

import java.io.IOException;


public class TestSortCells extends JNIMethodsVol2Bird {


    public TestSortCells() throws IOException {

        int[] cellPropIRangOfMax;
        int[] cellPropIAzimOfMax; 
        float[] cellPropDbzAvg; 
        float[] cellPropTexAvg; 
        float[] cellPropCv; 
        float[] cellPropArea; 
        float[] cellPropClutterArea; 
        float[] cellPropDbzMax; 
        int[] cellPropIndex; 
        char[] cellPropDrop; 
        int nCells; 
        int method;

        cellPropIRangOfMax = new int[] {0,3,6,4};
        cellPropIAzimOfMax = new int[] {2,5,2,4};
        cellPropDbzAvg = new float[] {11,2,25,6};
        cellPropTexAvg = new float[] {1,2,4,1};
        cellPropCv = new float[] {2,3,1,5};
        cellPropArea = new float[] {5,3,7,2};
        cellPropClutterArea = new float[] {0,0,0,0};
        cellPropDbzMax = new float[] {14,4,28,9};
        cellPropIndex = new int[] {3,1,2,0};
        cellPropDrop = new char[] {0,0,0,0};
        nCells = 4; 
        method = 1;
        
        sortCells(cellPropIRangOfMax, cellPropIAzimOfMax, cellPropDbzAvg, cellPropTexAvg, 
                  cellPropCv, cellPropArea, cellPropClutterArea, cellPropDbzMax, 
                  cellPropIndex, cellPropDrop, nCells,method);
        
        int nElems = nCells; 
        for (int iElem = 0;iElem<nElems;iElem++) {
            System.err.println("(Java) iElem "+iElem+" = "+cellPropIndex[iElem]);
        }

    }


}
