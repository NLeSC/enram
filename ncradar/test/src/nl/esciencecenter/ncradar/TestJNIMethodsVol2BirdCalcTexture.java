package nl.esciencecenter.ncradar;

import static org.junit.Assert.*;

import org.junit.Before;
import org.junit.Test;

public class TestJNIMethodsVol2BirdCalcTexture extends JNIMethodsVol2Bird{


    private byte[] texImage;
    private byte[] reflImage;
    private byte[] vradImage;
    private byte nRangNeighborhood;
    private byte nAzimNeighborhood;
    private byte nCountMin;
    private byte texType;
    private float texOffset;
    private float texScale;
    private float reflOffset;
    private float reflScale;
    private float vradOffset;
    private float vradScale;
    private int nRang;
    private int nAzim;
    private float delta = 0.0000001f;


    @Before
    public void setUp() throws Exception  {
        
        texImage = new byte[] {0,0,0,0,
                               0,0,0,0,
                               0,0,0,0};
        reflImage = new byte[] { 3,46,56, 7,
                                84, 6,78,72,
                                 3, 5,23,56};
        vradImage = new byte[] { 8,94,56,65,
                                23, 2,46,73,
                                 5, 6,34,53};
        nRangNeighborhood = 3;
        nAzimNeighborhood = 3;
        nCountMin = 0;
        texType = 1;
        texOffset = 0.0f;
        texScale =  1.0f;
        reflOffset = 2.0f;
        reflScale = -34.0f;
        vradOffset = 20f;
        vradScale = 56.0f;
        nRang = 4;
        nAzim = 3;

    }



    @Test
    public void testNativeCalcTexture() throws Exception{
        
        calcTexture(texImage,
                    reflImage,
                    vradImage,
                    nRangNeighborhood,
                    nAzimNeighborhood,
                    nCountMin,
                    texType,
                    texOffset,
                    texScale,
                    reflOffset,
                    reflScale,
                    vradOffset,
                    vradScale,
                    nRang,
                    nAzim);
        
        
        fail("Not complete yet");
        

       
    }


}
