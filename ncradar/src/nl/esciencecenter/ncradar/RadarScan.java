
package nl.esciencecenter.ncradar;


import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.PrintWriter;
import java.io.UnsupportedEncodingException;

import ucar.ma2.Array;
import ucar.ma2.Index;
import ucar.nc2.Attribute;
import ucar.nc2.NetcdfFile;
import ucar.nc2.Variable;

public class RadarScan {

	private int datasetIndex;
	private String directory;
	private String filename;
	private String scanType;
	private double[][][][] polygons;
	private long numberOfRays;
	private long numberOfBins;
	private double elevationAngle;
//	private double gain;
//	private double offset;
	private byte[][] scanData;
	private String startDate;
	private String startTime;
	private String endDate;
	private String endTime;
	private double radarPositionHeight;
	private double radarPositionLatitude;
	private double radarPositionLongitude;
	private String datasetName;
	
//    static {
//    	System.out.println("there's an error here relating to the library path");
//    	System.loadLibrary("sayhello"); 
//    	}
//		 
//    // Declare native method
//    private native void sayHello();	
	
	public RadarScan(String directory,String filename,int datasetIndex) throws IOException {
		
 
		this.datasetIndex = datasetIndex;
		this.datasetName = "dataset"+(datasetIndex+1);
		this.directory = directory;
		this.filename = filename;
		this.scanType = readScanType();
		this.numberOfRays = readNumberOfRays();
        this.numberOfBins = readNumberOfBins();
		this.elevationAngle = readElevationAngle();
		this.startDate = readStartDate();
        this.startTime = readStartTime();
		this.endDate = readEndDate();
        this.endTime = readEndTime();
		this.radarPositionHeight = readRadarPositionHeight();
		this.radarPositionLatitude = readRadarPositionLatitude();
		this.radarPositionLongitude = readRadarPositionLongitude();
		this.scanData = readScanData();
		
	}
	

	private String readScanType() throws IOException {
		
		String scanType = "";
		String fullFilename = this.directory + File.separator + this.filename;

		NetcdfFile ncfile = null;
		try {
			ncfile = NetcdfFile.open(fullFilename);

			Attribute attr = ncfile.findGlobalAttribute(datasetName+"_data1_what_quantity");
			if (attr==null){
				System.err.println("Looks like there is no attribute by that name.");
			}
		    scanType = attr.getStringValue();
		    
		} finally {
			ncfile.close();
		}
		
		return scanType;
	}

	
	private String readStartDate() throws IOException {
		
		String startDate = "";
		String fullFilename = this.directory + File.separator + this.filename;

		NetcdfFile ncfile = null;
		try {

			ncfile = NetcdfFile.open(fullFilename);
	        Attribute attr = ncfile.findGlobalAttribute(datasetName + "_what_startdate");
			if (attr==null){
				System.err.println("Looks like there is no attribute by that name.");
			}
	        startDate = attr.getStringValue();

	        
		} finally {
			ncfile.close();
		}
		
		return startDate;
	}
	
	private String readStartTime() throws IOException {
		
		String startTime = "";
		String fullFilename = this.directory + File.separator + this.filename;

		NetcdfFile ncfile = null;
		try {

			ncfile = NetcdfFile.open(fullFilename);
	        Attribute attr = ncfile.findGlobalAttribute(datasetName + "_what_starttime");
			if (attr==null){
				System.err.println("Looks like there is no attribute by that name.");
			}
	        startTime = attr.getStringValue();

	        
		} finally {
			ncfile.close();
		}
		
		return startTime;
	}
		
	
	private String readEndDate() throws IOException {
		
		String endDate = "";
		String fullFilename = this.directory + File.separator + this.filename;

		NetcdfFile ncfile = null;
		try {

			ncfile = NetcdfFile.open(fullFilename);
	        Attribute attr = ncfile.findGlobalAttribute(datasetName + "_what_enddate");
			if (attr==null){
				System.err.println("Looks like there is no attribute by that name.");
			}
	        
	        endDate = attr.getStringValue();

	        
		} finally {
			ncfile.close();
		}
		
		return endDate;
	}
	
	private String readEndTime() throws IOException {
		
		String endTime = "";
		String fullFilename = this.directory + File.separator + this.filename;

		NetcdfFile ncfile = null;
		try {

			ncfile = NetcdfFile.open(fullFilename);
	        Attribute attr = ncfile.findGlobalAttribute(datasetName + "_what_endtime");
			if (attr==null){
				System.err.println("Looks like there is no attribute by that name.");
			}
	        endTime = attr.getStringValue();

	        
		} finally {
			ncfile.close();
		}
		
		return endTime;
	}
			
	
	
	private byte[][] readScanData() throws IOException {
		
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
	        
			byte[][] scanData = new byte[nRows][nCols];

	        for (int iRow=0;iRow<nRows;iRow++){
	        	for (int iCol=0;iCol<nCols;iCol++){
	        		scanData[iRow][iCol] = data.getByte(index.set(iRow,iCol));
	        	}
	        }
	        
	        return scanData;
	        
	        
		} finally {
			ncfile.close();
		}

	}


	private long readNumberOfRays() throws IOException {
		
		long nRays;
		
		String fullFilename = this.directory + File.separator + this.filename;

		NetcdfFile ncfile = null;
		try {
			ncfile = NetcdfFile.open(fullFilename);
	        Attribute attr = ncfile.findGlobalAttribute(datasetName + "_where_nrays");
			if (attr==null){
				System.err.println("Looks like there is no attribute by that name.");
			}
	        nRays = (Long) attr.getNumericValue();
		} finally {
			ncfile.close();
		}
		
		return nRays;
	}

	
	private long readNumberOfBins() throws IOException {
		
		long nBins;
		
		String fullFilename = this.directory + File.separator + this.filename;

		NetcdfFile ncfile = null;
		try {
			ncfile = NetcdfFile.open(fullFilename);
	        Attribute attr = ncfile.findGlobalAttribute(datasetName + "_where_nbins");
			if (attr==null){
				System.err.println("Looks like there is no attribute by that name.");
			}
	        nBins = (Long) attr.getNumericValue();
		} finally {
			ncfile.close();
		}
		return nBins;
	}

	private double readElevationAngle() throws IOException {
		
		double elevAngle;
		
		String fullFilename = this.directory + File.separator + this.filename;

		NetcdfFile ncfile = null;
		try {
			ncfile = NetcdfFile.open(fullFilename);
	        Attribute attr = ncfile.findGlobalAttribute(datasetName + "_where_elangle");
			if (attr==null){
				System.err.println("Looks like there is no attribute by that name.");
			}
	        elevAngle = (Double) attr.getNumericValue();
		} finally {
			ncfile.close();
		}
		return elevAngle;
	}

	private double readRadarPositionHeight() throws IOException {
		
		double radarPositionHeight;
		
		String fullFilename = this.directory + File.separator + this.filename;

		NetcdfFile ncfile = null;
		try {
			ncfile = NetcdfFile.open(fullFilename);
	        Attribute attr = ncfile.findGlobalAttribute("where_height");
			if (attr==null){
				System.err.println("Looks like there is no attribute by that name.");
			}
	        radarPositionHeight = (Double) attr.getNumericValue();
		} finally {
			ncfile.close();
		}
		return radarPositionHeight;
	}
	
	private double readRadarPositionLatitude() throws IOException {
		
		double radarPositionLatitude;
		
		String fullFilename = this.directory + File.separator + this.filename;

		NetcdfFile ncfile = null;
		try {
			ncfile = NetcdfFile.open(fullFilename);
	        Attribute attr = ncfile.findGlobalAttribute("where_lat");
			if (attr==null){
				System.err.println("Looks like there is no attribute by that name.");
			}
	        radarPositionLatitude = (Double) attr.getNumericValue();
		} finally {
			ncfile.close();
		}
		return radarPositionLatitude;
	}
	
	private double readRadarPositionLongitude() throws IOException {
		
		double radarPositionLongitude;
		
		String fullFilename = this.directory + File.separator + this.filename;

		NetcdfFile ncfile = null;
		try {
			ncfile = NetcdfFile.open(fullFilename);
	        Attribute attr = ncfile.findGlobalAttribute("where_lon");
			if (attr==null){
				System.err.println("Looks like there is no attribute by that name.");
			}
	        radarPositionLongitude = (Double) attr.getNumericValue();
		} finally {
			ncfile.close();
		}
		return radarPositionLongitude;
	}
	
	
	
	public double[][][][] calcPolygons(){
		
		double angleTrailing = 0;
		double angleLeading = 0;
		double range = 0.50;
		double[][][][] polygons = new double[(int)numberOfBins][(int)numberOfRays][5][2];
				
		for (int iBin=0;iBin<numberOfBins;iBin++){
			
			double lengthInner = ((double) iBin/numberOfBins) * range;
			double lengthOuter = ((double) (iBin+1)/numberOfBins) * range;

			for (int iRay=0;iRay<numberOfRays;iRay++){
				
				angleTrailing = ((double) iRay/numberOfRays)*2*Math.PI;
				angleLeading = ((double) (iRay+1)/numberOfRays)*2*Math.PI;
				
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
	
	public void printAsWKTToCSV() throws FileNotFoundException, UnsupportedEncodingException {
		
		String filename = this.filename+".wkt.csv";
		printAsWKTToCSV(filename);
		
	}


	
	public void printAsWKTToCSV(String filename) throws FileNotFoundException, UnsupportedEncodingException {

		
		PrintWriter writer = new PrintWriter(filename, "UTF-8");

		int nPolyCorners = 5;
		int iBinMin = 0;
		int iBinMax = 500;
		int iRayMin = 225;
		int iRayMax = 315;
				
		String headerStr = "\"the_geom\",\"reflectivity\"";
		writer.println(headerStr);
		
		
		for (int iBin=0;iBin<numberOfBins;iBin++){
			
			for (int iRay=0;iRay<numberOfRays;iRay++){

				if (iBinMin<iBin & iBin<iBinMax & iRayMin<iRay & iRay<iRayMax){
				String polyStr = "\"POLYGON((";
				
				for (int iPolyCorner=0;iPolyCorner<nPolyCorners;iPolyCorner++){
					polyStr = polyStr + String.format("%.6f %.6f", polygons[iBin][iRay][iPolyCorner][0],polygons[iBin][iRay][iPolyCorner][1]);
					if (iPolyCorner<nPolyCorners-1){
						
						polyStr = polyStr + ",";						
						
					}
				}
				polyStr = polyStr + "))\"";

				polyStr = polyStr + "," + scanData[iRay][iBin];
				
				writer.println(polyStr);
				
				}

			}
		}

		writer.close();

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
		
		for (int iBin=0;iBin<numberOfBins;iBin++){
			
			for (int iRay=0;iRay<numberOfRays;iRay++){

				String polyStr = "\"{\"\"type\"\":\"\"MultiPolygon\"\",\"\"coordinates\"\":[[[";
				
				
				for (int iPolyCorner=0;iPolyCorner<nPolyCorners;iPolyCorner++){
					polyStr = polyStr + String.format("[%.6f,%.6f]", polygons[iBin][iRay][iPolyCorner][0],polygons[iBin][iRay][iPolyCorner][1]);
					if (iPolyCorner<nPolyCorners-1){
						
						polyStr = polyStr + ",";						
						
					}
				}

				polyStr = polyStr + "]]]}\"";

				polyStr = polyStr + "," + scanData[iRay][iBin];
				
				writer.println(polyStr);

			}
		}

		writer.close();

	}
	
	
	// getters below this point
	 
	public String getDirectory() {
		return directory;
	}

	public String getFilename() {
		return filename;
	}
	
	public long getNumberOfRays() {
		return numberOfRays;
	}

	public long getNumberOfBins() {
		return numberOfBins;
	}

	public double getElevationAngle() {
		return elevationAngle;
	}

	public byte[][] getScanData() {
		return scanData.clone();
	}


	public double[][][][] getPolygons() {
		return polygons.clone();
	}


	public int getDatasetIndex() {
		return datasetIndex;
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

	public String getEndDate() {
		return endDate;
	}

	public String getEndTime() {
		return endTime;
	}

	public double getRadarPositionLatitude() {
		return radarPositionLatitude;
	}

	public double getRadarPositionLongitude() {
		return radarPositionLongitude;
	}

	public String getDatasetName() {
		return datasetName;
	}

	
	public double getRadarPositionHeight() {
		return radarPositionHeight;
	}
	
	
	
	public static void main(String[] args) throws IOException{
		
		RadarScan rs = new RadarScan("/home/wbouten/tmp","T_PAGZ60_C_OKPR_20110815000447.hdf",10);
		String str = rs.getDirectory();
		System.out.println(str);

//		rs.printAsWKTToCSV();
//		rs.printAsGeoJSONToCSV();
	}

}



