package nl.esciencecenter.ncradar;

public class ScanMeta {

    private final int date;                          /*Date of scan data in YYYYMMDD.*/
    private final int time;                          /*Time of scan data in HHMMSS.*/
    private final float radarHeight;                 /*Height of radar antenna in km.*/
    private final float elevationAngle;              /*Elevation of scan in deg.*/
    private final int numberOfRangeBins;             /*Number of range bins in scan.*/
    private final int numberOfAzimuthBins;           /*Number of azimuth rays in scan.*/
    private final float binSizeRange;                /*Size of range bins in scan in km.*/
    private final float binSizeAzimuth;              /*Size of azimuth steps in scan in deg.*/
    private final int iAzimFirstRay;                 /*Ray number with which radar scan started.*/
    private final float valueOffset;                 /*Offset value of quantity contained by scan.*/
    private final float valueScale;                  /*Scale of value of quantity contained by scan.*/
    private final int missingValueValue;             /*Missing value of quantity contained by scan.*/
    private final float pulseRepeatFrequencyHigh;    /*High PRF used for scan in Hz.*/
    private final float pulseRepeatFrequencyLow;     /*Low PRF used for scan in Hz.*/
    private final float pulseLength;                 /*Pulse length in microsec.*/
    private final float radarConstant;               /*Radar constant in dB.*/
    private final float nominalMaxTransmissionPower; /*Nominal maximum TX power in kW.*/
    private final float radialVelocityAntenna;       /*Antenna velocity in deg/s.*/
    
    
    public ScanMeta(int date, int time, float radarHeight, float elevationAngle, 
                    int numberOfRangeBins, int numberOfAzimuthBins, float binSizeRange,  
                    float binSizeAzimuth, int iAzimFirstRay, float valueOffset, 
                    float valueScale, int missingValueValue, float pulseRepeatFrequencyHigh,
                    float pulseRepeatFrequencyLow, float pulseLength, float radarConstant, 
                    float nominalMaxTransmissionPower, float radialVelocityAntenna) {
        
        this.date = date;
        this.time = time;
        this.radarHeight = radarHeight; 
        this.elevationAngle = elevationAngle;
        this.numberOfRangeBins = numberOfRangeBins;
        this.numberOfAzimuthBins = numberOfAzimuthBins;
        this.binSizeRange = binSizeRange;
        this.binSizeAzimuth = binSizeAzimuth;
        this.iAzimFirstRay = iAzimFirstRay;
        this.valueOffset = valueOffset;
        this.valueScale = valueScale;
        this.missingValueValue = missingValueValue;
        this.pulseRepeatFrequencyHigh = pulseRepeatFrequencyHigh;
        this.pulseRepeatFrequencyLow = pulseRepeatFrequencyLow;
        this.pulseLength = pulseLength;
        this.radarConstant = radarConstant;
        this.nominalMaxTransmissionPower = nominalMaxTransmissionPower;
        this.radialVelocityAntenna = radialVelocityAntenna;
        
    }



    public int getDate() {
        return date;
    }



    public int getTime() {
        return time;
    }



    public float getRadarHeight() {
        return radarHeight;
    }



    public float getElevationAngle() {
        return elevationAngle;
    }



    public int getNumberOfRangeBins() {
        return numberOfRangeBins;
    }



    public int getNumberOfAzimuthBins() {
        return numberOfAzimuthBins;
    }



    public float getBinSizeRange() {
        return binSizeRange;
    }



    public float getBinSizeAzimuth() {
        return binSizeAzimuth;
    }



    public int getiAzimFirstRay() {
        return iAzimFirstRay;
    }



    public float getValueOffset() {
        return valueOffset;
    }



    public float getValueScale() {
        return valueScale;
    }



    public int getMissingValueValue() {
        return missingValueValue;
    }



    public float getPulseRepeatFrequencyHigh() {
        return pulseRepeatFrequencyHigh;
    }



    public float getPulseRepeatFrequencyLow() {
        return pulseRepeatFrequencyLow;
    }



    public float getPulseLength() {
        return pulseLength;
    }



    public float getRadarConstant() {
        return radarConstant;
    }



    public float getNominalMaxTransmissionPower() {
        return nominalMaxTransmissionPower;
    }



    public float getRadialVelocityAntenna() {
        return radialVelocityAntenna;
    }

}
