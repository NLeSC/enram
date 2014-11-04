package nl.esciencecenter.ncradar;

public class BirdDensityProfileJava extends JNIMethodsVol2Bird {

    private final RadarScanJava reflectivity;
    private final RadarScanJava radialVelocity;
    private int[] texture;
    private int[] cellImage;
    private final int nRang;
    private final int nAzim;



    public BirdDensityProfileJava(RadarScanJava reflectivity, RadarScanJava radialVelocity) throws Exception {

        this.reflectivity = reflectivity;
        this.radialVelocity = radialVelocity;

        nAzim = reflectivity.getNumberOfAzimuthBins();
        nRang = reflectivity.getNumberOfRangeBins();
        cellImage = new int[nAzim * nRang];

        {
            int nAzim = radialVelocity.getNumberOfAzimuthBins();
            int nRang = radialVelocity.getNumberOfRangeBins();

            if (nAzim != this.nAzim) {
                throw new Exception("Number of azimuth bins differ.");
            }
            if (nRang != this.nRang) {
                throw new Exception("Number of range bins differ.");
            }
        }

    }




    public void calcTexture(int nRangNeighborhood, int nAzimNeighborhood, int nCountMin, float texOffset, float texScale, int vradMissing) {

        int[][] dbzImage2D = reflectivity.getScanDataRaw2D();
        int[][] vradImage2D = reflectivity.getScanDataRaw2D();

        int[] texImage = new int[nAzim * nRang];
        int[] dbzImage = new int[nAzim * nRang];
        int[] vradImage = new int[nAzim * nRang];

        int iGlobal = 0;
        for (int iAzim = 0; iAzim < nAzim; iAzim++) {
            for (int iRang = 0; iRang < nRang; iRang++) {
                dbzImage[iGlobal] = dbzImage2D[iAzim][iRang];
                vradImage[iGlobal] = vradImage2D[iAzim][iRang];
                iGlobal++;
            }
        }

        float dbzOffset = (float) reflectivity.getDataOffset();
        float dbzScale = (float) reflectivity.getDataScale();
        float vradOffset = (float) radialVelocity.getDataOffset();
        float vradScale = (float) radialVelocity.getDataScale();

        calcTexture(texImage, dbzImage, vradImage,
                nRangNeighborhood, nAzimNeighborhood, nCountMin,
                texOffset, texScale,
                dbzOffset, dbzScale,
                vradOffset, vradScale, vradMissing,
                nRang, nAzim);

        texture = texImage.clone();

    }



    public int findCells() {

        int[] dbzImage = reflectivity.getScanDataRaw();

        int[] rhoImage = null;
        int[] zdrImage = null;

        int dbzMissing = reflectivity.getMissingValueValue();
        int dbznAzim = reflectivity.getNumberOfAzimuthBins();
        int dbznRang = reflectivity.getNumberOfRangeBins();
        float dbzRangeScale = (float) reflectivity.getRangeScale();
        float dbzValueOffset = (float) reflectivity.getDataOffset();
        float dbzValueScale = (float) reflectivity.getDataScale();

        float dbzThresMin = 0f;
        float rCellMax = 0f;
        int sign = 0;

        int nCells;

        nCells = findCells(dbzImage, cellImage, 
                dbzMissing, dbznAzim, dbznRang, dbzValueOffset, dbzRangeScale, dbzValueScale, dbzThresMin,
                rCellMax, sign);

        return nCells;

    };



    public int[] getTexture() throws Exception {

        if (texture == null) {
            throw new Exception("The texture array hasn't been calculated yet.");
        }

        return texture.clone();
    };

    
    
    public int[] getCellImage() throws Exception {

        if (cellImage == null) {
            throw new Exception("The cellImage array hasn't been calculated yet.");
        }

        return cellImage.clone();
    };
    


    protected CellProperties sortCells(CellProperties cellPropIn) throws Exception {

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

        sortCells(iRangOfMax, iAzimOfMax, dbzAvg, texAvg, cv, area, clutterArea, dbzMax, index, drop, nCells);

        CellProperties cellPropOut = cellPropIn.clone();

        cellPropOut.copyCellPropertiesFrom(iRangOfMax, iAzimOfMax, dbzAvg, texAvg, cv, area, clutterArea, dbzMax, index, drop);

        return cellPropOut;

    }



    protected int[] updateMap(CellProperties cellProp, int nCells, int nGlobal, int minCellArea) throws Exception {

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

        cellProp.copyCellPropertiesTo(iRangOfMax, iAzimOfMax, dbzAvg, texAvg, cv, area, clutterArea, dbzMax, index, drop);

        int nCellsValid;

        int[] cellImage = new int[nGlobal];

        nCellsValid = updateMap(cellImage, iRangOfMax, iAzimOfMax, dbzAvg, texAvg, cv, area, clutterArea, dbzMax, index, drop, nCells, nGlobal, minCellArea);

        cellProp.copyCellPropertiesFrom(iRangOfMax, iAzimOfMax, dbzAvg, texAvg, cv, area, clutterArea, dbzMax, index, drop);

        return cellImage;

    }



    public int getNumberOfAzimuthBins() {

        return this.reflectivity.getNumberOfAzimuthBins();
    }



    public int getNumberOfRangeBins() {

        return this.reflectivity.getNumberOfRangeBins();
    }



    public int[] getReflectivityRaw() {

        return this.reflectivity.getScanDataRaw();

    }



    public int[] getRadialVelocityRaw() {

        return this.radialVelocity.getScanDataRaw();

    }

}
