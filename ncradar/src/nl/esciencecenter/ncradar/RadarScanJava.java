package nl.esciencecenter.ncradar;

import java.io.File;
import java.io.IOException;
import ucar.ma2.Array;
import ucar.ma2.Index;
import ucar.nc2.NetcdfFile;
import ucar.nc2.Variable;


public class RadarScanJava extends NetcdfAttributeReader {

    private final PolarData polarData;
    private final ScanMeta scanMeta;
    private final RadarMeta radarMeta;



    public RadarScanJava(String directory, String filename, int datasetIndex) throws IOException {

        String datasetName = "dataset" + (datasetIndex + 1);
        String fullFilename = directory + File.separator + filename;

        this.radarMeta = new RadarMeta(fullFilename);

        double dataOffset = readDataOffset(fullFilename, datasetName);
        byte[] scanDataRaw = readScanDataRaw(fullFilename, datasetName);
        double dataScale = readDataScale(fullFilename, datasetName);
        int missingValueValue = readMissingValueValue(fullFilename, datasetName);
        int numberOfAzimuthBins = readNumberOfAzimuthBins(fullFilename, datasetName);
        int numberOfRangeBins = readNumberOfRangeBins(fullFilename, datasetName);
        double rangeOffset = readRangeOffset(fullFilename, datasetName);
        double rangeScale = readRangeScale(fullFilename, datasetName);
        int iAzimFirstRay = readiAzimFirstRay(fullFilename, datasetName);

        this.polarData = new PolarData(numberOfAzimuthBins, numberOfRangeBins, scanDataRaw,
                dataOffset, dataScale,
                rangeOffset, rangeScale,
                missingValueValue, iAzimFirstRay);

        double elevationAngle = readElevationAngle(fullFilename, datasetName);
        String endDate = readEndDate(fullFilename, datasetName);
        String endTime = readEndTime(fullFilename, datasetName);
        String scanType = readScanType(fullFilename, datasetName);
        String startDate = readStartDate(fullFilename, datasetName);
        String startTime = readStartTime(fullFilename, datasetName);

        this.scanMeta = new ScanMeta(datasetIndex, datasetName, directory,
                elevationAngle, endDate, endTime,
                filename, scanType, startDate, startTime);

    }



    public void calcVerticesAndFaces() {

        polarData.calcVerticesAndFaces();
    }



    public double getDataOffset() {

        return polarData.getDataOffset();
    }



    public double getDataScale() {

        return polarData.getDataScale();
    }



    public int getDatasetIndex() {

        return scanMeta.getDatasetIndex();
    }



    public String getDatasetName() {

        return scanMeta.getDatasetName();
    }



    public String getDirectory() {

        return scanMeta.getDirectory();
    }



    public double getElevationAngle() {

        return scanMeta.getElevationAngle();
    }



    public String getEndDate() {

        return scanMeta.getEndDate();
    }



    public String getEndTime() {

        return scanMeta.getEndTime();
    }



    public int[][] getFaces() {

        return polarData.getFaces();
    }



    public double[] getFacesValues() {

        double[][] scanData = this.getScanData2D();
        int nAzim = this.getNumberOfAzimuthBins();
        int nRang = this.getNumberOfRangeBins();
        double[] facesValues = new double[2 * nAzim * nRang];
        int iFace = 0;

        for (int iRang = 0; iRang < nRang; iRang++) {
            for (int iAzim = 0; iAzim < nAzim; iAzim++) {

                double v = scanData[iAzim][iRang];
                facesValues[iFace] = v;
                facesValues[iFace + 1] = v;
                iFace = iFace + 2;
            }
        }
        return facesValues;
    }



    public String getFilename() {

        return scanMeta.getFilename();
    }



    public int getiAzimFirstRay() {

        return polarData.getiAzimFirstRay();
    }



    public int getMissingValueValue() {

        return polarData.getMissingValueValue();
    }



    public float getNominalMaxTransmissionPower() {

        return radarMeta.getNominalMaxTransmissionPower();
    }



    public int getNumberOfAzimuthBins() {

        return polarData.getNumberOfAzimuthBins();
    }



    public int getNumberOfRangeBins() {

        return polarData.getNumberOfRangeBins();
    }



    public PolarData getPolarData() {

        return polarData.clone();
    }



    public float getPulseLength() {

        return radarMeta.getPulseLength();
    }



    public float getPulseRepeatFrequencyHigh() {

        return radarMeta.getPulseRepeatFrequencyHigh();
    }



    public float getPulseRepeatFrequencyLow() {

        return radarMeta.getPulseRepeatFrequencyLow();
    }



    public float getRadarConstant() {

        return radarMeta.getRadarConstant();
    }



    public double getRadarPositionHeight() {

        return radarMeta.getRadarPositionHeight();
    }



    public double getRadarPositionLatitude() {

        return radarMeta.getRadarPositionLatitude();
    }



    public double getRadarPositionLongitude() {

        return radarMeta.getRadarPositionLongitude();
    }



    public float getRadialVelocityAntenna() {

        return radarMeta.getRadialVelocityAntenna();
    }



    public double getRangeOffset() {

        return polarData.getRangeOffset();
    }



    public double getRangeScale() {

        return polarData.getRangeScale();
    }



    public double[] getScanData() {

        return polarData.getData();
    }



    public double[][] getScanData2D() {

        return polarData.getData2D();
    }



    public byte[] getScanDataRaw() {

        return polarData.getDataRaw();
    }



    public byte[][] getScanDataRaw2D() {

        return polarData.getDataRaw2D();
    }



    public String getScanType() {

        return scanMeta.getScanType();
    }



    public String getStartDate() {

        return scanMeta.getStartDate();
    }



    public String getStartTime() {

        return scanMeta.getStartTime();
    }



    public double[][] getVertices() {

        return polarData.getVertices();
    }



    private double readDataOffset(String fullFilename, String datasetName) throws IOException {

        String fullAttributename = datasetName + "_data1_what_offset";
        return readDoubleAttribute(fullFilename, fullAttributename);
    }



    private double readDataScale(String fullFilename, String datasetName) throws IOException {

        String fullAttributename = datasetName + "_data1_what_gain";
        return readDoubleAttribute(fullFilename, fullAttributename);
    }



    private double readElevationAngle(String fullFilename, String datasetName) throws IOException {

        String fullAttributename = datasetName + "_where_elangle";
        return readDoubleAttribute(fullFilename, fullAttributename);
    }



    private String readEndDate(String fullFilename, String datasetName) throws IOException {

        String fullAttributename = datasetName + "_what_enddate";
        return readStringAttribute(fullFilename, fullAttributename);
    }



    private String readEndTime(String fullFilename, String datasetName) throws IOException {

        String fullAttributename = datasetName + "_what_endtime";
        return readStringAttribute(fullFilename, fullAttributename);
    }



    private int readiAzimFirstRay(String fullFilename, String datasetName) throws IOException {

        String fullAttributename = datasetName + "_where_a1gate";
        return (int) readLongAttribute(fullFilename, fullAttributename);

    }



    private int readMissingValueValue(String fullFilename, String datasetName) throws IOException {

        String fullAttributename = datasetName + "_data1_what_nodata";
        return readIntegerAttribute(fullFilename, fullAttributename);
    }



    private int readNumberOfAzimuthBins(String fullFilename, String datasetName) throws IOException {

        String fullAttributename = datasetName + "_where_nrays";

        return (int) readLongAttribute(fullFilename, fullAttributename);

    }



    private int readNumberOfRangeBins(String fullFilename, String datasetName) throws IOException {

        String fullAttributename = datasetName + "_where_nbins";

        return (int) readLongAttribute(fullFilename, fullAttributename);

    }



    private double readRangeOffset(String fullFilename, String datasetName) throws IOException {

        String fullAttributename = datasetName + "_where_rstart";
        return readDoubleAttribute(fullFilename, fullAttributename);
    }



    private double readRangeScale(String fullFilename, String datasetName) throws IOException {

        String fullAttributename = datasetName + "_where_rscale";
        return readDoubleAttribute(fullFilename, fullAttributename);
    }



    private byte[] readScanDataRaw(String fullFilename, String datasetName) throws IOException {

        NetcdfFile ncfile = null;
        try {
            ncfile = NetcdfFile.open(fullFilename);
            Variable var = ncfile.findVariable(datasetName + "/data1/data");
            Array data = var.read();

            int[] dims = data.getShape();

            Index index = data.getIndex();
            int nRows = dims[0];
            int nCols = dims[1];
            int iGlobal = 0;

            byte[] scanDataRaw = new byte[nRows * nCols];

            for (int iRow = 0; iRow < nRows; iRow++) {
                for (int iCol = 0; iCol < nCols; iCol++) {
                    scanDataRaw[iGlobal] = data.getByte(index
                            .set(iRow, iCol));
                }
            }

            return scanDataRaw;

        } finally {
            ncfile.close();
        }

    }



    private String readScanType(String fullFilename, String datasetName) throws IOException {

        String fullAttributename = datasetName + "_data1_what_quantity";
        return readStringAttribute(fullFilename, fullAttributename);
    }



    private String readStartDate(String fullFilename, String datasetName) throws IOException {

        String fullAttributename = datasetName + "_what_startdate";
        return readStringAttribute(fullFilename, fullAttributename);
    }



    private String readStartTime(String fullFilename, String datasetName) throws IOException {

        String fullAttributename = datasetName + "_what_starttime";
        return readStringAttribute(fullFilename, fullAttributename);
    }

}
