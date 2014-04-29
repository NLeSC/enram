package nl.esciencecenter.ncradar;

public class CellProperties {
    
    private final int[] iRangOfMax;
    private final int[] iAzimOfMax; 
    private final float[] dbzAvg; 
    private final float[] texAvg; 
    private final float[] cv; 
    private final float[] area; 
    private final float[] clutterArea; 
    private final float[] max; 
    private final int[] index; 
    private final char[] drop; 
    
    public CellProperties(int[] iRangOfMax, int[] iAzimOfMax, float[] dbzAvg, float[] texAvg, float[] cv, 
                          float[] area, float[] clutterArea, float[] max, int[] index, char[] drop) {
        
        this.iRangOfMax = iRangOfMax;
        this.iAzimOfMax = iAzimOfMax;
        this.dbzAvg = dbzAvg;
        this.texAvg = texAvg;
        this.cv = cv;
        this.area = area;
        this.clutterArea = clutterArea;
        this.max = max;
        this.index = index;
        this.drop = drop;
    
    }

    public int[] getiRangOfMax() {
        return iRangOfMax;
    }

    public int[] getiAzimOfMax() {
        return iAzimOfMax;
    }

    public float[] getDbzAvg() {
        return dbzAvg;
    }

    public float[] getTexAvg() {
        return texAvg;
    }

    public float[] getCv() {
        return cv;
    }

    public float[] getArea() {
        return area;
    }

    public float[] getClutterArea() {
        return clutterArea;
    }

    public float[] getMax() {
        return max;
    }

    public int[] getIndex() {
        return index;
    }

    public char[] getDrop() {
        return drop;
    }


}
