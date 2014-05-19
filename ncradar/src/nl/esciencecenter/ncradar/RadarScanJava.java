package nl.esciencecenter.ncradar;

import java.io.File;
import java.io.IOException;
import ucar.ma2.Array;
import ucar.ma2.Index;
import ucar.nc2.NetcdfFile;
import ucar.nc2.Variable;


public class RadarScanJava extends NetcdfAttributeReader {

    private final int datasetIndex;
    private final String datasetName;
    private final String directory;
    private final double elevationAngle;
    private final String endDate;
    private final String endTime;
    private final String filename;
    private final String scanType;
    private final String startDate;
    private final String startTime;
    private final PolarData polarData;
    // stuff about the radar
    // private final double elevationAngles;
    private final float nominalMaxTransmissionPower;
    private final float pulseLength;
    private final float pulseRepeatFrequencyHigh;
    private final float pulseRepeatFrequencyLow;
    private final float radarConstant;
    private final double radarPositionHeight;
    private final double radarPositionLatitude;
    private final double radarPositionLongitude;
    private final float radialVelocityAntenna;



    public RadarScanJava(String directory, String filename, int datasetIndex) throws IOException {

        this.datasetIndex = datasetIndex;
        this.datasetName = "dataset" + (datasetIndex + 1);
        this.directory = directory;
        this.filename = filename;
        this.elevationAngle = readElevationAngle();
        this.endDate = readEndDate();
        this.endTime = readEndTime();
        this.radarPositionHeight = readRadarPositionHeight();
        this.radarPositionLatitude = readRadarPositionLatitude();
        this.radarPositionLongitude = readRadarPositionLongitude();
        this.scanType = readScanType();
        this.startDate = readStartDate();
        this.startTime = readStartTime();
        this.pulseRepeatFrequencyHigh = -1f;
        this.pulseRepeatFrequencyLow = -1f;
        this.pulseLength = -1f;
        this.radarConstant = -1f;
        this.nominalMaxTransmissionPower = -1f;
        this.radialVelocityAntenna = -1f;

        double dataOffset = readDataOffset();
        byte[] scanDataRaw = readScanDataRaw();
        double dataScale = readDataScale();
        byte missingValueValue = readMissingValueValue();
        int numberOfAzimuthBins = readNumberOfAzimuthBins();
        int numberOfRangeBins = readNumberOfRangeBins();
        double rangeOffset = readRangeOffset();
        double rangeScale = readRangeScale();
        int iAzimFirstRay = readiAzimFirstRay();

        this.polarData = new PolarData(numberOfAzimuthBins, numberOfRangeBins, scanDataRaw,
                dataOffset, dataScale,
                rangeOffset, rangeScale,
                missingValueValue, iAzimFirstRay);

    }



    public int getDatasetIndex() {

        return datasetIndex;
    }



    public String getDatasetName() {

        return datasetName;
    }



    public String getDirectory() {

        return directory;
    }



    public double getElevationAngle() {

        return elevationAngle;
    }



    public String getEndDate() {

        return endDate;
    }



    public String getEndTime() {

        return endTime;
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

        return filename;
    }



    public long getiAzimFirstRay() {

        return polarData.getiAzimFirstRay();
    }



    public int getMissingValueValue() {

        return polarData.getMissingValueValue();
    }



    public float getNominalMaxTransmissionPower() {

        return nominalMaxTransmissionPower;
    }



    public int getNumberOfAzimuthBins() {

        return polarData.getNumberOfAzimuthBins();
    }



    public int getNumberOfRangeBins() {

        return polarData.getNumberOfRangeBins();
    }



    public float getPulseLength() {

        return pulseLength;
    }



    public float getPulseRepeatFrequencyHigh() {

        return pulseRepeatFrequencyHigh;
    }



    public float getPulseRepeatFrequencyLow() {

        return pulseRepeatFrequencyLow;
    }



    public float getRadarConstant() {

        return radarConstant;
    }



    public double getRadarPositionHeight() {

        return radarPositionHeight;
    }



    public double getRadarPositionLatitude() {

        return radarPositionLatitude;
    }



    public double getRadarPositionLongitude() {

        return radarPositionLongitude;
    }



    public float getRadialVelocityAntenna() {

        return radialVelocityAntenna;
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

        return scanType;
    }



    public String getStartDate() {

        return startDate;
    }



    public String getStartTime() {

        return startTime;
    }



    public double getDataOffset() {

        return polarData.getDataOffset();
    }



    public double getDataScale() {

        return polarData.getDataScale();
    }



    public double[][] getVertices() {

        return polarData.getVertices();
    }



    private double readElevationAngle() throws IOException {

        String fullFilename = this.directory + File.separator + this.filename;
        String fullAttributename = this.datasetName + "_where_elangle";
        return readDoubleAttribute(fullFilename, fullAttributename);
    }



    private String readEndDate() throws IOException {

        String fullFilename = this.directory + File.separator + this.filename;
        String fullAttributename = this.datasetName + "_what_enddate";
        return readStringAttribute(fullFilename, fullAttributename);
    }



    private String readEndTime() throws IOException {

        String fullFilename = this.directory + File.separator + this.filename;
        String fullAttributename = this.datasetName + "_what_endtime";
        return readStringAttribute(fullFilename, fullAttributename);
    }



    private int readiAzimFirstRay() throws IOException {

        String fullFilename = this.directory + File.separator + this.filename;
        String fullAttributename = this.datasetName + "_where_a1gate";
        return (int) readLongAttribute(fullFilename, fullAttributename);

    }



    private byte readMissingValueValue() throws IOException {

        String fullFilename = this.directory + File.separator + this.filename;
        String fullAttributename = this.datasetName + "_data1_what_nodata";
        return (byte) readIntegerAttribute(fullFilename, fullAttributename);
    }



    private int readNumberOfAzimuthBins() throws IOException {

        String fullFilename = this.directory + File.separator + this.filename;
        String fullAttributename = this.datasetName + "_where_nrays";

        return (int) readLongAttribute(fullFilename, fullAttributename);

    }



    private int readNumberOfRangeBins() throws IOException {

        String fullFilename = this.directory + File.separator + this.filename;
        String fullAttributename = this.datasetName + "_where_nbins";

        return (int) readLongAttribute(fullFilename, fullAttributename);

    }



    private double readRadarPositionHeight() throws IOException {

        String fullFilename = this.directory + File.separator + this.filename;
        String fullAttributename = "where_height";
        return readDoubleAttribute(fullFilename, fullAttributename);
    }



    private double readRadarPositionLatitude() throws IOException {

        String fullFilename = this.directory + File.separator + this.filename;
        String fullAttributename = "where_lat";
        return readDoubleAttribute(fullFilename, fullAttributename);
    }



    private double readRadarPositionLongitude() throws IOException {

        String fullFilename = this.directory + File.separator + this.filename;
        String fullAttributename = "where_lon";
        return readDoubleAttribute(fullFilename, fullAttributename);
    }



    private double readRangeOffset() throws IOException {

        String fullFilename = this.directory + File.separator + this.filename;
        String fullAttributename = this.datasetName + "_where_rstart";
        return readDoubleAttribute(fullFilename, fullAttributename);
    }



    private double readRangeScale() throws IOException {

        String fullFilename = this.directory + File.separator + this.filename;
        String fullAttributename = this.datasetName + "_where_rscale";
        return readDoubleAttribute(fullFilename, fullAttributename);
    }



    private byte[] readScanDataRaw() throws IOException {

        String fullFilename = this.directory + File.separator + this.filename;

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



    private String readScanType() throws IOException {

        String fullFilename = this.directory + File.separator + this.filename;
        String fullAttributename = this.datasetName + "_data1_what_quantity";
        return readStringAttribute(fullFilename, fullAttributename);
    }



    private String readStartDate() throws IOException {

        String fullFilename = this.directory + File.separator + this.filename;
        String fullAttributename = this.datasetName + "_what_startdate";
        return readStringAttribute(fullFilename, fullAttributename);
    }



    private String readStartTime() throws IOException {

        String fullFilename = this.directory + File.separator + this.filename;
        String fullAttributename = this.datasetName + "_what_starttime";
        return readStringAttribute(fullFilename, fullAttributename);
    }



    private double readDataOffset() throws IOException {

        String fullFilename = this.directory + File.separator + this.filename;
        String fullAttributename = this.datasetName + "_data1_what_offset";
        return readDoubleAttribute(fullFilename, fullAttributename);
    }



    private double readDataScale() throws IOException {

        String fullFilename = this.directory + File.separator + this.filename;
        String fullAttributename = this.datasetName + "_data1_what_gain";
        return readDoubleAttribute(fullFilename, fullAttributename);
    }

}
