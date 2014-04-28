package nl.esciencecenter.ncradar;

public class CellProperties {
    
    private int[] iRangOfMax;
    private int[] iAzimOfMax; 
    private float[] dbzAvg; 
    private float[] texAvg; 
    private float[] cv; 
    private float[] area; 
    private float[] clutterArea; 
    private float[] max; 
    private int[] index; 
    private char[] drop; 
    
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

    public void setiRangOfMax(int[] iRangOfMax) {
        this.iRangOfMax = iRangOfMax;
    }

    public int[] getiAzimOfMax() {
        return iAzimOfMax;
    }

    public void setiAzimOfMax(int[] iAzimOfMax) {
        this.iAzimOfMax = iAzimOfMax;
    }

    public float[] getDbzAvg() {
        return dbzAvg;
    }

    public void setDbzAvg(float[] dbzAvg) {
        this.dbzAvg = dbzAvg;
    }

    public float[] getTexAvg() {
        return texAvg;
    }

    public void setTexAvg(float[] texAvg) {
        this.texAvg = texAvg;
    }

    public float[] getCv() {
        return cv;
    }

    public void setCv(float[] cv) {
        this.cv = cv;
    }

    public float[] getArea() {
        return area;
    }

    public void setArea(float[] area) {
        this.area = area;
    }

    public float[] getClutterArea() {
        return clutterArea;
    }

    public void setClutterArea(float[] clutterArea) {
        this.clutterArea = clutterArea;
    }

    public float[] getMax() {
        return max;
    }

    public void setMax(float[] max) {
        this.max = max;
    }

    public int[] getIndex() {
        return index;
    }

    public void setIndex(int[] index) {
        this.index = index;
    }

    public char[] getDrop() {
        return drop;
    }

    public void setDrop(char[] drop) {
        this.drop = drop;
    }

    

    
}
