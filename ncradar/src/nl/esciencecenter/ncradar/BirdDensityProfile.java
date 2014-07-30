package nl.esciencecenter.ncradar;

public class BirdDensityProfile extends JNIMethodsVol2Bird {

    private final RadarScanJava reflectivity;
    private final RadarScanJava radialVelocity;
    private final RadarScanJava corrCoefRho;
    private final RadarScanJava differentialReflectivity;
    private int[] texture;
    private final int nRang;
    private final int nAzim;



    public BirdDensityProfile(RadarScanJava reflectivity, RadarScanJava radialVelocity) throws Exception {

        this.reflectivity = reflectivity;
        this.radialVelocity = radialVelocity;
        this.corrCoefRho = null;
        this.differentialReflectivity = null;

        nAzim = reflectivity.getNumberOfAzimuthBins();
        nRang = reflectivity.getNumberOfRangeBins();

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



    public BirdDensityProfile(RadarScanJava reflectivity, RadarScanJava radialVelocity,
            RadarScanJava corrCoeffRho, RadarScanJava differentialReflectivity) throws Exception {

        this.reflectivity = reflectivity;
        this.radialVelocity = radialVelocity;
        this.corrCoefRho = corrCoeffRho;
        this.differentialReflectivity = differentialReflectivity;

        nAzim = reflectivity.getNumberOfAzimuthBins();
        nRang = reflectivity.getNumberOfRangeBins();

        {
            int nAzim = radialVelocity.getNumberOfAzimuthBins();
            int nRang = radialVelocity.getNumberOfRangeBins();

            if (nAzim != this.nAzim) {
                throw new Exception("Number of azimuth bins differ (vrad).");
            }
            if (nRang != this.nRang) {
                throw new Exception("Number of range bins differ (vrad).");
            }
        }
        {
            int nAzim = corrCoeffRho.getNumberOfAzimuthBins();
            int nRang = corrCoeffRho.getNumberOfRangeBins();

            if (nAzim != this.nAzim) {
                throw new Exception("Number of azimuth bins differ (rho).");
            }
            if (nRang != this.nRang) {
                throw new Exception("Number of range bins differ (rho).");
            }
        }
        {
            int nAzim = differentialReflectivity.getNumberOfAzimuthBins();
            int nRang = differentialReflectivity.getNumberOfRangeBins();

            if (nAzim != this.nAzim) {
                throw new Exception("Number of azimuth bins differ (zdr).");
            }
            if (nRang != this.nRang) {
                throw new Exception("Number of range bins differ (zdr).");
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



    //
    //
    // public int findCells() {
    //
    // int[] texImage = getTextureRaw();
    //
    // int[] cellImage = new int[nAzim * nRang];
    //
    // int[] rhoImage = null;
    // int[] zdrImage = null;
    //
    // int texMissing = 0;
    // int texnAzim = nAzim;
    // int texnRang = nRang;
    // float texRangeScale = 0f;
    // float texValueOffset = 0f;
    // float texValueScale = 1f;
    //
    // int rhoMissing = this.corrCoefRho.getMissingValueValue();
    // int rhonAzim = this.corrCoefRho.getNumberOfAzimuthBins();
    // int rhonRang = this.corrCoefRho.getNumberOfRangeBins();
    // float rhoValueOffset = (float) this.corrCoefRho.getDataOffset();
    // float rhoValueScale = (float) this.corrCoefRho.getDataScale();
    //
    // int zdrMissing = this.differentialReflectivity.getMissingValueValue();
    // int zdrnAzim = this.differentialReflectivity.getNumberOfAzimuthBins();
    // int zdrnRang = this.differentialReflectivity.getNumberOfRangeBins();
    // float zdrValueOffset = (float)
    // this.differentialReflectivity.getDataOffset();
    // float zdrValueScale = (float)
    // this.differentialReflectivity.getDataScale();
    //
    // float texThresMin = 0f;
    // float dbzThresMin = 0f;
    // float rhoThresMin = 0f;
    // float zdrThresMin = 0f;
    // float rCellMax = 0f;
    // int sign = 0;
    //
    // int nCells;
    //
    // nCells = findCells(texImage, rhoImage, zdrImage, cellImage,
    // texMissing, texnAzim, texnRang, texValueOffset, texRangeScale,
    // texValueScale, texThresMin,
    // rhoMissing, rhonAzim, rhonRang, rhoValueOffset, rhoValueScale,
    // rhoThresMin,
    // zdrMissing, zdrnAzim, zdrnRang, zdrValueOffset, zdrValueScale,
    // zdrThresMin, dbzThresMin, rCellMax, sign);
    //
    // return nCells;
    //
    // };
    //

    public double[][] getTexture() {

        int[] textureRaw = getTextureRaw();
        double[][] texture = new double[nAzim][nRang];
        int iGlobal = 0;

        for (int iAzim = 0; iAzim < nAzim; iAzim++) {
            for (int iRang = 0; iRang < nRang; iRang++) {
                Number number = textureRaw[iGlobal];
                texture[iAzim][iRang] = number.doubleValue();
                iGlobal++;
            }
        }

        return texture;

    }



    protected int[] getTextureRaw() {

        return texture.clone();
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

}
