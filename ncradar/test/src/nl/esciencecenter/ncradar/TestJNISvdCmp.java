package nl.esciencecenter.ncradar;

import static org.junit.Assert.*;
import org.junit.Test;





public class TestJNISvdCmp extends JNIMethodsVol2Bird {

    double delta = 0.001;


    @Test
    public void testNativeSvdCmp1() {
        
        // numbers are based on http://people.sc.fsu.edu/~jburkardt/m_src/svd_demo/svd_demo_5_3_output.txt (which uses
        // MATLAB's [U,W,V] = svd(A) function)
        
        // note that the unit test below only tests for the absolute value, not the sign.


        int nRows = 5;
        int nCols = 3;

        float[] originalArrayA = {  2.0f,  1.0f,  1.0f,
                                   10.0f,  3.0f,  4.0f,
                                    8.0f,  1.0f,  4.0f,
                                    6.0f,  0.0f,  8.0f,
                                    4.0f,  6.0f,  8.0f};

        float[] actualArrayA = originalArrayA.clone();

        float[] actualArrayW = new float[nCols];

        float[] actualArrayV = new float[nCols*nCols];


        
        int status = svdcmp(actualArrayA,nRows,nCols,actualArrayW,actualArrayV);

        
        float[] actualArrayU = actualArrayA.clone();

        
        float[] expectedArrayU = { -0.122437f,    0.0452879f,    0.140845f,
                                   -0.552266f,    0.468282f,     0.415306f,
                                   -0.447998f,    0.400116f,    -0.0569765f,
                                   -0.485998f,   -0.124504f,    -0.821077f,
                                   -0.493068f,   -0.776574f,     0.36093f};

        float[] expectedArrayW = { 19.3031f,  6.20391f, 4.11136f};
        
        float[] expectedArrayV = { -0.737695f,  0.664259f,  0.12069f,
                                   -0.268643f, -0.452811f,  0.850172f,
                                   -0.619384f, -0.594746f, -0.512485f};
        
        
        for (int iRow = 0; iRow < nRows; iRow++) {
            for (int iCol = 0; iCol < nCols; iCol++) {

                int iGlobal = iRow * nCols + iCol;
                float e = Math.abs(expectedArrayU[iGlobal]);
                float a = Math.abs(actualArrayU[iGlobal]);
                assertEquals(e, a, delta);

            }
        }
        
        
        
    }
    
    
    

    
    

    @Test
    public void testNativeSvdCmp2() {
        
        // numbers are based on http://people.sc.fsu.edu/~jburkardt/m_src/svd_demo/svd_demo_5_3_output.txt (which uses
        // MATLAB's [U,W,V] = svd(A) function)
        
        // note that the unit test below only tests for the absolute value, not the sign.


        int nRows = 5;
        int nCols = 5;

        float[] originalArrayA = {  1.0f,  2.0f,  8.0f,  8.0f,  0.0f,
                                    1.0f,  3.0f,  6.0f,  8.0f,  3.0f,
                                    1.0f,  6.0f,  3.0f,  7.0f,  3.0f,
                                    2.0f,  6.0f,  7.0f,  3.0f,  4.0f,
                                    6.0f,  4.0f,  6.0f,  8.0f,  7.0f };

        float[] actualArrayA = originalArrayA.clone();

        float[] actualArrayW = new float[nCols];

        float[] actualArrayV = new float[nCols*nCols];


        
        int status = svdcmp(actualArrayA,nRows,nCols,actualArrayW,actualArrayV);

        
        float[] actualArrayU = actualArrayA.clone();

        
        float[] expectedArrayU = {  -0.4289f,  -0.7456f,  -0.0136f,   0.2627f,  -0.4369f,
                                    -0.4396f,  -0.2795f,  -0.1375f,  -0.1349f,   0.8316f,
                                    -0.3922f,   0.0910f,   0.1597f,  -0.8538f,  -0.2888f,
                                    -0.4016f,   0.2935f,   0.7949f,   0.3397f,   0.0729f,
                                    -0.5547f,   0.5211f,  -0.5688f,   0.2614f,  -0.1698f };

        float[] expectedArrayW = {  24.3114f,   6.4187f,   4.5695f,   4.0760f,   1.1237f };
        
        float[] expectedArrayV = {  -0.2218f,   0.4330f,  -0.3971f,   0.3734f,  -0.6828f, 
                                    -0.3767f,   0.3212f,   0.6592f,  -0.4705f,  -0.3147f,
                                    -0.5506f,  -0.3409f,   0.3711f,   0.6570f,   0.1061f,
                                    -0.6308f,  -0.3918f,  -0.4941f,  -0.4522f,  -0.0035f,
                                    -0.3284f,   0.6631f,  -0.1611f,   0.0547f,   0.6508f };
        
        int[] columnOrder = {0,1,4,2,3};
        
        for (int iRow = 0; iRow < nRows; iRow++) {
            for (int iCol = 0; iCol < nCols; iCol++) {

                
                int iGlobalE = iRow * nCols + columnOrder[iCol];
                int iGlobalA = iRow * nCols + iCol;
                
                float e = Math.abs(expectedArrayU[iGlobalE]);
                float a = Math.abs(actualArrayU[iGlobalA]);
                
                assertEquals(e, a, delta);

            }
        }
    }
    
}
