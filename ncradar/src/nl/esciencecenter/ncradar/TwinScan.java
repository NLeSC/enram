package nl.esciencecenter.ncradar;

public class TwinScan extends JNIMethodsVol2Bird {

    private final RadarScanJava reflectivity;
    private final RadarScanJava radialVelocity;
    private final ParameterValues parameterValues;
    private int[] texture;
    private int[] cellImage;
    private int[] clutterImage;
    private final int nRang;
    private final int nAzim;



    public TwinScan(RadarScanJava reflectivity, RadarScanJava radialVelocity, ParameterValues parameterValues) throws Exception {

        this.reflectivity = reflectivity;
        this.radialVelocity = radialVelocity;
        this.parameterValues = parameterValues;

        nAzim = reflectivity.getNumberOfAzimuthBins();
        nRang = reflectivity.getNumberOfRangeBins();
        cellImage = new int[nAzim * nRang];
        clutterImage = new int[nAzim * nRang];
        
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



    public int analyzeCells(int nCells, int cmFlagInt, int verboseInt) throws Exception {

        int[] dbzImage = this.reflectivity.getScanDataRaw();
        int[] vradImage = this.radialVelocity.getScanDataRaw();
        int[] texImage = this.getTexture();
        int[] clutterImage = this.getClutterImage();
        int[] cellImage = this.getCellImage();
        int dbznRang = this.reflectivity.getNumberOfRangeBins();
        int dbznAzim = this.radialVelocity.getNumberOfAzimuthBins();
        float dbzElev = (float) this.reflectivity.getElevationAngle();
        float dbzValueScale = (float) this.reflectivity.getDataScale();
        float dbzValueOffset = (float) this.reflectivity.getDataOffset();
        float vradValueScale = (float) this.radialVelocity.getDataScale();
        float vradValueOffset = (float) this.radialVelocity.getDataOffset();
        float clutterValueScale = 1.0f;
        float clutterValueOffset = 0.0f;
        float texValueScale = (float) this.parameterValues.getSTDEVSCALE();
        float texValueOffset = 0;

        
        int areaMin = this.parameterValues.getAREACELL();
        float cellDbzMin = (float) this.parameterValues.getDBZCELL();
        float cellStdDevMax = (float) this.parameterValues.getSTDEVCELL();
        float cellClutterFraction = (float) this.parameterValues.getCLUTPERCCELL();
        float vradMinValue = (float) parameterValues.getVRADMIN();
        float clutterValueMax = (float) this.parameterValues.getDBZCLUTTER();
        
        
        int nCellsValid = analyzeCells(dbzImage, vradImage, texImage,
                clutterImage, cellImage, dbznRang, dbznAzim,
                dbzElev, dbzValueScale, dbzValueOffset,
                vradValueScale, vradValueOffset,
                clutterValueScale, clutterValueOffset,
                texValueScale, texValueOffset,
                nCells, areaMin, cellDbzMin, cellStdDevMax,
                cellClutterFraction, vradMinValue, clutterValueMax,
                cmFlagInt, verboseInt);
        
        
        this.cellImage = cellImage.clone();
        
        int iGlobal;
        int nGlobal = cellImage.length;

        int minValue = cellImage[0];
        int maxValue = cellImage[0];
        for (iGlobal = 1;iGlobal < nGlobal;iGlobal++) {
            if (cellImage[iGlobal] < minValue) {
                minValue = cellImage[iGlobal];
            }
            if (cellImage[iGlobal] > maxValue) {
                maxValue = cellImage[iGlobal];
            }
        }
        System.out.println("minimum value in cellImage array = " + minValue);
        System.out.println("maximum value in cellImage array = " + maxValue);
        
        
        return nCellsValid; 
        
    }
    
    
    
    

    public void calcTexture() {

        int[] dbzImage = reflectivity.getScanDataRaw();
        int[] vradImage = radialVelocity.getScanDataRaw();
        int[] texImage = new int[nAzim * nRang];

        float dbzOffset = (float) reflectivity.getDataOffset();
        float dbzScale = (float) reflectivity.getDataScale();
        float vradOffset = (float) radialVelocity.getDataOffset();
        float vradScale = (float) radialVelocity.getDataScale();
        float texOffset = 0;
        float texScale = (float) this.parameterValues.getSTDEVSCALE();
        
        int nRangNeighborhood = this.parameterValues.getNTEXBINRANG();
        int nAzimNeighborhood = this.parameterValues.getNTEXBINAZIM();
        int nCountMin = this.parameterValues.getNTEXMIN();
        int vradMissing = this.radialVelocity.getMissingValueValue();
        
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
        int dbzMissing = reflectivity.getMissingValueValue();
        int dbznAzim = reflectivity.getNumberOfAzimuthBins();
        int dbznRang = reflectivity.getNumberOfRangeBins();
        float dbzRangeScale = (float) reflectivity.getRangeScale();
        float dbzValueOffset = (float) reflectivity.getDataOffset();
        float dbzValueScale = (float) reflectivity.getDataScale();
        float dbzThresMin = (float) this.parameterValues.getDBZMIN();
        int rCellMax = (int) (this.parameterValues.getRANGMAX());
        
        int nCells = findCells(dbzImage, cellImage, dbzMissing, dbznAzim, dbznRang, dbzValueOffset, 
                dbzRangeScale, dbzValueScale, dbzThresMin, rCellMax);

        return nCells;
    };

    
    
    
    
    public void fringeCells() throws Exception {

        
        int nRang = this.reflectivity.getNumberOfRangeBins();
        int nAzim = this.reflectivity.getNumberOfAzimuthBins();
        float azimuthScale = (float) this.reflectivity.getAzimuthScaleDeg();
        float rangeScale = (float) this.reflectivity.getRangeScale();
        float fringeDist = (float) this.parameterValues.getEMASKMAX();
        
        fringeCells(cellImage, nRang, nAzim, azimuthScale, rangeScale, fringeDist);

    }
    

    
    public void getListOfSelectedGates(int iLayer, int iData) throws Exception {
        
        int nRang;
        int nAzim;
        int missing;
        int nRecordsMax;
        int iRecord;
        int nPoints;
        int iPoint;
        int iParFitted;
        int nDims;
        int nParsFitted;
        
        float absVradMin;
        float radarHeight;
        float vradValueOffset;
        float vradValueScale;
        float dbzValueOffset;
        float dbzValueScale;
        float rangeScale;
        float azimuthScale;
        float elevAngle;
        float altitudeMin;
        float altitudeMax;
        float rangeMin;
        float rangeMax;
        float layerThickness;
        
        int[] vradImageInt;
        int[] dbzImageInt;
        int[] cellImage;
        int[] listOfCellIds;
        
        float[] listOfAzimuths;
        float[] listOfElevAngles;
        float[] listOfVradObs;
        float[] listOfVradFitted;
        float[] listOfDbzObs;
        float[] parameterVector;
        float[] avar; 
        float[] points;
        
        
        nRang = this.getNumberOfRangeBins();
        nAzim = this.getNumberOfAzimuthBins();
        rangeScale = (float) this.reflectivity.getRangeScale();
        azimuthScale = (float) this.reflectivity.getAzimuthScaleDeg();
        elevAngle = (float) this.reflectivity.getElevationAngle();
        missing = this.reflectivity.getMissingValueValue();
        radarHeight = (float) this.reflectivity.radarMeta.getRadarPositionHeight();
        dbzValueOffset = (float) this.reflectivity.getDataOffset();
        dbzValueScale = (float) this.reflectivity.getDataScale();
        vradValueOffset = (float) this.radialVelocity.getDataOffset();
        vradValueScale = (float) this.radialVelocity.getDataScale();
        absVradMin = (float) this.parameterValues.getVRADMIN();
        layerThickness = (float) this.parameterValues.getHLAYER();
        altitudeMin = (float) (iLayer * layerThickness);
        altitudeMax = (float) ((iLayer+1.0f) * layerThickness);
        rangeMin = (float) this.parameterValues.getRANGMIN();
        rangeMax = (float) this.parameterValues.getRANGMAX();

        
        vradImageInt = this.radialVelocity.getScanDataRaw();
        dbzImageInt = this.reflectivity.getScanDataRaw();
        cellImage = this.getCellImage();
        
        nRecordsMax = this.detNumberOfGates(iLayer, layerThickness, rangeMin, rangeMax, rangeScale, elevAngle, nRang, nAzim, radarHeight);
        nPoints = 0;
        
        listOfAzimuths = new float[nRecordsMax];
        listOfElevAngles = new float[nRecordsMax];
        listOfVradObs = new float[nRecordsMax];
        listOfDbzObs = new float[nRecordsMax];
        listOfCellIds = new int[nRecordsMax];
        
        for (iRecord = 0; iRecord < nRecordsMax; iRecord++) {
            listOfAzimuths[iRecord] = Float.NaN;
            listOfElevAngles[iRecord] = Float.NaN;
            listOfVradObs[iRecord] = Float.NaN;
            listOfDbzObs[iRecord] = Float.NaN;
            listOfCellIds[iRecord] = (int) Float.NaN;
        }
        
        nPoints = getListOfSelectedGates(nRang, nAzim, rangeScale, azimuthScale, elevAngle, missing, 
                                         radarHeight, vradValueOffset, vradValueScale, vradImageInt,
                                         dbzValueOffset, dbzValueScale, dbzImageInt, 
                                         listOfAzimuths, listOfElevAngles, listOfVradObs, listOfDbzObs, listOfCellIds, 
                                         cellImage, rangeMin, rangeMax, altitudeMin, 
                                         altitudeMax, absVradMin, iData, nPoints);

        
        // TODO the part below this should be moved to a higher level, where you collect results from all scan elevation angles

        listOfVradFitted = new float[nPoints];
        for (iPoint = 0; iPoint < nPoints; iPoint++) {
            listOfVradFitted[iPoint] = Float.NaN; 
        }
        parameterVector = new float[] {Float.NaN,Float.NaN,Float.NaN};
        nParsFitted = 3;
        avar = new float[nParsFitted];
        for (iParFitted = 0; iParFitted < nParsFitted; iParFitted++) {
            avar[iParFitted] = Float.NaN; 
        }

        nDims = 2;
        points = new float[nDims * nPoints];
        
        for (iPoint = 0; iPoint < nPoints; iPoint++) {
            points[iPoint*nDims + 0] = listOfAzimuths[iPoint];
            points[iPoint*nDims + 1] = listOfElevAngles[iPoint];
        }
        
        float chisq = svdfit(points, nDims, listOfVradObs, listOfVradFitted, nPoints, parameterVector, avar, nParsFitted);


    }
    


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

    
    public double getAzimuthScale() {

        return this.reflectivity.getAzimuthScaleDeg();
        
    }
    
    public double getElevationAngle() {

        return this.reflectivity.getElevationAngle();
        
    }
    
    
    public int getMissingValueValue() {

        return this.reflectivity.getMissingValueValue();
        
    }
    
//    public float getRangeScale() {
//
//        return this.reflectivity.
//        
//    }
//    
//    public float getRangeScale() {
//
//        return (float) this.reflectivity.getRangeScale();
//        
//    }
//    
//    float radarHeight;
//    float valueOffset;
//    float valueScale;



    public int[] getReflectivityRaw() {

        return this.reflectivity.getScanDataRaw();

    }



    public int[] getRadialVelocityRaw() {

        return this.radialVelocity.getScanDataRaw();

    }



    public int[] getClutterImage() {

        return clutterImage;
    }


}
