package nl.esciencecenter.ncradar;

public class TestDist extends JNIMethodsVol2Bird  {

    public static void main(String[] args) {
        
        int range1; 
        int azim1;
        int range2;
        int azim2;
        float rangeScale;
        float azimScale;
        float dist;

        range1 = 12; 
        azim1 = 14;
        range2 = 67;
        azim2 = 24;
        rangeScale = 4.5f;
        azimScale = 2.2f;
        
        dist = dist(range1,azim1,range2,azim2,rangeScale,azimScale);

        System.err.println("(Java) dist = "+ dist);
       
        
        
    }

}
