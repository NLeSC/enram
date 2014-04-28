package nl.esciencecenter.ncradar;

import java.io.IOException;

public class TestFringeCells extends JNIMethodsVol2Bird {
    
    public TestFringeCells() throws IOException {
        

      int[] cellImage = new int[] {2,3,0,1,2,3,4,5};
      int nRang = 4;
      int nAzim = 2;
      float aScale = 0.4f;
      float rScale = 0.9f;
      float fringe = 2;
      
      fringecells(cellImage, nRang, nAzim, aScale, rScale, fringe);
        

    }

}
