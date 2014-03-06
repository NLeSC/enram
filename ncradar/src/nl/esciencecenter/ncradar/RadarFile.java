
package nl.esciencecenter.ncradar;


import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.UnsupportedEncodingException;

import ucar.nc2.Attribute;
import ucar.nc2.NetcdfFile;
import ucar.nc2.Variable;
import ucar.nc2.dataset.NetcdfDataset;

public class RadarFile {
	
	private String directory;
	private String filename;
	private double[][][][] polygons;
	private long nRays;
	private long nBins;
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

		this.nRays = readNumberOfRays();
		System.out.println("nRays = " + this.nRays);
		
//        this.nBins = readNumberOfBins();
//        System.out.println("nBins = " + this.nBins);
//        
//		this.elevationAngle = readElevationAngle();
//		System.out.println("elevationAngle = " + this.elevationAngle);
//		
//		this.radarPositionHeight = readRadarPositionHeight();
//		this.radarPositionLatitude = readRadarPositionLatitude();
//		this.radarPositionLongitude = readRadarPositionLongitude();
//		
//		
//		this.scanData = readScanData();
//		
//		this.calcPolygons();
//		
//		System.out.println("construction finished -- all good so far");
		
		
	}
	
	
	private long readNumberOfRays() throws IOException{
		
		String fullFilename = this.directory + File.separator + this.filename;

		NetcdfFile ncfile = null;
	
		ncfile = NetcdfDataset.openFile(fullFilename, null) ;
			
        Attribute attr = ncfile.findAttribute("dataset1_where_nrays");

		return nRays;
		
	}

	
	
	private void calcPolygons(){
		
		double angleTrailing = 0;
		double angleLeading = 0;
		double range = 0.50;
		double[][][][] polygons = new double[(int)nBins][(int)nRays][5][2];
				
		for (int iBin=0;iBin<nBins;iBin++){
			
			double lengthInner = ((double) iBin/nBins) * range;
			double lengthOuter = ((double) (iBin+1)/nBins) * range;

			for (int iRay=0;iRay<nRays;iRay++){
				
				angleTrailing = ((double) iRay/nRays)*2*Math.PI;
				angleLeading = ((double) (iRay+1)/nRays)*2*Math.PI;
				
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
		
		this.polygons = polygons;
		
	}
	
//	private void printAsWKTToCSV() throws FileNotFoundException, UnsupportedEncodingException {
//
//		
//		PrintWriter writer = new PrintWriter("polygon-test.wkt.csv", "UTF-8");
//
//		int nPolyCorners = 5;
//		int iBinMin = 0;
//		int iBinMax = 500;
//		int iRayMin = 225;
//		int iRayMax = 315;
//				
//		String headerStr = "\"the_geom\",\"reflectivity\"";
//		writer.println(headerStr);
//		
//		
//		for (int iBin=0;iBin<nBins;iBin++){
//			
//			for (int iRay=0;iRay<nRays;iRay++){
//
//				if (iBinMin<iBin & iBin<iBinMax & iRayMin<iRay & iRay<iRayMax){
//				String polyStr = "\"POLYGON((";
//				
//				for (int iPolyCorner=0;iPolyCorner<nPolyCorners;iPolyCorner++){
//					polyStr = polyStr + String.format("%.6f %.6f", polygons[iBin][iRay][iPolyCorner][0],polygons[iBin][iRay][iPolyCorner][1]);
//					if (iPolyCorner<nPolyCorners-1){
//						
//						polyStr = polyStr + ",";						
//						
//					}
//				}
//				polyStr = polyStr + "))\"";
//
//				polyStr = polyStr + "," + scanData[iRay][iBin];
//				
//				writer.println(polyStr);
//				
//				}
//
//			}
//		}
//
//		writer.close();
//
//	}
//
//	private void printAsGeoJSONToCSV() throws FileNotFoundException, UnsupportedEncodingException {
//
//		
//		PrintWriter writer = new PrintWriter("polygon-test.geojson.csv", "UTF-8");
//
//		int nPolyCorners = 5;
//		String headerStr = "\"the_geom\",\"reflectivity\"";
//		writer.println(headerStr);
//		
//		for (int iBin=0;iBin<nBins;iBin++){
//			
//			for (int iRay=0;iRay<nRays;iRay++){
//
//				String polyStr = "\"{\"\"type\"\":\"\"MultiPolygon\"\",\"\"coordinates\"\":[[[";
//				
//				
//				for (int iPolyCorner=0;iPolyCorner<nPolyCorners;iPolyCorner++){
//					polyStr = polyStr + String.format("[%.6f,%.6f]", polygons[iBin][iRay][iPolyCorner][0],polygons[iBin][iRay][iPolyCorner][1]);
//					if (iPolyCorner<nPolyCorners-1){
//						
//						polyStr = polyStr + ",";						
//						
//					}
//				}
//
//				polyStr = polyStr + "]]]}\"";
//
//				polyStr = polyStr + "," + scanData[iRay][iBin];
//				
//				writer.println(polyStr);
//
//			}
//		}
//
//		writer.close();
//
//	}
	
	
	// getters below this point
	 
	public String getDirectory() {
		return directory;
	}

	public String getFilename() {
		return filename;
	}
	
	public long getnRays() {
		return nRays;
	}

	public long getnBins() {
		return nBins;
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



