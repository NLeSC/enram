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

    public CellProperties(int[] iRangOfMax, int[] iAzimOfMax, float[] dbzAvg,
            float[] texAvg, float[] cv, float[] area, float[] clutterArea,
            float[] dbzMax, int[] index, char[] drop) throws Exception {

        // use length of first array
        int nCells = iRangOfMax.length;
        
        assertLengths(iRangOfMax, iAzimOfMax, dbzAvg, texAvg, cv, area,
                clutterArea, dbzMax, index, drop);
        
        cellProperties = new PropertiesOfACell[nCells];

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

}
