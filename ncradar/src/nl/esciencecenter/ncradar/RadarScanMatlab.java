package nl.esciencecenter.ncradar;

import java.io.IOException;


public class RadarScanMatlab {

    private final RadarScanJava radarScanJava;



    public RadarScanMatlab(String directory, String filename, int datasetIndex) throws IOException {

        this.radarScanJava = new RadarScanJava(directory, filename, datasetIndex);

    }



    public void calcVerticesAndFaces() {

        this.radarScanJava.getPolarData().calcVerticesAndFaces();
    }



    private char[][] convertToMatlabString(String str) {

        char[] charArray = str.toCharArray();
        int nChars = charArray.length;
        char[][] charArray2D = new char[1][nChars];
        for (int iChar = 0; iChar < nChars; iChar++) {
            charArray2D[0][iChar] = charArray[iChar];
        }
        return charArray2D;
    }



    public double getDataOffset() {

        return this.radarScanJava.getDataOffset();
    }



    public double getDataScale() {

        return this.radarScanJava.getDataScale();
    }



    public double getDatasetIndex() {

        return this.radarScanJava.getDatasetIndex();
    }



    public char[][] getDatasetName() {

        char[] charArray;
        charArray = this.radarScanJava.getDatasetName().toCharArray();
        int nChars = charArray.length;
        char[][] charArray2D = new char[1][nChars];
        for (int iChar = 0; iChar < nChars; iChar++) {
            charArray2D[0][iChar] = charArray[iChar];
        }
        return charArray2D;
    }



    public char[][] getDirectory() {

        String str = this.radarScanJava.getDirectory();
        return convertToMatlabString(str);
    }



    public double getElevationAngle() {

        return this.radarScanJava.getElevationAngle();
    }



    public char[][] getEndDate() {

        String str = this.radarScanJava.getEndDate();
        return convertToMatlabString(str);
    }



    public char[][] getEndTime() {

        String str = this.radarScanJava.getEndTime();
        return convertToMatlabString(str);
    }



    public double[][] getFaces() {

        int[][] facesInt;
        double[][] facesDouble;
        int nFaces;
        int nCorners;

        facesInt = this.radarScanJava.getFaces();
        nFaces = facesInt.length;
        nCorners = 3;
        facesDouble = new double[nFaces][nCorners];

        for (int iFace = 0; iFace < nFaces; iFace++) {
            for (int iCorner = 0; iCorner < nCorners; iCorner++) {
                facesDouble[iFace][iCorner] = (double) facesInt[iFace][iCorner] + 1;
            }
        }
        return facesDouble;
    }



    public double[] getFacesValues() {

        return this.radarScanJava.getFacesValues();
    }



    public char[][] getFilename() {

        String str = this.radarScanJava.getFilename();
        return convertToMatlabString(str);
    }



    public double getiAzimFirstRay() {

        return this.radarScanJava.getiAzimFirstRay();
    }



    public double getMissingValueValue() {

        return this.radarScanJava.getMissingValueValue();
    }



    public double getNumberOfAzimuthBins() {

        return this.radarScanJava.getNumberOfAzimuthBins();
    }



    public double getNumberOfRangeBins() {

        return this.radarScanJava.getNumberOfRangeBins();
    }



    public double getRadarPositionHeight() {

        return this.radarScanJava.getRadarPositionHeight();
    }



    public double getRadarPositionLatitude() {

        return this.radarScanJava.getRadarPositionLatitude();
    }



    public double getRadarPositionLongitude() {

        return this.radarScanJava.getRadarPositionLongitude();
    }



    public RadarScanJava getRadarScanJava() {

        return this.radarScanJava;
    }



    public double getRangeOffset() {

        return this.radarScanJava.getRangeOffset();
    }



    public double getRangeScale() {

        return this.radarScanJava.getRangeScale();
    }



    public double[][] getScanData() {

        return this.radarScanJava.getScanData2D();
    }



    public double[][] getScanDataRaw() {

        int[][] scanDataRaw = this.radarScanJava.getScanDataRaw2D();
        int nRows = this.radarScanJava.getNumberOfAzimuthBins();
        int nCols = this.radarScanJava.getNumberOfRangeBins();
        double[][] scanData = new double[nRows][nCols];

        for (int iRow = 0; iRow < nRows; iRow++) {
            for (int iCol = 0; iCol < nCols; iCol++) {
                Number number = scanDataRaw[iRow][iCol];
                scanData[iRow][iCol] = number.doubleValue();
            }
        }
        return scanData;
    }



    public char[][] getScanType() {

        String str = this.radarScanJava.getScanType();
        return convertToMatlabString(str);
    }



    public char[][] getStartDate() {

        String str = this.radarScanJava.getStartDate();
        return convertToMatlabString(str);
    }



    public char[][] getStartTime() {

        String str = this.radarScanJava.getStartTime();
        return convertToMatlabString(str);
    }



    public double[][] getVertices() {

        return this.radarScanJava.getVertices();
    }

}
