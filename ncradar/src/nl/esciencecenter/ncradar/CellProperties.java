package nl.esciencecenter.ncradar;


public class CellProperties {

    private static class PropertiesOfACell {
        int iRangOfMax;
        int iAzimOfMax;
        float dbzAvg;
        float texAvg;
        float cv;
        float area;
        float clutterArea;
        float dbzMax;
        int index;
        char drop;
    }

    protected PropertiesOfACell[] cellProperties;

    public CellProperties(int nCells) {

        cellProperties = new PropertiesOfACell[nCells];
        for (int iCell = 0; iCell<nCells; iCell++) {
            cellProperties[iCell] = new PropertiesOfACell();
        }

    }
    
    public CellProperties(int[] iRangOfMax, int[] iAzimOfMax, float[] dbzAvg,
            float[] texAvg, float[] cv, float[] area, float[] clutterArea,
            float[] dbzMax, int[] index, char[] drop) throws Exception {

        // use length of first array
        int nCells = iRangOfMax.length;
        
        cellProperties = new PropertiesOfACell[nCells];
        
        assertLengths(iRangOfMax, iAzimOfMax, dbzAvg, texAvg, cv, area,
                clutterArea, dbzMax, index, drop);
        

        copyCellPropertiesFrom(iRangOfMax, iAzimOfMax, dbzAvg, texAvg, cv,
                area, clutterArea, dbzMax, index, drop);
    }

    protected void copyCellPropertiesFrom(final int[] iRangOfMax, final int[] iAzimOfMax,
            final float[] dbzAvg, final float[] texAvg, final float[] cv, final float[] area,
            final float[] clutterArea, final float[] dbzMax,final  int[] index, final char[] drop)
            throws Exception {

        int nElems = cellProperties.length;

        assertLengths(iRangOfMax, iAzimOfMax, dbzAvg, texAvg, cv, area,
                clutterArea, dbzMax, index, drop);

        if (iRangOfMax.length != cellProperties.length) {
            throw new Exception(
                    " Length(s) of array do not match cellProperties length.");
        }
        
        for (int iElem = 0; iElem < nElems; iElem++) {

            cellProperties[iElem] = new PropertiesOfACell();
            
            cellProperties[iElem].iRangOfMax = iRangOfMax[iElem];
            cellProperties[iElem].iAzimOfMax = iAzimOfMax[iElem];
            cellProperties[iElem].dbzAvg = dbzAvg[iElem];
            cellProperties[iElem].texAvg = texAvg[iElem];
            cellProperties[iElem].cv = cv[iElem];
            cellProperties[iElem].area = area[iElem];
            cellProperties[iElem].clutterArea = clutterArea[iElem];
            cellProperties[iElem].dbzMax = dbzMax[iElem];
            cellProperties[iElem].index = index[iElem];
            cellProperties[iElem].drop = drop[iElem];
        }
        
    }



    protected void copyCellPropertiesTo(int[] iRangOfMax, int[] iAzimOfMax,
            float[] dbzAvg, float[] texAvg, float[] cv, float[] area,
            float[] clutterArea, float[] dbzMax, int[] index, char[] drop) {

        int nElems = cellProperties.length;

        for (int iElem = 0; iElem < nElems; iElem++) {
            iRangOfMax[iElem] = cellProperties[iElem].iRangOfMax;
            iAzimOfMax[iElem] = cellProperties[iElem].iAzimOfMax;
            dbzAvg[iElem] = cellProperties[iElem].dbzAvg;
            texAvg[iElem] = cellProperties[iElem].texAvg;
            cv[iElem] = cellProperties[iElem].cv;
            area[iElem] = cellProperties[iElem].area;
            clutterArea[iElem] = cellProperties[iElem].clutterArea;
            dbzMax[iElem] = cellProperties[iElem].dbzMax;
            index[iElem] = cellProperties[iElem].index;
            drop[iElem] = cellProperties[iElem].drop;
        }

    }
    
    private void assertLengths(int[] iRangOfMax, int[] iAzimOfMax,
            float[] dbzAvg, float[] texAvg, float[] cv, float[] area,
            float[] clutterArea, float[] dbzMax, int[] index, char[] drop) throws Exception {

        int nCells = this.cellProperties.length; 
        
    }

    
    public int getiRangOfMax(int iCell) {
        
        return cellProperties[iCell].iRangOfMax;
        
    }
    
    public int[] getAlliRangOfMax() {
        
        int nCells = cellProperties.length;
        
        int[] iRangOfMax = new int[nCells];
        
        for (int iCell = 0; iCell < nCells; iCell++) {
            iRangOfMax[iCell] = cellProperties[iCell].iRangOfMax;
        }
        
        return iRangOfMax;
        
    }    

    public int getiAzimOfMax(int iCell) {
        
        return cellProperties[iCell].iAzimOfMax;
        
    }
    
    public int[] getAlliAzimOfMax() {
        
        int nCells = cellProperties.length;
        
        int[] iAzimOfMax = new int[nCells];
        
        for (int iCell = 0; iCell < nCells; iCell++) {
            iAzimOfMax[iCell] = cellProperties[iCell].iAzimOfMax;
        }
        
        return iAzimOfMax;
        
    }    
    
    
    
    public float getDbzAvg(int iCell) {
        
        return cellProperties[iCell].dbzAvg;
        
    }
    
    
    public float[] getAllDbzAvg() {
        
        int nCells = cellProperties.length;
        
        float[] dbzAvg = new float[nCells];
        
        for (int iCell = 0; iCell < nCells; iCell++) {
            dbzAvg[iCell] = cellProperties[iCell].dbzAvg;
        }
        
        return dbzAvg;
        
    }        
    
    
    public float getTexAvg(int iCell) {
        
        return cellProperties[iCell].texAvg;
        
    }
    
    
    
    public float[] getAllTexAvg() {
        
        int nCells = cellProperties.length;
        
        float[] texAvg = new float[nCells];
        
        for (int iCell = 0; iCell < nCells; iCell++) {
            texAvg[iCell] = cellProperties[iCell].texAvg;
        }
        
        return texAvg;
        
    }         
    
    
    
    public float getCv(int iCell) {
        
        return cellProperties[iCell].cv;
        
    }

    
    
    public float[] getAllCv() {
        
        int nCells = cellProperties.length;
        
        float[] cv = new float[nCells];
        
        for (int iCell = 0; iCell < nCells; iCell++) {
            cv[iCell] = cellProperties[iCell].cv;
        }
        
        return cv;
        
    }         
    
    
    
    
    public float getArea(int iCell) {
        
        return cellProperties[iCell].area;
        
    }
    
    
    public float[] getAllArea() {
        
        int nCells = cellProperties.length;
        
        float[] area = new float[nCells];
        
        for (int iCell = 0; iCell < nCells; iCell++) {
            area[iCell] = cellProperties[iCell].area;
        }
        
        return area;
        
    }        
    
    
    public float getClutterArea(int iCell) {
        
        return cellProperties[iCell].clutterArea;
        
    }
    
    
    public float[] getAllClutterArea() {
        
        int nCells = cellProperties.length;
        
        float[] clutterArea = new float[nCells];
        
        for (int iCell = 0; iCell < nCells; iCell++) {
            clutterArea[iCell] = cellProperties[iCell].clutterArea; 
        }
        
        return clutterArea;
        
    }        
    
    
    public float getDbzMax(int iCell) {
        
        return cellProperties[iCell].dbzMax;
        
    }
    
    
    public float[] getAllDbzMax() {
        
        int nCells = cellProperties.length;
        
        float[] dbzMax = new float[nCells];
        
        for (int iCell = 0; iCell < nCells; iCell++) {
            dbzMax[iCell] = cellProperties[iCell].dbzMax; 
        }
        
        return dbzMax;
        
    }            
    
    public int getIndex(int iCell) {
        
        return cellProperties[iCell].index;
        
    }

    
    
    public int[] getAllIndex() {
        
        int nCells = cellProperties.length;
        
        int[] index = new int[nCells];
        
        for (int iCell = 0; iCell < nCells; iCell++) {
            index[iCell] = cellProperties[iCell].index; 
        }
        
        return index;
        
    }            
    
    
    
    public char getDrop(int iCell) {
        
        return cellProperties[iCell].drop;
        
    }
    

    
    public char[] getAllDrop() {
        
        int nCells = cellProperties.length;
        
        char[] drop = new char[nCells];
        
        for (int iCell = 0; iCell < nCells; iCell++) {
            drop[iCell] = cellProperties[iCell].drop; 
        }
        
        return drop;
        
    }
    
    public PropertiesOfACell[] clone() {

        int nCells = cellProperties.length;
        
        PropertiesOfACell[] cellPropClone = new PropertiesOfACell[nCells];
        
        for (int iCell = 0; iCell < nCells; iCell++) {
            
            cellPropClone[iCell].iRangOfMax = this.getiRangOfMax(iCell);
            cellPropClone[iCell].iAzimOfMax = this.getiAzimOfMax(iCell);
            cellPropClone[iCell].dbzAvg = this.getDbzAvg(iCell);
            cellPropClone[iCell].texAvg = this.getTexAvg(iCell);
            cellPropClone[iCell].cv = this.getCv(iCell);
            cellPropClone[iCell].area = this.getArea(iCell);
            cellPropClone[iCell].clutterArea = this.getClutterArea(iCell);
            cellPropClone[iCell].dbzMax = this.getDbzMax(iCell);
            cellPropClone[iCell].index = this.getIndex(iCell);
            cellPropClone[iCell].drop = this.getDrop(iCell);
        }        
        
        return cellPropClone;
        
    }
}
