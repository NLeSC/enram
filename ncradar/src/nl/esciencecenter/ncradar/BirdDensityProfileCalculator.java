package nl.esciencecenter.ncradar;

import java.io.IOException;

public class BirdDensityProfileCalculator extends JNIMethodsVol2Bird {
	
	
	public BirdDensityProfileCalculator() throws IOException {
		
	}
	
    public static void main(String[] args) throws IOException{
        
        RadarScan rsz = new RadarScan("/home/wbouten/tmp","T_PAGZ60_C_OKPR_20110815000447.hdf",3);
        RadarScan rsv = new RadarScan("/home/wbouten/tmp","T_PAGZ60_C_OKPR_20110815000447.hdf",3);


        int tMissing = 23;
        int tnAzim = 5;
        int tnRange = 6;
        double tOffset = 0.00;
        double tScale = 0.001;
        int[] vImage = new int[] {60,61,62,63,64,65,66,67,68,69,
                                  70,71,72,73,74, 5,76,77,78,79,
                                  80,81,82,83,84,85,86,87,88,89};
        int vMissing = 45;
        int vnAzim = tnAzim;
        int vnRange = tnRange;
        double vOffset = 4.23;
        double vScale = 0.001;
        int[] zImage = new int[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
        	                      10, 1,12,13,14,15,16,17,18,19,
                                  20,21,22,23,24,25,26,27,28,29};
        int zMissing = 68;
        int znAzim = tnAzim;
        int znRange = tnRange;
        double zOffset = 56.9;
        double zScale = 100;
        int nRangLocal = 3;
        int nAzimLocal = 3;
        int nCountMin = 4;
        int textype = 2; // (TEXSTDEV)

        int[] summedByC = calcTexture(tMissing,tnAzim,tnRange,tOffset,tScale,
        		                  vImage,vMissing,vnAzim,vnRange,vOffset,vScale,
        		                  zImage,zMissing,znAzim,znRange,zOffset,zScale,
        		                  nRangLocal,nAzimLocal,nCountMin,textype);

        int nElems = summedByC.length;
        
        for (int iElem=-0;iElem<nElems;iElem++){
        	System.out.println("(Java) iElem "+iElem+" = "+summedByC[iElem]);
        }

//        rs.calcPolygons();
//        rs.printAsWKTToCSV();
//        rs.printAsGeoJSONToCSV();
    }



}
