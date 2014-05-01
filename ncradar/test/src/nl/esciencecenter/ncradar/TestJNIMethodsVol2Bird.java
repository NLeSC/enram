package nl.esciencecenter.ncradar;

import static org.junit.Assert.*;

import org.junit.Test;

public class TestJNIMethodsVol2Bird extends JNIMethodsVol2Bird{

    
    
    @Test
    public void testNativeDist(){

        // blah
        int range1 = 1;
        int azimuth1 = 2;
        int range2 = 3;
        int azimuth2 = 4;
        float rangeScale = 5.6f;
        float azimuthScale = 7.8f;
        
        float theDist;
        
        theDist = dist(range1, azimuth1, range2, azimuth2, rangeScale, azimuthScale);

        double expected = Math.sqrt(Math.pow((range1-range2)*rangeScale, 2.0) + 
                                    Math.pow((azimuth1-azimuth2)*azimuthScale, 2.0));
        double actual = theDist;
        
        double delta = 0.0000001;
        
        assertEquals(expected, actual, delta);


    }
    

}
