package nl.esciencecenter.ncradar;

public class ParameterValues {

    private int AREACELL;
    private double AZIMMAX;
    private double AZIMMIN;
    private double CLUTPERCCELL;
    private double DBZCELL;
    private double DBZCLUTTER;
    private double DBZFACTOR;
    private double DBZMAX;
    private double DBZMIN;
    private double DBZNOISE;
    private double DBZRAIN;
    private double EMASKMAX;
    private double HLAYER;
    private int NDATA;
    private int NDBZMIN;
    private int NEIGHBOURS;
    private int NGAPBIN;
    private int NGAPMIN;
    private int NLAYER;
    private int NTEXBINAZIM;
    private int NTEXBINRANG;
    private int NTEXMIN;
    private double RANGMAX;
    private double RANGMAXSTDEV;
    private double RANGMIN;
    private double RANGMINSTDEV;
    private double RHOMIN;
    private double SIGMABIRD;
    private double STDEVBIRD;
    private double STDEVCELL;
    private double STDEVSCALE;
    private double VDIFMAX;
    private double VMASKMAX;
    private double VRADMIN;
    private double ZDRMIN;



    public ParameterValues() {

        // FIXME still have to rename properties to more meaningful names;
        // FIXME also make compliant with naming convention

        this.HLAYER = 0.200;
        this.NLAYER = 30;
        this.NDATA = 4;
        this.RANGMIN = 5.0;
        this.RANGMINSTDEV = 5.0;
        this.RANGMAXSTDEV = 25.0;
        this.RANGMAX = 25.0;
        this.AZIMMIN = 0.0;
        this.AZIMMAX = 360.0;
        this.VRADMIN = 1.0;
        this.NGAPBIN = 8;
        this.NGAPMIN = 5;
        this.NDBZMIN = 25;
        this.VDIFMAX = 10.0;
        this.VMASKMAX = 0.0;
        this.EMASKMAX = 5.0;
        this.RHOMIN = 0.9;
        this.ZDRMIN = 3.0;
        this.DBZMIN = -100.0;
        this.DBZMAX = 20.0;
        this.DBZNOISE = -40.0;
        this.DBZRAIN = 0.0;
        this.DBZCELL = 15.0;
        this.STDEVCELL = 5.0;
        this.AREACELL = 4;
        this.CLUTPERCCELL = 0.5;
        this.NEIGHBOURS = 5;
        this.STDEVSCALE = 0.1;
        this.NTEXBINAZIM = 3;
        this.NTEXBINRANG = 3;
        this.NTEXMIN = 4;
        this.DBZCLUTTER = -10.0;
        this.DBZFACTOR = 360.7;
        this.SIGMABIRD = 11.0;
        this.STDEVBIRD = 2.0;

    }



    protected int getAREACELL() {

        return AREACELL;
    }



    protected double getAZIMMAX() {

        return AZIMMAX;
    }



    protected double getAZIMMIN() {

        return AZIMMIN;
    }



    protected double getCLUTPERCCELL() {

        return CLUTPERCCELL;
    }



    protected double getDBZCELL() {

        return DBZCELL;
    }



    protected double getDBZCLUTTER() {

        return DBZCLUTTER;
    }



    protected double getDBZFACTOR() {

        return DBZFACTOR;
    }



    protected double getDBZMAX() {

        return DBZMAX;
    }



    protected double getDBZMIN() {

        return DBZMIN;
    }



    protected double getDBZNOISE() {

        return DBZNOISE;
    }



    protected double getDBZRAIN() {

        return DBZRAIN;
    }



    protected double getEMASKMAX() {

        return EMASKMAX;
    }



    protected double getHLAYER() {

        return HLAYER;
    }



    protected int getNDATA() {

        return NDATA;
    }



    protected int getNDBZMIN() {

        return NDBZMIN;
    }



    protected int getNEIGHBOURS() {

        return NEIGHBOURS;
    }



    protected int getNGAPBIN() {

        return NGAPBIN;
    }



    protected int getNGAPMIN() {

        return NGAPMIN;
    }



    protected int getNLAYER() {

        return NLAYER;
    }



    protected int getNTEXBINAZIM() {

        return NTEXBINAZIM;
    }



    protected int getNTEXBINRANG() {

        return NTEXBINRANG;
    }



    protected int getNTEXMIN() {

        return NTEXMIN;
    }



    protected double getRANGMAX() {

        return RANGMAX;
    }



    protected double getRANGMAXSTDEV() {

        return RANGMAXSTDEV;
    }



    protected double getRANGMIN() {

        return RANGMIN;
    }



    protected double getRANGMINSTDEV() {

        return RANGMINSTDEV;
    }



    protected double getRHOMIN() {

        return RHOMIN;
    }



    protected double getSIGMABIRD() {

        return SIGMABIRD;
    }



    protected double getSTDEVBIRD() {

        return STDEVBIRD;
    }



    protected double getSTDEVCELL() {

        return STDEVCELL;
    }



    protected double getSTDEVSCALE() {

        return STDEVSCALE;
    }



    protected double getVDIFMAX() {

        return VDIFMAX;
    }



    protected double getVMASKMAX() {

        return VMASKMAX;
    }



    protected double getVRADMIN() {

        return VRADMIN;
    }



    protected double getZDRMIN() {

        return ZDRMIN;
    }



    protected void setAREACELL(int aREACELL) {

        AREACELL = aREACELL;
    }



    protected void setAZIMMAX(double aZIMMAX) {

        AZIMMAX = aZIMMAX;
    }



    protected void setAZIMMIN(double aZIMMIN) {

        AZIMMIN = aZIMMIN;
    }



    protected void setCLUTPERCCELL(double cLUTPERCCELL) {

        CLUTPERCCELL = cLUTPERCCELL;
    }



    protected void setDBZCELL(double dBZCELL) {

        DBZCELL = dBZCELL;
    }



    protected void setDBZCLUTTER(double dBZCLUTTER) {

        DBZCLUTTER = dBZCLUTTER;
    }



    protected void setDBZFACTOR(double dBZFACTOR) {

        DBZFACTOR = dBZFACTOR;
    }



    protected void setDBZMAX(double dBZMAX) {

        DBZMAX = dBZMAX;
    }



    protected void setDBZMIN(double dBZMIN) {

        DBZMIN = dBZMIN;
    }



    protected void setDBZNOISE(double dBZNOISE) {

        DBZNOISE = dBZNOISE;
    }



    protected void setDBZRAIN(double dBZRAIN) {

        DBZRAIN = dBZRAIN;
    }



    protected void setEMASKMAX(double eMASKMAX) {

        EMASKMAX = eMASKMAX;
    }



    protected void setHLAYER(float hLAYER) {

        HLAYER = hLAYER;
    }



    protected void setNDATA(int nDATA) {

        NDATA = nDATA;
    }



    protected void setNDBZMIN(int nDBZMIN) {

        NDBZMIN = nDBZMIN;
    }



    protected void setNEIGHBOURS(int nEIGHBOURS) {

        NEIGHBOURS = nEIGHBOURS;
    }



    protected void setNGAPBIN(int nGAPBIN) {

        NGAPBIN = nGAPBIN;
    }



    protected void setNGAPMIN(int nGAPMIN) {

        NGAPMIN = nGAPMIN;
    }



    protected void setNLAYER(int nLAYER) {

        NLAYER = nLAYER;
    }



    protected void setNTEXBINAZIM(int nTEXBINAZIM) {

        NTEXBINAZIM = nTEXBINAZIM;
    }



    protected void setNTEXBINRANG(int nTEXBINRANG) {

        NTEXBINRANG = nTEXBINRANG;
    }



    protected void setNTEXMIN(int nTEXMIN) {

        NTEXMIN = nTEXMIN;
    }



    protected void setRANGMAX(double rANGMAX) {

        RANGMAX = rANGMAX;
    }



    protected void setRANGMAXSTDEV(double rANGMAXSTDEV) {

        RANGMAXSTDEV = rANGMAXSTDEV;
    }



    protected void setRANGMIN(double rANGMIN) {

        RANGMIN = rANGMIN;
    }



    protected void setRANGMIN(float rANGMIN) {

        RANGMIN = rANGMIN;
    }



    protected void setRANGMINSTDEV(double rANGMINSTDEV) {

        RANGMINSTDEV = rANGMINSTDEV;
    }



    protected void setRHOMIN(double rHOMIN) {

        RHOMIN = rHOMIN;
    }



    protected void setSIGMABIRD(double sIGMABIRD) {

        SIGMABIRD = sIGMABIRD;
    }



    protected void setSTDEVBIRD(double sTDEVBIRD) {

        STDEVBIRD = sTDEVBIRD;
    }



    protected void setSTDEVCELL(double sTDEVCELL) {

        STDEVCELL = sTDEVCELL;
    }



    protected void setSTDEVSCALE(double sTDEVSCALE) {

        STDEVSCALE = sTDEVSCALE;
    }



    protected void setVDIFMAX(double vDIFMAX) {

        VDIFMAX = vDIFMAX;
    }



    protected void setVMASKMAX(double vMASKMAX) {

        VMASKMAX = vMASKMAX;
    }



    protected void setVRADMIN(double vRADMIN) {

        VRADMIN = vRADMIN;
    }



    protected void setZDRMIN(double zDRMIN) {

        ZDRMIN = zDRMIN;
    }

}
