package nl.esciencecenter.ncradar;


import java.io.IOException;

import ucar.nc2.Attribute;
import ucar.nc2.NetcdfFile;

public class NetcdfAttributeReader {

    int readIntegerAttribute(String fullFilename,String fullAttributename) throws IOException {

        Attribute attr; 
        NetcdfFile ncfile = null;
        Number number;
        try {
            ncfile = NetcdfFile.open(fullFilename);
            attr = ncfile.findGlobalAttribute(fullAttributename);
            if (attr==null){
                System.err.println("Looks like there is no attribute by the name '" + fullAttributename + "' in file '" +fullFilename+"'.");
            }
            number = attr.getNumericValue();
        } finally {
            ncfile.close();
        }
        return number.intValue();
    }

    long readLongAttribute(String fullFilename,String fullAttributename) throws IOException {

        Attribute attr;
        NetcdfFile ncfile = null;
        try {
            ncfile = NetcdfFile.open(fullFilename);
            attr = ncfile.findGlobalAttribute(fullAttributename);
            if (attr==null){
                System.err.println("Looks like there is no attribute by the name '" + fullAttributename + "' in file '" +fullFilename+"'.");
            }
        } finally {
            ncfile.close();
        }
        return (Long) attr.getNumericValue();
    }

    double readDoubleAttribute(String fullFilename,String fullAttributename) throws IOException {
        
        Attribute attr; 
        NetcdfFile ncfile = null;
        try {
            ncfile = NetcdfFile.open(fullFilename);
            attr = ncfile.findGlobalAttribute(fullAttributename);
            if (attr==null){
                System.err.println("Looks like there is no attribute by the name '" + fullAttributename + "' in file '" +fullFilename+"'.");
            }
        } finally {
            ncfile.close();
        }
        return (Double) attr.getNumericValue();
    }


    String readStringAttribute(String fullFilename,String fullAttributename) throws IOException {
        
        Attribute attr;
        NetcdfFile ncfile = null;

        try {
            ncfile = NetcdfFile.open(fullFilename);
            attr = ncfile.findGlobalAttribute(fullAttributename);
            if (attr==null){
                System.err.println("Looks like there is no attribute by the name '" + fullAttributename + "' in file '" +fullFilename+"'.");
            }
        } finally {
            ncfile.close();
        }
        return attr.getStringValue();
    }

}
