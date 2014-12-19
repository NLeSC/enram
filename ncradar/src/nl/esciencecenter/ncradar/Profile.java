package nl.esciencecenter.ncradar;

public class Profile extends JNIMethodsSvdFit{

    private final int nAzim;



    public Profile(int nAzim) {
        
        this.nAzim = nAzim;

    }

    public void svdfit(int iLayer, int iData) throws Exception {
        
        int nPoints;
        int iPoint;
        int iParFitted;
        int nDims;
        int nParsFitted;
        
        float[] listOfAzimuths;
        float[] listOfElevAngles;
        float[] listOfVradObs;
        float[] listOfVradFitted;
        float[] listOfDbzObs;
        float[] parameterVector;
        float[] avar; 
        float[] points;

//        listOfVradFitted = new float[nPoints];
//        for (iPoint = 0; iPoint < nPoints; iPoint++) {
//            listOfVradFitted[iPoint] = Float.NaN; 
//        }
//        parameterVector = new float[] {Float.NaN,Float.NaN,Float.NaN};
//        nParsFitted = 3;
//        avar = new float[nParsFitted];
//        for (iParFitted = 0; iParFitted < nParsFitted; iParFitted++) {
//            avar[iParFitted] = Float.NaN; 
//        }
//
//        nDims = 2;
//        points = new float[nDims * nPoints];
//        
//        for (iPoint = 0; iPoint < nPoints; iPoint++) {
//            points[iPoint*nDims + 0] = listOfAzimuths[iPoint];
//            points[iPoint*nDims + 1] = listOfElevAngles[iPoint];
//        }
//        
//        float chisq = svdfit(points, nDims, listOfVradObs, listOfVradFitted, nPoints, parameterVector, avar, nParsFitted);

    }


}
