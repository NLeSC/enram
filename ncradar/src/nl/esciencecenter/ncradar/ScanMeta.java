package nl.esciencecenter.ncradar;

public class ScanMeta {

    private final int datasetIndex;
    private final String datasetName;
    private final String directory;
    private final double elevationAngle;
    private final String endDate;
    private final String endTime;
    private final String filename;
    private final String scanType;
    private final String startDate;
    private final String startTime;



    public ScanMeta(int datasetIndex, String datasetName, String directory,
            double elevationAngle, String endDate, String endTime,
            String filename, String scanType, String startDate,
            String startTime) {

        this.datasetIndex = datasetIndex;
        this.datasetName = datasetName;
        this.directory = directory;
        this.elevationAngle = elevationAngle;
        this.endDate = endDate;
        this.endTime = endTime;
        this.filename = filename;
        this.scanType = scanType;
        this.startDate = startDate;
        this.startTime = startTime;

    }



    protected int getDatasetIndex() {

        return datasetIndex;
    }



    protected String getDatasetName() {

        return datasetName;
    }



    protected String getDirectory() {

        return directory;
    }



    protected double getElevationAngle() {

        return elevationAngle;
    }



    protected String getEndDate() {

        return endDate;
    }



    protected String getEndTime() {

        return endTime;
    }



    protected String getFilename() {

        return filename;
    }



    protected String getScanType() {

        return scanType;
    }



    protected String getStartDate() {

        return startDate;
    }



    protected String getStartTime() {

        return startTime;
    }

}
