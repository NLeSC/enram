package nl.esciencecenter.ncradar;

public class BirdDensityProfile extends JNIMethodsVol2Bird {

    private final RadarScanJava radarScanReflectivity;
    private final RadarScanJava radarScanRadialVelocity;
    private byte[][] texture;
    private final int nRang;
    private final int nAzim;



    public BirdDensityProfile(RadarScanJava radarScanReflectivity,
            RadarScanJava radarScanRadialVelocity) throws Exception {

        this.radarScanReflectivity = radarScanReflectivity;
        this.radarScanRadialVelocity = radarScanRadialVelocity;

        nAzim = (int) radarScanReflectivity.getNumberOfAzimuthBins();
        nRang = (int) radarScanReflectivity.getNumberOfRangeBins();

        int nAzim = (int) radarScanReflectivity.getNumberOfAzimuthBins();
        int nRang = (int) radarScanReflectivity.getNumberOfRangeBins();

        if (nAzim != this.nAzim) {
            throw new Exception("Number of azimuth bins differ.");
        }
        if (nRang != this.nRang) {
            throw new Exception("Number of range bins differ.");
        }

    }



    public void calcTexture(byte nRangNeighborhood, byte nAzimNeighborhood,
            byte nCountMin, byte texType, float texOffset, float texScale) {

        byte[][] reflImage2D = radarScanReflectivity.getScanDataRaw();
        byte[][] vradImage2D = radarScanReflectivity.getScanDataRaw();

        byte[] texImage = new byte[nAzim * nRang];
        byte[] reflImage = new byte[nAzim * nRang];
        byte[] vradImage = new byte[nAzim * nRang];

        int iGlobal = 0;
        for (int iAzim = 0; iAzim < nAzim; iAzim++) {
            for (int iRang = 0; iRang < nRang; iRang++) {
                reflImage[iGlobal] = reflImage2D[iAzim][iRang];
                vradImage[iGlobal] = vradImage2D[iAzim][iRang];
                iGlobal++;
            }
        }

        float reflOffset = (float) radarScanReflectivity.getOffset();
        float reflScale = (float) radarScanReflectivity.getGain();
        float vradOffset = (float) radarScanRadialVelocity.getOffset();
        float vradScale = (float) radarScanRadialVelocity.getGain();

        calcTexture(texImage, reflImage, vradImage, nRangNeighborhood,
                nAzimNeighborhood, nCountMin, texType, texOffset, texScale,
                reflOffset, reflScale, vradOffset, vradScale, nRang, nAzim);

        iGlobal = 0;
        byte[][] texImage2D = new byte[nAzim][nRang];
        for (int iAzim = 0; iAzim < nAzim; iAzim++) {
            for (int iRang = 0; iRang < nRang; iRang++) {
                texImage2D[iAzim][iRang] = texImage[iGlobal];
                iGlobal++;
            }
        }

        texture = texImage2D.clone();

    }



    public double[][] getTexture() {

        byte[][] textureRaw = getTextureRaw();
        double[][] texture = new double[nAzim][nRang];

        for (int iAzim = 0; iAzim < nAzim; iAzim++) {
            for (int iRang = 0; iRang < nRang; iRang++) {

                Number number = textureRaw[iAzim][iRang];
                texture[iAzim][iRang] = number.doubleValue();
            }
        }

        return texture;

    };



    protected byte[][] getTextureRaw() {

        return texture.clone();
    }

}
