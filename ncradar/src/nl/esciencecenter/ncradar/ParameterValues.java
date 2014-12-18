package nl.esciencecenter.ncradar;

public class ParameterValues {

    private final int AREACELL;
    private final double AZIMMAX;
    private final double AZIMMIN;
    private final double CLUTPERCCELL;
    private final double DBZCELL;
    private final double DBZCLUTTER;
    private final double DBZFACTOR;
    private final double DBZMAX;
    private final double DBZMIN;
    private final double DBZNOISE;
    private final double DBZRAIN;
    private final double EMASKMAX;
    private final double HLAYER;
    private final int NDATA;
    private final int NDBZMIN;
    private final int NEIGHBOURS;
    private final int NGAPBIN;
    private final int NGAPMIN;
    private final int NLAYER;
    private final int NTEXBINAZIM;
    private final int NTEXBINRANG;
    private final int NTEXMIN;
    private final double RANGMAX;
    private final double RANGMAXSTDEV;
    private final double RANGMIN;
    private final double RANGMINSTDEV;
    private final double SIGMABIRD;
    private final double STDEVBIRD;
    private final double STDEVCELL;
    private final double STDEVSCALE;
    private final double VDIFMAX;
    private final double VMASKMAX;
    private final double VRADMIN;



    public ParameterValues() {

        // FIXME still have to rename properties to more meaningful names;
        // FIXME also make compliant with naming convention

        this.HLAYER = 0.200; // p. 84
        this.NLAYER = 30;
        this.NDATA = 4;
        this.RANGMIN = 5.0 * 1000; // p. 84 says 10.0  // FIXME original value was in km
        this.RANGMINSTDEV = 5.0; // equivalent of RANGMINSTDEV, but for VRAD
                                 // instead of DBZ?
        this.RANGMAXSTDEV = 25.0;
        this.RANGMAX = 25.0 * 1000; // p.84   // FIXME original value was in km
        this.AZIMMIN = 0.0;
        this.AZIMMAX = 360.0;
        this.VRADMIN = 1.0; // p. 74
        this.NGAPBIN = 8;
        this.NGAPMIN = 5;
        this.NDBZMIN = 25;
        this.VDIFMAX = 10.0;
        this.VMASKMAX = 0.0;
        this.EMASKMAX = 500.0;  // original value was 5, but that does nothing. In any case, must be same units as rangeScale 
        this.DBZMIN = -100.0;
        this.DBZMAX = 20.0;
        this.DBZNOISE = -40.0;
        this.DBZRAIN = 0.0; // p.75
        this.DBZCELL = 15.0; // p. 78
        this.STDEVCELL = 5.0; // p. 78
        this.AREACELL = 4;
        this.CLUTPERCCELL = 0.5; // p. 78
        this.NEIGHBOURS = 5;
        this.STDEVSCALE = 0.1;
        this.NTEXBINAZIM = 3;
        this.NTEXBINRANG = 3;
        this.NTEXMIN = 4;
        this.DBZCLUTTER = -10.0;
        this.DBZFACTOR = 360.7; // eq. 7.13 p. 89
        this.SIGMABIRD = 11.0; // p.89
        this.STDEVBIRD = 2.0; // p. 86

    }



    public int getAREACELL() {

        return AREACELL;
    }



    public double getAZIMMAX() {

        return AZIMMAX;
    }



    public double getAZIMMIN() {

        return AZIMMIN;
    }



    public double getCLUTPERCCELL() {

        return CLUTPERCCELL;
    }



    public double getDBZCELL() {

        return DBZCELL;
    }



    public double getDBZCLUTTER() {

        return DBZCLUTTER;
    }



    public double getDBZFACTOR() {

        return DBZFACTOR;
    }



    public double getDBZMAX() {

        return DBZMAX;
    }



    public double getDBZMIN() {

        return DBZMIN;
    }



    public double getDBZNOISE() {

        return DBZNOISE;
    }



    public double getDBZRAIN() {

        return DBZRAIN;
    }



    public double getEMASKMAX() {

        return EMASKMAX;
    }



    public double getHLAYER() {

        return HLAYER;
    }



    public int getNDATA() {

        return NDATA;
    }



    public int getNDBZMIN() {

        return NDBZMIN;
    }



    public int getNEIGHBOURS() {

        return NEIGHBOURS;
    }



    public int getNGAPBIN() {

        return NGAPBIN;
    }



    public int getNGAPMIN() {

        return NGAPMIN;
    }



    public int getNLAYER() {

        return NLAYER;
    }



    public int getNTEXBINAZIM() {

        return NTEXBINAZIM;
    }



    public int getNTEXBINRANG() {

        return NTEXBINRANG;
    }



    public int getNTEXMIN() {

        return NTEXMIN;
    }



    public double getRANGMAX() {

        return RANGMAX;
    }



    public double getRANGMAXSTDEV() {

        return RANGMAXSTDEV;
    }



    public double getRANGMIN() {

        return RANGMIN;
    }



    public double getRANGMINSTDEV() {

        return RANGMINSTDEV;
    }



    public double getSIGMABIRD() {

        return SIGMABIRD;
    }



    public double getSTDEVBIRD() {

        return STDEVBIRD;
    }



    public double getSTDEVCELL() {

        return STDEVCELL;
    }



    public double getSTDEVSCALE() {

        return STDEVSCALE;
    }



    public double getVDIFMAX() {

        return VDIFMAX;
    }



    public double getVMASKMAX() {

        return VMASKMAX;
    }



    public double getVRADMIN() {

        return VRADMIN;
    }




}
