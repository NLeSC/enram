package nl.esciencecenter.ncradar;

public class JNIMethodsVol2Bird {

    static {

        try {
            System.loadLibrary("vol2birdjni");
        } catch (UnsatisfiedLinkError e) {
            System.err
                    .println("Native code library 'vol2bird' failed to load.\n"
                            + e);
        }
    }



    protected static final native int analyzeCells(int[] dbzImageInt, int[] vradImageInt, int[] texImageInt,
            int[] clutterImageInt, int[] cellImageInt, int dbznRang, int dbznAzim,
            float dbzElev, float dbzValueScale, float dbzValueOffset,
            float vradValueScale, float vradValueOffset,
            float clutterValueScale, float clutterValueOffset,
            float texValueScale, float texValueOffset,
            int nCells, int areaMin, float cellDbzMin, float cellStdDevMax,
            float cellClutterFraction, float vradMinValue, float dbzClutterMin,
            int cmFlagInt, int dualPolFlagInt, int verboseInt);



    protected static final native float calcDist(int range1, int azimuth1,
            int range2, int azimuth2, float rangeScale, float azimuthScale);



    protected static final native void calcTexture(int[] texImage,
            int[] dbzImage, int[] vradImage, int nRangNeighborhood,
            int nAzimNeighborhood, int nCountMin, int texType,
            float texOffset, float texScale, float dbzOffset, float dbzScale,
            float vradOffset, float vradScale, int vradMissing, int nRang, int nAzim);



    protected static final native void calcVvp(int nRang, int nAzim, float rangeScale, float azimuthScale, float elevAngle,
            int missing, float radarHeight, float valueOffset, float valueScale, int[] vradImageInt, float[] points,
            float[] yObs, int[] c, int[] cellImage, int nDims, int nPointsMaxPtr, float rangeMin, float rangeMax,
            float HLAYER, float heightInputPar, float vradMin, int iData, int layer, int nPointsPtr);



    protected static final native void classify(int dbznRang, int dbznAzim, float dbzRangeScale, float dbzElev,
            float dbzHeig, float dbzValueScale, float dbzValueOffset, float dbzAzimScale,
            int dbzMissing, float vradValueScale, float vradValueOffset, int vradMissing,
            int rawReflMissing, float clutterValueScale, float clutterValueOffset,
            int[] cellImageInt, int[] dbzImageInt, int[] vradImageInt,
            int[] rawReflImageInt, int[] clutterImageInt, float[] zdata,
            float rangeMin, float rangeMax, float HLAYER, float XOFFSET, float XSCALE,
            float XMEAN, float height, float azimMin, float azimMax, float vradMin,
            float dbzClutter, float dbzMin, float dBZx, float DBZNOISE, int NGAPMIN,
            int NGAPBIN, int NDBZMIN, int layer, int np, int nPointsPtr,
            int nPointsAllPtr, int nPointsClutterPtr, int nPointsRainPtr, int nPointsRainNoFringePtr,
            int clutterFlagInt, int rawReflFlagInt, int xflagInt);



    protected static final native int findCells(int[] texImage,
            int[] rhoImage, int[] zdrImage, int[] cellImage, int texMissing,
            int texnAzim, int texnRang, float texValueOffset,
            float texRangeScale, float texValueScale, float texThresMin,
            int rhoMissing, int rhonAzim, int rhonRang, float rhoValueOffset,
            float rhoValueScale, float rhoThresMin, int zdrMissing,
            int zdrnAzim, int zdrnRang, float zdrValueOffset,
            float zdrValueScale, float zdrThresMin, float reflThresMin,
            float rCellMax, int sign);



    protected static final native int findNearbyGateIndex(int nAzimParent, int nRangParent, int iParent,
            int nAzimChild, int nRangChild, int iChild);



    protected static final native void fringeCells(int[] cellImage, int nRange,
            int nAzim, float azimuthScale, float rangeScale, float fringe);



    protected static final native void sortCells(int[] cellPropIRangOfMax,
            int[] cellPropIAzimOfMax, float[] cellPropDbzAvg,
            float[] cellPropTexAvg, float[] cellPropCv, float[] cellPropArea,
            float[] cellPropClutterArea, float[] cellPropDbzMax,
            int[] cellPropIndex, char[] cellPropDrop, int nCells, int method);



    protected static final native int updateMap(int[] cellImage,
            int[] cellPropIRangOfMax, int[] cellPropIAzimOfMax,
            float[] cellPropDbzAvg, float[] cellPropTexAvg, float[] cellPropCv,
            float[] cellPropArea, float[] cellPropClutterArea,
            float[] cellPropDbzMax, int[] cellPropIndex, char[] cellPropDrop,
            int nCells, int nGlobal, int minCellArea);

}