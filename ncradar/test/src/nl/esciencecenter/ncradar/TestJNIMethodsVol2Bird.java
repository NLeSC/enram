package nl.esciencecenter.ncradar;

import static org.junit.Assert.*;

import org.junit.Test;

public class TestJNIMethodsVol2Bird extends JNIMethodsVol2Bird{

    
    
    @Test
    public void testNativeDist(){


        int range1 = 1;
        int azimuth1 = 2;
        int range2 = 3;
        int azimuth2 = 4;
        float rangeScale = 5.6f;
        float azimuthScale = 7.8f;
        
        float theDist;
        
        theDist = dist(range1, azimuth1, range2, azimuth2, rangeScale, azimuthScale);

        double x1 = range1 * Math.cos(2 * Math.PI * azimuth1 * azimuthScale / 360);
        double x2 = range2 * Math.cos(2 * Math.PI * azimuth2 * azimuthScale / 360);
        double y1 = range1 * Math.sin(2 * Math.PI * azimuth1 * azimuthScale / 360);
        double y2 = range2 * Math.sin(2 * Math.PI * azimuth2 * azimuthScale / 360);

        double expected = Math.sqrt(Math.pow(x1-x2, 2.0) + 
                                    Math.pow(y1-y2, 2.0));
        
        double actual = theDist;
        
        double delta = 0.0000001;
        
        assertEquals(expected, actual, delta);


    }
    

}
