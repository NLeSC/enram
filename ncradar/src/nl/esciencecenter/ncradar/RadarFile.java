
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

public class RadarFile {
	
	private String directory;
	private String filename;
	private double[][][][] polygons;
	private long numberOfRays;
	private long numberOfBins;
	private double elevationAngle;
	private byte[][] scanData;
	private double radarPositionHeight;
	private double radarPositionLatitude;
	private double radarPositionLongitude;
	
	public RadarFile(String directory,String filename) throws IOException {
		
		this.directory = directory;
		System.out.println("directory = " + this.directory);
		
		this.filename = filename;
		System.out.println("filename = " + this.filename);

		this.numberOfRays = readNumberOfRays();
		System.out.println("nRays = " + this.numberOfRays);
		
        this.numberOfBins = readNumberOfBins();
        System.out.println("nBins = " + this.numberOfBins);
        
		this.elevationAngle = readElevationAngle();
		System.out.println("elevationAngle = " + this.elevationAngle);
		
		this.radarPositionHeight = readRadarPositionHeight();
		this.radarPositionLatitude = readRadarPositionLatitude();
		this.radarPositionLongitude = readRadarPositionLongitude();
		
		
		this.scanData = readScanData();
		
		this.polygons = calcPolygons();
		
		System.out.println("construction finished -- all good so far");
		
		
	}
	
	
	private byte[][] readScanData() throws IOException {
		
		String fullFilename = this.directory + File.separator + this.filename;

		NetcdfFile ncfile = null;
		try {
			ncfile = NetcdfFile.open(fullFilename);
	        Variable var = ncfile.findVariable("dataset1/data1/data");
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
	        Attribute attr = ncfile.findGlobalAttribute("dataset1_where_nrays");
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
	        Attribute attr = ncfile.findGlobalAttribute("dataset1_where_nbins");
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
	        Attribute attr = ncfile.findGlobalAttribute("dataset1_where_elangle");
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
	        radarPositionLongitude = (Double) attr.getNumericValue();
		} finally {
			ncfile.close();
		}
		return radarPositionLongitude;
	}
	
	
	
	private double[][][][] calcPolygons(){
		
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

		
		PrintWriter writer = new PrintWriter("polygon-test.wkt.csv", "UTF-8");

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

		
		PrintWriter writer = new PrintWriter("polygon-test.geojson.csv", "UTF-8");

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
		return scanData;
	}
	
	public static void main(String[] args) throws IOException{
		
		RadarFile rf = new RadarFile("/home/daisycutter/tmp","T_PAGZ60_C_OKPR_20110815000447.hdf");
		String str = rf.getDirectory();
		System.out.println(str);

//		rf.printAsWKTToCSV();
//		rf.printAsGeoJSONToCSV();
	}
	
}



