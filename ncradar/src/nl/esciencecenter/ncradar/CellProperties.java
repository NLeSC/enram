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

    private int nCells;
    
    protected PropertiesOfACell[] cellProperties;

    public CellProperties(int nCells) {

        this.nCells = nCells;
        cellProperties = new PropertiesOfACell[nCells];
        for (int iCell = 0; iCell<nCells; iCell++) {
            cellProperties[iCell] = new PropertiesOfACell();
        }

    }
    
    public CellProperties(int[] iRangOfMax, int[] iAzimOfMax, float[] dbzAvg,
            float[] texAvg, float[] cv, float[] area, float[] clutterArea,
            float[] dbzMax, int[] index, char[] drop) throws Exception {

        // use length of first array
        this.nCells = iRangOfMax.length;
        
        cellProperties = new PropertiesOfACell[nCells];

        copyCellPropertiesFrom(iRangOfMax, iAzimOfMax, dbzAvg, texAvg, cv,
                               area, clutterArea, dbzMax, index, drop);
    }

    protected void copyCellPropertiesFrom(final int[] iRangOfMax, final int[] iAzimOfMax,
            final float[] dbzAvg, final float[] texAvg, final float[] cv, final float[] area,
            final float[] clutterArea, final float[] dbzMax,final  int[] index, final char[] drop)
            throws Exception {

        int nCells = getnCells();

        if (iRangOfMax.length != nCells || 
            iAzimOfMax.length != nCells ||
            dbzAvg.length != nCells ||
            texAvg.length != nCells || 
            cv.length != nCells || 
            area.length != nCells ||
            clutterArea.length != nCells || 
            dbzMax.length != nCells ||
            index.length != nCells || 
            drop.length != nCells ) {
            throw new Exception(
                    
                    " Length(s) of array do not match cellProperties length.");
        }
        
        for (int iCell = 0; iCell < nCells; iCell++) {

            cellProperties[iCell] = new PropertiesOfACell();
            
            cellProperties[iCell].iRangOfMax = iRangOfMax[iCell];
            cellProperties[iCell].iAzimOfMax = iAzimOfMax[iCell];
            cellProperties[iCell].dbzAvg = dbzAvg[iCell];
            cellProperties[iCell].texAvg = texAvg[iCell];
            cellProperties[iCell].cv = cv[iCell];
            cellProperties[iCell].area = area[iCell];
            cellProperties[iCell].clutterArea = clutterArea[iCell];
            cellProperties[iCell].dbzMax = dbzMax[iCell];
            cellProperties[iCell].index = index[iCell];
            cellProperties[iCell].drop = drop[iCell];
        }
        
    }



    protected void copyCellPropertiesTo(int[] iRangOfMax, int[] iAzimOfMax,
            float[] dbzAvg, float[] texAvg, float[] cv, float[] area,
            float[] clutterArea, float[] dbzMax, int[] index, char[] drop) {

        int nCells = getnCells();

        for (int iCell = 0; iCell < nCells; iCell++) {
            iRangOfMax[iCell] = cellProperties[iCell].iRangOfMax;
            iAzimOfMax[iCell] = cellProperties[iCell].iAzimOfMax;
            dbzAvg[iCell] = cellProperties[iCell].dbzAvg;
            texAvg[iCell] = cellProperties[iCell].texAvg;
            cv[iCell] = cellProperties[iCell].cv;
            area[iCell] = cellProperties[iCell].area;
            clutterArea[iCell] = cellProperties[iCell].clutterArea;
            dbzMax[iCell] = cellProperties[iCell].dbzMax;
            index[iCell] = cellProperties[iCell].index;
            drop[iCell] = cellProperties[iCell].drop;
        }

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
    
    public CellProperties clone() {

        int nCells = getnCells();
        CellProperties cellPropClone = new CellProperties(nCells);
        
        int[] iRangOfMax = this.getAlliRangOfMax();
        int[] iAzimOfMax = this.getAlliAzimOfMax();
        float[] dbzAvg = this.getAllDbzAvg();
        float[] texAvg = this.getAllTexAvg();
        float[] cv = this.getAllCv();
        float[] area = this.getAllArea();
        float[] clutterArea = this.getAllClutterArea();
        float[] dbzMax = this.getAllDbzMax();
        int[] index = this.getAllIndex();
        char[] drop = this.getAllDrop();
        
        try {
            cellPropClone.copyCellPropertiesFrom(iRangOfMax,
                                                 iAzimOfMax, 
                                                 dbzAvg, 
                                                 texAvg, 
                                                 cv, 
                                                 area, 
                                                 clutterArea, 
                                                 dbzMax, 
                                                 index, 
                                                 drop);
        } catch (Exception e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
        
        return cellPropClone;
        
    }

    public int getnCells() {
        return nCells;
    }

}
