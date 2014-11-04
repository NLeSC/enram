package nl.esciencecenter.ncradar;

import static org.junit.Assert.*;
import org.junit.Test;





public class TestJNISvdFit extends JNIMethodsVol2Bird {
    
    
    // parameters are not in the order x,y,z but rather z,x,y:
    int[] parameterOrder = new int[]{2,0,1};
    int nDims = 2;
    int nPoints = 9;
    int nParsFitted = 3;
    float [] vradFitted = new float[nPoints];
    float[] actualParameterVector = new float[nParsFitted];
    float[] avar = new float[nParsFitted*nParsFitted];
    float chisq;
    

    @Test
    public void testNativeSvdFitElev0PosX() {
 
        
        // this unit test's data says: orientation of the velocity field is
        // exactly in the direction of positive x:
        float[] expectedParameterVector = new float[]{1,0,0};
        
        // each row is an azimuthAngle, elevationAngle coordinate: 
        float[] points = new float[] {   0.00f,  0.00f,
                                        45.00f,  0.00f,
                                        90.00f,  0.00f,
                                       135.00f,  0.00f,
                                       180.00f,  0.00f,
                                       225.00f,  0.00f,
                                       270.00f,  0.00f,
                                       315.00f,  0.00f,
                                       360.00f,  0.00f};
        
        // the value of vrad when observing a vector [u0,v0,w0] = [1,0,0] at each point in the array 'points': 
        float[] vradObs = new float[] { 0.00f,
                                        0.71f,
                                        1.00f,
                                        0.71f,
                                        0.00f,
                                       -0.71f,
                                       -1.00f,
                                       -0.71f,
                                       -0.00f };
        
        chisq = svdfit(points, nDims, vradObs, vradFitted, nPoints, actualParameterVector, avar, nParsFitted);
        
        if (chisq == -1.0) {
            fail();
        }
        
        double vradDelta = 0.01;
        for (int iPoint=0; iPoint < nPoints; iPoint++) {
            float e = vradObs[iPoint];
            float a = vradFitted[iPoint];
            assertEquals(e, a, vradDelta );
        }

        double parameterVectorDelta = 0.01;
        for (int iParFitted = 0; iParFitted < nParsFitted; iParFitted++) {
            float e = expectedParameterVector[parameterOrder[iParFitted]];
            float a = actualParameterVector[iParFitted];
            assertEquals(e, a, parameterVectorDelta );
        }

    }

    
    
    
    
    @Test
    public void testNativeSvdFitElev0PosY() {
        
        // this unit test's data says: orientation of the velocity field is
        // exactly in the direction of positive y:
        float[] expectedParameterVector = new float[]{0,1,0};
        
        // each row is an azimuthAngle, elevationAngle coordinate: 
        float[] points = new float[] {   0.00f,  0.00f,
                                        45.00f,  0.00f,
                                        90.00f,  0.00f,
                                       135.00f,  0.00f,
                                       180.00f,  0.00f,
                                       225.00f,  0.00f,
                                       270.00f,  0.00f,
                                       315.00f,  0.00f,
                                       360.00f,  0.00f};
        
        // the value of vrad when observing a vector [u0,v0,w0] = [0,1,0] at each point in the array 'points': 
        float[] vradObs = new float[] { 1.00f,
                                        0.71f,
                                        0.00f,
                                       -0.71f,
                                       -1.00f,
                                       -0.71f,
                                       -0.00f,
                                        0.71f,
                                        1.00f };
        
        chisq = svdfit(points, nDims, vradObs, vradFitted, nPoints, actualParameterVector, avar, nParsFitted);
        
        
        if (chisq == -1.0) {
            fail();
        }
        
        double vradDelta = 0.01;
        for (int iPoint=0; iPoint < nPoints; iPoint++) {
            float e = vradObs[iPoint];
            float a = vradFitted[iPoint];
            assertEquals(e, a, vradDelta );
        }
        

        double parameterVectorDelta = 0.01;
        for (int iParFitted = 0; iParFitted < nParsFitted; iParFitted++) {
            float e = expectedParameterVector[parameterOrder[iParFitted]];
            float a = actualParameterVector[iParFitted];
            assertEquals(e, a, parameterVectorDelta );
        }

    }

    
    

    
    
    
    
    @Test
    public void testNativeSvdFitElev0PosZ() {
        
        // this unit test's data says: orientation of the velocity field is
        // exactly in the direction of positive z:
        float[] expectedParameterVector = new float[]{0,0,0};  // last number is a zero because at an
        // observation angle of 0, you have no information about the length of the vector in the z-direction
        
        // each row is an azimuthAngle, elevationAngle coordinate: 
        float[] points = new float[] {   0.00f,  0.00f,
                                        45.00f,  0.00f,
                                        90.00f,  0.00f,
                                       135.00f,  0.00f,
                                       180.00f,  0.00f,
                                       225.00f,  0.00f,
                                       270.00f,  0.00f,
                                       315.00f,  0.00f,
                                       360.00f,  0.00f};
        
        // the value of vrad when observing a vector [u0,v0,w0] = [0,0,1] at each point in the array 'points': 
        float[] vradObs = new float[] { 0.00f,
                                        0.00f,
                                        0.00f,
                                        0.00f,
                                        0.00f,
                                        0.00f,
                                        0.00f,
                                        0.00f,
                                        0.00f };

        chisq = svdfit(points, nDims, vradObs, vradFitted, nPoints, actualParameterVector, avar, nParsFitted);
        
        
        if (chisq == -1.0) {
            fail();
        }
        
        double vradDelta = 0.01;
        for (int iPoint=0; iPoint < nPoints; iPoint++) {
            float e = vradObs[iPoint];
            float a = vradFitted[iPoint];
            assertEquals(e, a, vradDelta );
        }
        

        double parameterVectorDelta = 0.01;
        for (int iParFitted = 0; iParFitted < nParsFitted; iParFitted++) {
            float e = expectedParameterVector[parameterOrder[iParFitted]];
            float a = actualParameterVector[iParFitted];
            assertEquals(e, a, parameterVectorDelta );
        }

    }

   
    
    

    
    @Test
    public void testNativeSvdFitElev30PosX() {
        
        // this unit test's data says: orientation of the velocity field is
        // exactly in the direction of positive x:
        float[] expectedParameterVector = new float[]{1,0,0};
        
        // each row is an azimuthAngle, elevationAngle coordinate: 
        float[] points = new float[] {   0.00f,  30.00f,
                                        45.00f,  30.00f,
                                        90.00f,  30.00f,
                                       135.00f,  30.00f,
                                       180.00f,  30.00f,
                                       225.00f,  30.00f,
                                       270.00f,  30.00f,
                                       315.00f,  30.00f,
                                       360.00f,  30.00f};
        
        // the value of vrad when observing a vector [u0,v0,w0] = [1,0,0] at each point in the array 'points': 
        float[] vradObs = new float[] { 0.00f,
                                        0.61f,
                                        0.87f,
                                        0.61f,
                                        0.00f,
                                       -0.61f,
                                       -0.87f,
                                       -0.61f,
                                       -0.00f };

        chisq = svdfit(points, nDims, vradObs, vradFitted, nPoints, actualParameterVector, avar, nParsFitted);
        
        
        if (chisq == -1.0) {
            fail();
        }
        
        double vradDelta = 0.01;
        for (int iPoint=0; iPoint < nPoints; iPoint++) {
            float e = vradObs[iPoint];
            float a = vradFitted[iPoint];
            assertEquals(e, a, vradDelta );
        }
        

        double parameterVectorDelta = 0.01;
        for (int iParFitted = 0; iParFitted < nParsFitted; iParFitted++) {
            float e = expectedParameterVector[parameterOrder[iParFitted]];
            float a = actualParameterVector[iParFitted];
            assertEquals(e, a, parameterVectorDelta );
        }

    }

    
        
    
    

    
    @Test
    public void testNativeSvdFitElev30PosY() {
        
        // this unit test's data says: orientation of the velocity field is
        // exactly in the direction of positive y:
        float[] expectedParameterVector = new float[]{0,1,0};
        
        // each row is an azimuthAngle, elevationAngle coordinate: 
        float[] points = new float[] {   0.00f,  30.00f,
                                        45.00f,  30.00f,
                                        90.00f,  30.00f,
                                       135.00f,  30.00f,
                                       180.00f,  30.00f,
                                       225.00f,  30.00f,
                                       270.00f,  30.00f,
                                       315.00f,  30.00f,
                                       360.00f,  30.00f};
        
        // the value of vrad when observing a vector [u0,v0,w0] = [0,1,0] at each point in the array 'points': 
        float[] vradObs = new float[] { 0.87f,
                                        0.61f,
                                        0.00f,
                                       -0.61f,
                                       -0.87f,
                                       -0.61f,
                                       -0.00f,
                                        0.61f,
                                        0.87f };

        chisq = svdfit(points, nDims, vradObs, vradFitted, nPoints, actualParameterVector, avar, nParsFitted);
        
        
        if (chisq == -1.0) {
            fail();
        }
        
        double vradDelta = 0.01;
        for (int iPoint=0; iPoint < nPoints; iPoint++) {
            float e = vradObs[iPoint];
            float a = vradFitted[iPoint];
            assertEquals(e, a, vradDelta );
        }
        

        double parameterVectorDelta = 0.01;
        for (int iParFitted = 0; iParFitted < nParsFitted; iParFitted++) {
            float e = expectedParameterVector[parameterOrder[iParFitted]];
            float a = actualParameterVector[iParFitted];
            assertEquals(e, a, parameterVectorDelta );
        }

    }

    
        
        
    
   
    
    @Test
    public void testNativeSvdFitElev30PosZ() {
        
        // this unit test's data says: orientation of the velocity field is
        // exactly in the direction of positive z:
        float[] expectedParameterVector = new float[]{0,0,1};
        
        // each row is an azimuthAngle, elevationAngle coordinate: 
        float[] points = new float[] {   0.00f,  30.00f,
                                        45.00f,  30.00f,
                                        90.00f,  30.00f,
                                       135.00f,  30.00f,
                                       180.00f,  30.00f,
                                       225.00f,  30.00f,
                                       270.00f,  30.00f,
                                       315.00f,  30.00f,
                                       360.00f,  30.00f};
        
        // the value of vrad when observing a vector [u0,v0,w0] = [0,0,1] at each point in the array 'points': 
        float[] vradObs = new float[] { 0.50f,
                                        0.50f,
                                        0.50f,
                                        0.50f,
                                        0.50f,
                                        0.50f,
                                        0.50f,
                                        0.50f,
                                        0.50f };

        chisq = svdfit(points, nDims, vradObs, vradFitted, nPoints, actualParameterVector, avar, nParsFitted);
        
        
        if (chisq == -1.0) {
            fail();
        }
        
        double vradDelta = 0.01;
        for (int iPoint=0; iPoint < nPoints; iPoint++) {
            float e = vradObs[iPoint];
            float a = vradFitted[iPoint];
            assertEquals(e, a, vradDelta );
        }
        

        double parameterVectorDelta = 0.01;
        for (int iParFitted = 0; iParFitted < nParsFitted; iParFitted++) {
            float e = expectedParameterVector[parameterOrder[iParFitted]];
            float a = actualParameterVector[iParFitted];
            assertEquals(e, a, parameterVectorDelta );
        }

    }

    
    
    
    
    
    
    
}
