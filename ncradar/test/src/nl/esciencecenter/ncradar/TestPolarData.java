package nl.esciencecenter.ncradar;

import static org.junit.Assert.*;
import org.junit.Test;





public class TestPolarData {
    
    
    int numberOfAzimuthBins = 4;
    int numberOfRangeBins = 2;
    int[] dataRaw = new int[]{0,1,2,3,4,5,6,7};
    double dataOffset = 10.0;
    double dataScale = 0.1;
    double rangeOffset = 5.0;
    double rangeScale = 3.0;
    int missingValueValue = 255;
    int iAzimFirstRay = 0;
    
    PolarData polarData = new PolarData(numberOfAzimuthBins, numberOfRangeBins, 
            dataRaw, dataOffset, dataScale, 
            rangeOffset, rangeScale, 
            missingValueValue, iAzimFirstRay);
        


    @Test
    public void testPolarDataAzimuthScaleDeg1() {

        double a = polarData.getAzimuthScaleDeg();
        double e = 90;

        double delta = 0.0001;
        assertEquals(e, a, delta );        
        
        
    }

    
    
    
    @Test
    public void testPolarDataAzimuthScaleRad1() {

        double a = polarData.getAzimuthScaleRad();
        double e = (90.0/360)*2*Math.PI;

        double delta = 0.0001;
        assertEquals(e, a, delta );
        
        
    }

    
    
    
    
    @Test
    public void testPolarDataGetData1() {

        double[] a = polarData.getData();
        double[] e = new double[]{0.0,1.0,2.0,3.0,4.0,5.0,6.0,7.0};
        
        int nAzim = polarData.getNumberOfAzimuthBins();
        int nRang = polarData.getNumberOfRangeBins();

        double delta  = 0.001;
        int iGlobal = 0;
        for (int iAzim = 0; iAzim < nAzim; iAzim++){
            for (int iRang = 0; iRang < nRang; iRang++){
                assertEquals(e[iGlobal], (a[iGlobal]-dataOffset)/dataScale,delta);
                iGlobal = iGlobal + 1;
            }
        }
        
    }
    
    
    
    @Test
    public void testPolarDataGetData2D1() {

        double[][] a = polarData.getData2D();
        double[][] e = new double[][]{{0.0,1.0},{2.0,3.0},{4.0,5.0},{6.0,7.0}};
        
        int nAzim = polarData.getNumberOfAzimuthBins();
        int nRang = polarData.getNumberOfRangeBins();

        double delta  = 0.001;
        for (int iAzim = 0; iAzim < nAzim; iAzim++){
            for (int iRang = 0; iRang < nRang; iRang++){
                assertEquals(e[iAzim][iRang], (a[iAzim][iRang]-dataOffset)/dataScale,delta);
            }
        }
        
    }

    
    
    @Test
    public void testPolarDataGetDataRaw1() {

        int[] a = polarData.getDataRaw();
        int[] e = new int[]{0,1,2,3,4,5,6,7};
        
        int nData = a.length;

        for (int iData = 0; iData < nData; iData++){
            assertEquals(e[iData], a[iData]);
        }
        
    }

        
    
    @Test
    public void testPolarDataGetDataRaw2D1() {

        int[][] a = polarData.getDataRaw2D();
        int[][] e = new int[][]{{0,1},{2,3},{4,5},{6,7}};
        
        int nAzim = polarData.getNumberOfAzimuthBins();
        int nRang = polarData.getNumberOfRangeBins();
        
        for (int iAzim = 0; iAzim < nAzim; iAzim++){
            for (int iRang = 0; iRang < nRang; iRang++){
                assertEquals(e[iAzim][iRang], a[iAzim][iRang]);
            }
        }
        
    }

        
    

        
    
    @Test
    public void testPolarDataNumberOfAzimuthBins1() {

        int a = polarData.getNumberOfAzimuthBins();
        int e = 4;

        assertEquals(e, a);
    }

        
    @Test
    public void testPolarDataNumberOfRangeBins1() {

        int a = polarData.getNumberOfRangeBins();
        int e = 2;

        assertEquals(e, a);
    }

        
    
    
}
