
package nl.esciencecenter.ncradar;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.PrintWriter;
import java.io.UnsupportedEncodingException;

import ucar.ma2.Array;
import ucar.ma2.Index;
import ucar.nc2.NetcdfFile;
import ucar.nc2.Variable;

public class RadarScanJava extends NetcdfAttributeReader {

    private int datasetIndex;
    private String directory;
    private String filename;
    private String scanType;
    private double[][][][] polygons;
    private double elevationAngle;
    private double gain;
    private double offset;
    private byte[][] scanDataRaw;
    private double[][] scanData;
    private String startDate;
    private String startTime;
    private String endDate;
    private String endTime;
    private double radarPositionHeight;
    private double radarPositionLatitude;
    private double radarPositionLongitude;
    private String datasetName;
    private int missingValue;
    private long numberOfAzimuthBins;
    private long numberOfRangeBins;
    private double[][] vertices;
    private int[][] faces;
    private double rangeScale;
    private double rangeOffset;
    private long scanStartsAtAzimuthBin;

    public RadarScanJava(String directory,String filename,int datasetIndex) throws IOException {

        this.datasetIndex = datasetIndex;
        this.datasetName = "dataset"+(datasetIndex+1);
        this.directory = directory;
        this.filename = filename;
        this.scanType = readScanType();
        this.gain = readGain();
        this.offset = readOffset();
        this.elevationAngle = readElevationAngle();
        this.startDate = readStartDate();
        this.startTime = readStartTime();
        this.endDate = readEndDate();
        this.endTime = readEndTime();
        this.radarPositionHeight = readRadarPositionHeight();
        this.radarPositionLatitude = readRadarPositionLatitude();
        this.radarPositionLongitude = readRadarPositionLongitude();
        this.missingValue = readMissingValue();
        this.numberOfAzimuthBins = readNumberOfAzimuthBins();
        this.numberOfRangeBins = readNumberOfRangeBins();
        this.scanDataRaw = readScanDataRaw();
        this.rangeScale = readRangeScale();
        this.rangeOffset = readRangeOffset();
        this.scanStartsAtAzimuthBin = readScanStartsAtAzimuthBin();
    }

    public double[][][][] calcPolygons(){

        double angleTrailing = 0;
        double angleLeading = 0;
        double range = 0.50;
        double[][][][] polygons = new double[(int)numberOfRangeBins][(int)numberOfAzimuthBins][5][2];

        for (int iBin=0;iBin<numberOfRangeBins;iBin++){

            double lengthInner = ((double) iBin/numberOfRangeBins) * range;
            double lengthOuter = ((double) (iBin+1)/numberOfRangeBins) * range;

            for (int iRay=0;iRay<numberOfAzimuthBins;iRay++){

                angleTrailing = ((double) iRay/numberOfAzimuthBins)*2*Math.PI;
                angleLeading = ((double) (iRay+1)/numberOfAzimuthBins)*2*Math.PI;
                
                polygons[iBin][iRay][0][0] = radarPositionLongitude + Math.sin(angleTrailing)*lengthOuter;
                polygons[iBin][iRay][0][1] = radarPositionLatitude + Math.cos(angleTrailing)*lengthOuter;

                polygons[iBin][iRay][1][0] = radarPositionLongitude + Math.sin(angleLeading)*lengthOuter;
                polygons[iBin][iRay][1][1] = radarPositionLatitude + Math.cos(angleLeading)*lengthOuter;

                polygons[iBin][iRay][2][0] = radarPositionLongitude + Math.sin(angleLeading)*lengthInner;
                polygons[iBin][iRay][2][1] = radarPositionLatitude + Math.cos(angleLeading)*lengthInner;

                polygons[iBin][iRay][3][0] = radarPositionLongitude + Math.sin(angleTrailing)*lengthInner;
                polygons[iBin][iRay][3][1] = radarPositionLatitude + Math.cos(angleTrailing)*lengthInner;

                // close the polygons
                polygons[iBin][iRay][4][0] = polygons[iBin][iRay][0][0];
                polygons[iBin][iRay][4][1] = polygons[iBin][iRay][0][1];

            }
        }

        return polygons;

    }

    public void calcVerticesAndFaces(){
        
        int iVertex;
        int iFace;
        int nRang = (int) getNumberOfRangeBins();
        int nAzim = (int) getNumberOfAzimuthBins();
        
        int nVertices = (nRang+1) * nAzim;
        int nFaces = nRang * nAzim * 2;
        float rangeScale = 5.0f;   //FIXME
        float minRange = 5.5f * rangeScale; // FIXME
        float azimuthScale = 1.0f;  //FIXME
        float azimuthScaleRad = (float) (2 * Math.PI * azimuthScale/360) ;  //FIXME
        
        vertices = new double[nVertices][2];
        for (int iRang = 0; iRang <= nRang; iRang++) {
            double s = minRange + rangeScale*(iRang-0.5);
            for (int iAzim = 0; iAzim < nAzim; iAzim++) {
                
                iVertex = iRang * nAzim + iAzim;
                double alpha = azimuthScaleRad * (iAzim-0.5);
                vertices[iVertex] = new double[]{Math.sin(alpha) * s,Math.cos(alpha) * s};

            } //for iAzim
        }// for iRang
        
        faces = new int [nFaces][3];
        iFace = 0;
        int delta;
        for (int iRang = 0; iRang < nRang; iRang++) {
            for (int iAzim = 0; iAzim < nAzim; iAzim++) {
                
                iVertex = iRang * nAzim + iAzim;

                if (iAzim == nAzim - 1) {
                    delta = -nAzim;
                }
                else {
                    delta = 0;
                }
                    
                faces[iFace] = new int[]{iVertex,
                                         iVertex + 1 + delta,
                                         iVertex + 1 + nAzim + delta};
                iFace = iFace + 1;
                faces[iFace] = new int[]{iVertex,
                                         iVertex + 1 + nAzim + delta,
                                         iVertex + nAzim};
                iFace = iFace + 1;

            } //for iAzim
        }// for iRang

        
    }

    public void convertScanDataRaw() {
        
        int nRows;
        int nCols;
        double gain;
        double offset;
        
        nRows = (int) getNumberOfAzimuthBins();
        nCols = (int) getNumberOfRangeBins();
        scanData = new double[nRows][nCols];
        gain = getGain();
        offset = getOffset();
        
        for (int iRow=0;iRow<nRows;iRow++) {
            for (int iCol=0;iCol<nCols;iCol++) {

                Number number = scanDataRaw[iRow][iCol];
                scanData[iRow][iCol] = offset + gain * number.doubleValue();
            }
        }
        
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
        return faces.clone();
    }

    public double[] getFacesValues() {
        
        double[][] scanData = this.getScanData();
        int nAzim = (int) this.getNumberOfAzimuthBins();
        int nRang = (int) this.getNumberOfRangeBins();
        double[] facesValues = new double[2*nAzim*nRang];
        int iFace = 0;
        
        for (int iRang = 0; iRang < nRang; iRang++) {
            for (int iAzim = 0; iAzim < nAzim; iAzim++) {
                
                double v =  scanData[iAzim][iRang];
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

    public double getGain() {
        return gain;
    }
    
    public int getMissingValue() {
        return missingValue;
    }

    public long getNumberOfAzimuthBins() {
        return numberOfAzimuthBins;
    }

    public long getNumberOfRangeBins() {
        return numberOfRangeBins;
    }
    
    public double getOffset() {
        return offset;
    }

    public double[][][][] getPolygons() {
        return polygons.clone();
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
    
    
    public double getRangeOffset() {
        return rangeOffset;
    }

    public double getRangeScale() {
        return rangeScale;
    }

    public double[][] getScanData() {
        this.convertScanDataRaw();
        return scanData.clone();
    }

    public byte[][] getScanDataRaw() {
        return scanDataRaw.clone();
    }
    
    public long getScanStartsAtAzimuthBin() {
        return scanStartsAtAzimuthBin;
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

    public double[][] getVertices() {
        return vertices.clone();
    }
    
    public void printAsGeoJSONToCSV() throws FileNotFoundException, UnsupportedEncodingException {

       String filename = this.filename+".geojson.csv";
       printAsGeoJSONToCSV(filename);
       
    }

    public void printAsGeoJSONToCSV(String filename) throws FileNotFoundException, UnsupportedEncodingException {

        PrintWriter writer = new PrintWriter(filename, "UTF-8");

        int nPolyCorners = 5;
        String headerStr = "\"the_geom\",\"reflectivity\"";
        writer.println(headerStr);
        
        for (int iBin=0;iBin<numberOfRangeBins;iBin++){
            
            for (int iRay=0;iRay<numberOfAzimuthBins;iRay++){

                String polyStr = "\"{\"\"type\"\":\"\"MultiPolygon\"\",\"\"coordinates\"\":[[[";
                
                
                for (int iPolyCorner=0;iPolyCorner<nPolyCorners;iPolyCorner++){
                    polyStr = polyStr + String.format("[%.6f,%.6f]", polygons[iBin][iRay][iPolyCorner][0],polygons[iBin][iRay][iPolyCorner][1]);
                    if (iPolyCorner<nPolyCorners-1){
                        
                        polyStr = polyStr + ",";                        
                        
                    }
                }

                polyStr = polyStr + "]]]}\"";

                polyStr = polyStr + "," + scanDataRaw[iRay][iBin];
                
                writer.println(polyStr);

            }
        }

        writer.close();

    }

    public void printAsWKTToCSV() throws FileNotFoundException, UnsupportedEncodingException {

        String filename = this.filename+".wkt.csv";
        printAsWKTToCSV(filename);

    }

    public void printAsWKTToCSV(String filename) throws FileNotFoundException, UnsupportedEncodingException {


        PrintWriter writer = new PrintWriter(filename, "UTF-8");

        int nPolyCorners = 5;
        int iBinMin = 0;
        int iBinMax = (int) numberOfRangeBins;
        int iRayMin = 0;
        int iRayMax = (int) numberOfAzimuthBins;

        String headerStr = "\"the_geom\",\"reflectivity\"";
        writer.println(headerStr);


        for (int iBin=0;iBin<numberOfRangeBins;iBin++){

            for (int iRay=0;iRay<numberOfAzimuthBins;iRay++){

                if (iBinMin<=iBin & iBin<=iBinMax & iRayMin<=iRay & iRay<=iRayMax){
                String polyStr = "\"POLYGON((";

                for (int iPolyCorner=0;iPolyCorner<nPolyCorners;iPolyCorner++){
                    polyStr = polyStr + String.format("%.6f %.6f", polygons[iBin][iRay][iPolyCorner][0],polygons[iBin][iRay][iPolyCorner][1]);
                    if (iPolyCorner<nPolyCorners-1){

                        polyStr = polyStr + ",";

                    }
                }
                polyStr = polyStr + "))\"";

                polyStr = polyStr + "," + scanDataRaw[iRay][iBin];

                writer.println(polyStr);

                }

            }
        }

        writer.close();

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

    private double readGain() throws IOException {

        String fullFilename = this.directory + File.separator + this.filename;
        String fullAttributename = this.datasetName + "_data1_what_gain";
        return readDoubleAttribute(fullFilename, fullAttributename);
    }

    private int readMissingValue() throws IOException {

        String fullFilename = this.directory + File.separator + this.filename;
        String fullAttributename = this.datasetName + "_data1_what_nodata";
        return readIntegerAttribute(fullFilename, fullAttributename);
    }

    private long readNumberOfAzimuthBins() throws IOException {

        String fullFilename = this.directory + File.separator + this.filename;
        String fullAttributename = this.datasetName + "_where_nrays";

        return readLongAttribute(fullFilename,fullAttributename);

    }

    private long readNumberOfRangeBins() throws IOException {

        String fullFilename = this.directory + File.separator + this.filename;
        String fullAttributename = this.datasetName + "_where_nbins";

        return readLongAttribute(fullFilename,fullAttributename);

    }

    private double readOffset() throws IOException {

        String fullFilename = this.directory + File.separator + this.filename;
        String fullAttributename = this.datasetName + "_data1_what_offset";
        return readDoubleAttribute(fullFilename, fullAttributename);
    }

    private double readRadarPositionHeight() throws IOException {

        String fullFilename = this.directory + File.separator + this.filename;
        String fullAttributename =  "where_height";
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

    private byte[][] readScanDataRaw() throws IOException {

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
            
            byte[][] scanDataRaw = new byte[nRows][nCols];

            for (int iRow=0;iRow<nRows;iRow++){
                for (int iCol=0;iCol<nCols;iCol++){
                    scanDataRaw[iRow][iCol] = data.getByte(index.set(iRow,iCol));
                }
            }
            
            return scanDataRaw;
            
        } finally {
            ncfile.close();
        }

    }
    
    private long readScanStartsAtAzimuthBin() throws IOException {

            String fullFilename = this.directory + File.separator + this.filename;
            String fullAttributename = this.datasetName + "_where_a1gate";
            return readLongAttribute(fullFilename,fullAttributename);

    }
    
    private String readScanType() throws IOException {

        String fullFilename = this.directory + File.separator + this.filename;
        String fullAttributename = this.datasetName+"_data1_what_quantity";
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
    
    
}



