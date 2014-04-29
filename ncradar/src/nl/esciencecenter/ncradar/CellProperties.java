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
        
        this.iRangOfMax = iRangOfMax.clone();
        this.iAzimOfMax = iAzimOfMax.clone();
        this.dbzAvg = dbzAvg.clone();
        this.texAvg = texAvg.clone();
        this.cv = cv.clone();
        this.area = area.clone();
        this.clutterArea = clutterArea.clone();
        this.max = max.clone();
        this.index = index.clone();
        this.drop = drop.clone();
    
    }

    public int[] getiRangOfMax() {
        return iRangOfMax.clone();
    }

    public int[] getiAzimOfMax() {
        return iAzimOfMax.clone();
    }

    public float[] getDbzAvg() {
        return dbzAvg.clone();
    }

    public float[] getTexAvg() {
        return texAvg.clone();
    }

    public float[] getCv() {
        return cv.clone();
    }

    public float[] getArea() {
        return area.clone();
    }

    public float[] getClutterArea() {
        return clutterArea.clone();
    }

    public float[] getMax() {
        return max.clone();
    }

    public int[] getIndex() {
        return index.clone();
    }

    public char[] getDrop() {
        return drop.clone();
    }


}
