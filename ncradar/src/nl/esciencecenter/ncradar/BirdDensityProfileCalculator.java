
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


        int[] vImage = rsv.convertScanData();
        int vMissing = rsv.getMissingValue();
        int vnAzim = (int) rsv.getNumberOfAzimuthBins();
        int vnRange = (int) rsv.getNumberOfRangeBins();
        double vOffset = rsv.getOffset();
        double vScale = rsv.getGain();

        int[] zImage = rsz.convertScanData();
        int zMissing = rsz.getMissingValue();
        int znAzim = (int) rsz.getNumberOfAzimuthBins();
        int znRange = (int) rsz.getNumberOfRangeBins();
        double zOffset = rsz.getOffset();
        double zScale = rsz.getGain();

        int nRangLocal = 3;
        int nAzimLocal = 3;
        int nCountMin = 4;
        int textype = 2; // (TEXSTDEV)

        int tMissing = 255;
        int tnAzim = vnAzim;
        int tnRange = vnRange;
        double tOffset = 0.00;
        double tScale = 1;


        int[] texture = calcTexture(tMissing,tnAzim,tnRange,tOffset,tScale,
                               vImage,vMissing,vnAzim,vnRange,vOffset,vScale,
                               zImage,zMissing,znAzim,znRange,zOffset,zScale,
                               nRangLocal,nAzimLocal,nCountMin,textype);

        int nElems = texture.length;

        for (int iElem=0;iElem<nElems;iElem++){
            System.err.println("(Java) iElem "+iElem+" = "+texture[iElem]);
        }

    }
    



}
