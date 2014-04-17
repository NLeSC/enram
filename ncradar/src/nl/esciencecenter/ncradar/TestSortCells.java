package nl.esciencecenter.ncradar;

import java.io.IOException;


public class TestSortCells extends JNIMethodsVol2Bird {


    public TestSortCells() throws IOException {

        int[] cellPropIRangOfMax;
        int[] cellPropIAzimOfMax; 
        double[] cellPropDbz; 
        double[] cellPropTex; 
        double[] cellPropCv; 
        double[] cellPropArea; 
        double[] cellPropClutterArea; 
        double[] cellPropMax; 
        int[] cellPropIndex; 
        int[] cellPropDrop; 
        int nCells; 
        int method;

        cellPropIRangOfMax = new int[] {0,3,6,4};
        cellPropIAzimOfMax = new int[] {2,5,2,4};
        cellPropDbz = new double[] {11,2,25,6};
        cellPropTex = new double[] {1,2,4,1};
        cellPropCv = new double[] {2,3,1,5};
        cellPropArea = new double[] {5,3,7,2};
        cellPropClutterArea = new double[] {0,0,0,0};
        cellPropMax = new double[] {14,4,28,9};
        cellPropIndex = new int[] {3,1,2,0};
        cellPropDrop = new int[] {0,0,0,0};
        nCells = 4; 
        method = 1;
        
        sortCells(cellPropIRangOfMax, cellPropIAzimOfMax, cellPropDbz, cellPropTex, 
                  cellPropCv, cellPropArea, cellPropClutterArea, cellPropMax, 
                  cellPropIndex, cellPropDrop, nCells,method);
        
        int nElems = nCells; 
        for (int iElem = 0;iElem<nElems;iElem++) {
            System.err.println("(Java) iElem "+iElem+" = "+cellPropIndex[iElem]);
        }

    }


}
