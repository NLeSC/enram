package nl.esciencecenter.ncradar;

public class JNIMethodsVol2Bird {

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

    private byte[][] texture;

    static {

        try {
            System.loadLibrary("calctexture");
        } catch (UnsatisfiedLinkError e) {
            System.err
                    .println("Native code library containing 'texture' method failed to load.\n"
                            + e);
        }
        try {
            System.loadLibrary("fringecells");
        } catch (UnsatisfiedLinkError e) {
            System.err
                    .println("Native code library containing 'fringecells' method failed to load.\n"
                            + e);
        }
        try {
            System.loadLibrary("updatemap");
        } catch (UnsatisfiedLinkError e) {
            System.err
                    .println("Native code library containing 'updatemap' method failed to load.\n"
                            + e);
        }
        try {
            System.loadLibrary("sortcells");
        } catch (UnsatisfiedLinkError e) {
            System.err
                    .println("Native code library containing 'sortcells' method failed to load.\n"
                            + e);
        }
        try {
            System.loadLibrary("dist");
        } catch (UnsatisfiedLinkError e) {
            System.err
                    .println("Native code library containing 'dist' method failed to load.\n"
                            + e);
        }
    }



    protected static final native float calcDist(int range1, int azimuth1,
            int range2, int azimuth2, float rangeScale, float azimuthScale);



    protected static final native void calcTexture(byte[] texImage,
            byte[] reflImage, byte[] vradImage, byte nRangNeighborhood,
            byte nAzimNeighborhood, byte nCountMin, byte texType,
            float texOffset, float texScale, float reflOffset, float reflScale,
            float vradOffset, float vradScale, int nRang, int nAzim);



    protected static final native int findCells(int[] cellImage,
            char[] texImage, char[] rhoImage, char[] zdrImage,
            float dbzThresMin, int texMissing, int texnAzim, int texnRang,
            float texValueOffset, float texRangeScale, float texValueScale,
            float texThresMin, int rhoMissing, int rhonAzim, int rhonRang,
            float rhoValueOffset, float rhoValueScale, float rhoThresMin,
            int zdrMissing, int zdrnAzim, int zdrnRang, float zdrValueOffset,
            float zdrValueScale, float zdrThresMin, float rCellMax, char sign);



    protected void calcTexture(RadarScanJava radarScanReflectivity,
            RadarScanJava radarScanRadialVelocity, byte nRangNeighborhood,
            byte nAzimNeighborhood, byte nCountMin, byte texType,
            float texOffset, float texScale) {

        byte[][] reflImage2D = radarScanReflectivity.getScanDataRaw();
        byte[][] vradImage2D = radarScanReflectivity.getScanDataRaw();

        int nAzim = (int) radarScanReflectivity.getNumberOfAzimuthBins();
        int nRang = (int) radarScanReflectivity.getNumberOfRangeBins();

        byte[] texImage = new byte[nAzim * nRang];
        byte[] reflImage = new byte[nAzim * nRang];
        byte[] vradImage = new byte[nAzim * nRang];

        int iGlobal = 0;
        for (int iAzim = 0; iAzim < nAzim; iAzim++) {
            for (int iRang = 0; iRang < nRang; iRang++) {
                reflImage[iGlobal] = reflImage2D[iAzim][iRang];
                vradImage[iGlobal] = vradImage2D[iAzim][iRang];
                iGlobal++;
            }
        }

        float reflOffset = (float) radarScanReflectivity.getOffset();
        float reflScale = (float) radarScanReflectivity.getGain();
        float vradOffset = (float) radarScanRadialVelocity.getOffset();
        float vradScale = (float) radarScanRadialVelocity.getGain();

        calcTexture(texImage, reflImage, vradImage, nRangNeighborhood,
                nAzimNeighborhood, nCountMin, texType, texOffset, texScale,
                reflOffset, reflScale, vradOffset, vradScale, nRang, nAzim);

        iGlobal = 0;
        byte[][] texImage2D = new byte[nAzim][nRang];
        for (int iAzim = 0; iAzim < nAzim; iAzim++) {
            for (int iRang = 0; iRang < nRang; iRang++) {
                texImage2D[iAzim][iRang] = texImage[iGlobal];
                iGlobal++;
            }
        }

        texture = texImage2D.clone();

    }



    protected byte[][] getTexture() {

        return texture.clone();
    };



    protected CellProperties sortCells(CellProperties cellPropIn, int method)
            throws Exception {

        int[] iRangOfMax = cellPropIn.getAlliRangOfMax();
        int[] iAzimOfMax = cellPropIn.getAlliAzimOfMax();
        float[] dbzAvg = cellPropIn.getAllDbzAvg();
        float[] texAvg = cellPropIn.getAllTexAvg();
        float[] cv = cellPropIn.getAllCv();
        float[] area = cellPropIn.getAllArea();
        float[] clutterArea = cellPropIn.getAllClutterArea();
        float[] dbzMax = cellPropIn.getAllDbzMax();
        int[] index = cellPropIn.getAllIndex();
        char[] drop = cellPropIn.getAllDrop();

        int nCells = cellPropIn.getnCells();

        sortCells(iRangOfMax, iAzimOfMax, dbzAvg, texAvg, cv, area,
                clutterArea, dbzMax, index, drop, nCells, method);

        CellProperties cellPropOut = cellPropIn.clone();

        cellPropOut.copyCellPropertiesFrom(iRangOfMax, iAzimOfMax, dbzAvg,
                texAvg, cv, area, clutterArea, dbzMax, index, drop);

        return cellPropOut;

    }



    protected int[] updateMap(CellProperties cellProp, int nCells, int nGlobal,
            int minCellArea) throws Exception {

        int[] iRangOfMax = new int[nCells];
        int[] iAzimOfMax = new int[nCells];
        float[] dbzAvg = new float[nCells];
        float[] texAvg = new float[nCells];
        float[] cv = new float[nCells];
        float[] area = new float[nCells];
        float[] clutterArea = new float[nCells];
        float[] dbzMax = new float[nCells];
        int[] index = new int[nCells];
        char[] drop = new char[nCells];

        cellProp.copyCellPropertiesTo(iRangOfMax, iAzimOfMax, dbzAvg, texAvg,
                cv, area, clutterArea, dbzMax, index, drop);

        int nCellsValid;

        int[] cellImage = new int[nGlobal];

        nCellsValid = updateMap(cellImage, iRangOfMax, iAzimOfMax, dbzAvg,
                texAvg, cv, area, clutterArea, dbzMax, index, drop, nCells,
                nGlobal, minCellArea);

        cellProp.copyCellPropertiesFrom(iRangOfMax, iAzimOfMax, dbzAvg, texAvg,
                cv, area, clutterArea, dbzMax, index, drop);

        return cellImage;

    }

}