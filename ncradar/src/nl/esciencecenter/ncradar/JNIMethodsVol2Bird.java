package nl.esciencecenter.ncradar;

public class JNIMethodsVol2Bird {

    static {
        //System.loadLibrary("calctexture");
        System.loadLibrary("dist");
        System.loadLibrary("fringecells");
        System.loadLibrary("updatemap");
        System.loadLibrary("sortcells");
        //System.loadLibrary("findcells");
        }
         
    static final native float dist(int range1, 
                                   int azim1,
                                   int range2,
                                   int azim2,
                                   float rscale,
                                   float ascale);

    
    
    static final native void fringeCells(int[] cellImage, 
                                         int nRange,
                                         int nAzim,
                                         float azimuthScale,
                                         float rangeScale,
                                         float fringe);
    

    public int[] updateMap(final CellProperties inputCellProp, int nCells, int nGlobal, int minCellArea, CellProperties outputCellProp) throws Exception {
        
        int[] cellImage = new int[nGlobal];
        
        int[] iRangOfMax = null;
        int[] iAzimOfMax = null;
        float[] dbzAvg = null;
        float[] texAvg = null;
        float[] cv = null;
        float[] area = null;
        float[] clutterArea = null;
        float[] dbzMax = null;
        int[] index = null;
        char[] drop = null;
        
        inputCellProp.copyCellPropertiesTo(iRangOfMax, iAzimOfMax, dbzAvg, texAvg, cv, area, clutterArea, dbzMax, index, drop);
        
        int nCellsValid;
        
        nCellsValid = updateMap(cellImage,
                                iRangOfMax,
                                iAzimOfMax,
                                dbzAvg,
                                texAvg,
                                cv,
                                area,
                                clutterArea,
                                dbzMax,
                                index,
                                drop,
                                nCells,
                                nGlobal,
                                minCellArea);
        
        outputCellProp.copyCellPropertiesFrom(iRangOfMax, iAzimOfMax, dbzAvg, texAvg, cv, area, clutterArea, dbzMax, index, drop);
        
        return cellImage;
        
    };
    
    private static final native int updateMap(int[] cellImage,
                                              int[] cellPropIRangOfMax,
                                              int[] cellPropIAzimOfMax,
                                              float[] cellPropDbzAvg,
                                              float[] cellPropTexAvg,
                                              float[] cellPropCv,
                                              float[] cellPropArea,
                                              float[] cellPropClutterArea,
                                              float[] cellPropDbzMax,
                                              int[] cellPropIndex,
                                              char[] cellPropDrop,
                                              int nCells,
                                              int nGlobal,
                                              int minCellArea);

    public CellProperties sortCells(CellProperties cellProp, int method) {
        
//        int[] iRangOfMax = cellProp.getiRangOfMax();
//        int[] iAzimOfMax = cellProp.getiAzimOfMax(); 
//        float[] dbzAvg = cellProp.getDbzAvg();
//        float[] texAvg = cellProp.getTexAvg();
//        float[] cv = cellProp.getCv();
//        float[] area = cellProp.getArea();
//        float[] clutterArea = cellProp.getClutterArea();
//        float[] dbzMax = cellProp.getMax();
//        int[] index = cellProp.getIndex();
//        char[] drop = cellProp.getDrop();
//        
//        int nCells = cellProp.
//        
//        sortCells(iRangOfMax,
//                  iAzimOfMax,
//                  dbzAvg,
//                  texAvg,
//                  cv,
//                  area,
//                  clutterArea,
//                  dbzMax,
//                  index,
//                  drop,
//                  nCells,
//                  method);
//        
//        cellProp.setiRangOfMax(iRangOfMax);
//        cellProp.setiAzimOfMax(iAzimOfMax);
//        cellProp.setDbzAvg(dbzAvg);
//        cellProp.setTexAvg(texAvg);
//        cellProp.setCv(cv);
//        cellProp.setArea(area);
//        cellProp.setClutterArea(clutterArea);
//        cellProp.setMax(dbzMax);
//        cellProp.setIndex(index);
//        cellProp.setDrop(drop);
        
        return cellProp;
        
    };
    
    private static final native void sortCells(int[] cellPropIRangOfMax,
                                               int[] cellPropIAzimOfMax,
                                               float[] cellPropDbzAvg,
                                               float[] cellPropTexAvg,
                                               float[] cellPropCv,
                                               float[] cellPropArea,
                                               float[] cellPropClutterArea,
                                               float[] cellPropDbzMax,
                                               int[] cellPropIndex,
                                               char[] cellPropDrop,
                                               int nCells,
                                               int method);
    
    static final native int findCells(int[] cellImage,
                                      char[] texImage,
                                      char[] rhoImage,
                                      char[] zdrImage,
                                      float dbzThresMin,
                                      int texMissing,
                                      int texnAzim,
                                      int texnRang,
                                      float texValueOffset,
                                      float texRangeScale,
                                      float texValueScale,
                                      float texThresMin,
                                      int rhoMissing,
                                      int rhonAzim,
                                      int rhonRang,
                                      float rhoValueOffset,
                                      float rhoValueScale,
                                      float rhoThresMin,
                                      int zdrMissing,
                                      int zdrnAzim,
                                      int zdrnRang,
                                      float zdrValueOffset,
                                      float zdrValueScale,
                                      float zdrThresMin,
                                      float rCellMax,
                                      char sign);
   
}