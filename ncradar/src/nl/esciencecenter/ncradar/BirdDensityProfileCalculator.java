
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

        
        int[] cellPropIRangOfMax;
        int[] cellPropIAzimOfMax; 
        float[] cellPropDbzAvg; 
        float[] cellPropTexAvg; 
        float[] cellPropCv; 
        float[] cellPropArea; 
        float[] cellPropClutterArea; 
        float[] cellPropMax; 
        int[] cellPropIndex; 
        char[] cellPropDrop; 
        int nCells; 
        int method;

        cellPropIRangOfMax = new int[] {0,3,6,4};
        cellPropIAzimOfMax = new int[] {2,5,2,4};
        cellPropDbzAvg = new float[] {2,11,25,6};
        cellPropTexAvg = new float[] {1,2,4,1};
        cellPropCv = new float[] {2,3,1,5};
        cellPropArea = new float[] {5,3,7,2};
        cellPropClutterArea = new float[] {0,0,0,0};
        cellPropMax = new float[] {14,4,28,9};
        cellPropIndex = new int[] {0,1,2,3};
        cellPropDrop = new char[] {0,0,0,0};
        nCells = 4; 
        method = 2;
        
        sortCells(cellPropIRangOfMax, cellPropIAzimOfMax, cellPropDbzAvg, cellPropTexAvg, 
                  cellPropCv, cellPropArea, cellPropClutterArea, cellPropMax, 
                  cellPropIndex, cellPropDrop, nCells,method);
        
        int nElems = nCells;
        for (int iElem = 0;iElem<nElems;iElem++) {
            System.err.println("(Java) iElem "+iElem+": cellPropArea[iElem] = "+cellPropArea[iElem]+
                                                     "; cellPropDbzAvg[iElem] = "+cellPropDbzAvg[iElem]);
        }

        
        
        float dist;
        dist = dist(12,14,67,24,4.5f,2.2f);
        System.err.println("(Java) dist = "+ dist);
        
        
//        int[] cellImage = new int[] {2,3,0,1,2,3,4,5};
//        int nRang = 4;
//        int nAzim = 2;
//        float aScale = 0.4f;
//        float rScale = 0.9f;
//        float fringe = 2;
//        fringecells(cellImage, nRang, nAzim, aScale, rScale, fringe);
        

//        RadarScan rsz = new RadarScan("/home/wbouten/tmp","T_PAGZ60_C_OKPR_20110815000447.hdf",3);
//        RadarScan rsv = new RadarScan("/home/wbouten/tmp","T_PAHZ60_C_OKPR_20110815000447.hdf",3);
//
//        BirdDensityProfileCalculator birdprofile = new BirdDensityProfileCalculator(rsz,rsv);
//
//
//        int[] vImage = rsv.convertScanData();
//        int vMissing = rsv.getMissingValue();
//        int vnAzim = (int) rsv.getNumberOfAzimuthBins();
//        int vnRange = (int) rsv.getNumberOfRangeBins();
//        double vOffset = rsv.getOffset();
//        double vScale = rsv.getGain();
//
//        int[] zImage = rsz.convertScanData();
//        int zMissing = rsz.getMissingValue();
//        int znAzim = (int) rsz.getNumberOfAzimuthBins();
//        int znRange = (int) rsz.getNumberOfRangeBins();
//        double zOffset = rsz.getOffset();
//        double zScale = rsz.getGain();
//
//        int nRangLocal = 3;
//        int nAzimLocal = 3;
//        int nCountMin = 4;
//        int textype = 2; // (TEXSTDEV)
//
//        int tMissing = 255;
//        int tnAzim = vnAzim;
//        int tnRange = vnRange;
//        double tOffset = 0.00;
//        double tScale = 1;
//
//
//        int[] texture = calcTexture(tMissing,tnAzim,tnRange,tOffset,tScale,
//                               vImage,vMissing,vnAzim,vnRange,vOffset,vScale,
//                               zImage,zMissing,znAzim,znRange,zOffset,zScale,
//                               nRangLocal,nAzimLocal,nCountMin,textype);
//
//        int nElems = texture.length;
//
//        for (int iElem=0;iElem<nElems;iElem++){
//            System.err.println("(Java) iElem "+iElem+" = "+texture[iElem]);
//        }
//
    }
    



}
