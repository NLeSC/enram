package nl.esciencecenter.ncradar;

import java.io.IOException;


public class RadarMeta extends NetcdfAttributeReader {

    private final float nominalMaxTransmissionPower;
    private final float pulseLength;
    private final float pulseRepeatFrequencyHigh;
    private final float pulseRepeatFrequencyLow;
    private final float radarConstant;
    private final double radarPositionHeight;
    private final double radarPositionLatitude;
    private final double radarPositionLongitude;
    private final float radialVelocityAntenna;



    public RadarMeta(String fullFilename) throws IOException {

        this.pulseRepeatFrequencyHigh = -1f;
        this.pulseRepeatFrequencyLow = -1f;
        this.pulseLength = -1f;
        this.radarConstant = -1f;
        this.nominalMaxTransmissionPower = -1f;
        this.radialVelocityAntenna = -1f;

        this.radarPositionHeight = readRadarPositionHeight(fullFilename);
        this.radarPositionLatitude = readRadarPositionLatitude(fullFilename);
        this.radarPositionLongitude = readRadarPositionLongitude(fullFilename);

    }



    protected float getNominalMaxTransmissionPower() {

        return nominalMaxTransmissionPower;
    }



    protected float getPulseLength() {

        return pulseLength;
    }



    protected float getPulseRepeatFrequencyHigh() {

        return pulseRepeatFrequencyHigh;
    }



    protected float getPulseRepeatFrequencyLow() {

        return pulseRepeatFrequencyLow;
    }



    protected float getRadarConstant() {

        return radarConstant;
    }



    protected double getRadarPositionHeight() {

        return radarPositionHeight;
    }



    protected double getRadarPositionLatitude() {

        return radarPositionLatitude;
    }



    protected double getRadarPositionLongitude() {

        return radarPositionLongitude;
    }



    protected float getRadialVelocityAntenna() {

        return radialVelocityAntenna;
    }



    private double readRadarPositionHeight(String fullFilename) throws IOException {

        String fullAttributename = "where_height";
        return readDoubleAttribute(fullFilename, fullAttributename);
    }



    private double readRadarPositionLatitude(String fullFilename) throws IOException {

        String fullAttributename = "where_lat";
        return readDoubleAttribute(fullFilename, fullAttributename);
    }



    private double readRadarPositionLongitude(String fullFilename) throws IOException {

        String fullAttributename = "where_lon";
        return readDoubleAttribute(fullFilename, fullAttributename);
    }

}
