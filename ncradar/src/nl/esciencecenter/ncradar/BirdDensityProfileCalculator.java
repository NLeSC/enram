
package nl.esciencecenter.ncradar;

import java.io.IOException;

public class BirdDensityProfileCalculator extends JNIMethodsVol2Bird {

    private RadarScan rsz;
    private RadarScan rsv;

    public BirdDensityProfileCalculator(RadarScan rsz, RadarScan rsv) throws IOException {

        this.rsz = rsz;
        this.rsv = rsv;

    }
    
    public static void main(String[] args) throws IOException{


        RadarScan rsz = new RadarScan("/home/wbouten/tmp","T_PAGZ60_C_OKPR_20110815000447.hdf",3);
        RadarScan rsv = new RadarScan("/home/wbouten/tmp","T_PAHZ60_C_OKPR_20110815000447.hdf",3);

        BirdDensityProfileCalculator birdprofile = new BirdDensityProfileCalculator(rsz,rsv);


        int tMissing = 23;
        int tnAzim = 5;
        int tnRange = 6;
        double tOffset = 0.00;
        double tScale = 0.001;
        
        int[] vImage = new int[] {60,61,62,63,64,65,66,67,68,69,
                                  70,71,72,73,74, 5,76,77,78,79,
                                  80,81,82,83,84,85,86,87,88,89};
        int vMissing = rsv.getMissingValue();
        int vnAzim = (int) rsv.getNumberOfAzimuthBins();
        int vnRange = (int) rsv.getNumberOfRangeBins();
        double vOffset = rsv.getOffset();
        double vScale = rsv.getGain();

        int[] zImage = new int[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
                                  10, 1,12,13,14,15,16,17,18,19,
                                  20,21,22,23,24,25,26,27,28,29};
        int zMissing = rsz.getMissingValue();
        int znAzim = (int) rsz.getNumberOfAzimuthBins();
        int znRange = (int) rsz.getNumberOfRangeBins();
        double zOffset = rsz.getOffset();
        double zScale = rsz.getGain();

        int nRangLocal = 3;
        int nAzimLocal = 3;
        int nCountMin = 4;
        int textype = 2; // (TEXSTDEV)

        int[] texture = calcTexture(tMissing,tnAzim,tnRange,tOffset,tScale,
                               vImage,vMissing,vnAzim,vnRange,vOffset,vScale,
                               zImage,zMissing,znAzim,znRange,zOffset,zScale,
                               nRangLocal,nAzimLocal,nCountMin,textype);

        int nElems = texture.length;

        for (int iElem=-0;iElem<nElems;iElem++){
            System.out.println("(Java) iElem "+iElem+" = "+texture[iElem]);
        }

    }



}
