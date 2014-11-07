package nl.esciencecenter.ncradar;

import java.io.FileNotFoundException;
import java.io.PrintWriter;
import java.io.UnsupportedEncodingException;


public class Writer {

    public void printAsGeoJSONToCSV() throws FileNotFoundException,
            UnsupportedEncodingException {

        String filename = this.filename + ".geojson.csv";
        printAsGeoJSONToCSV(filename);

    }



    public void printAsGeoJSONToCSV(String filename)
            throws FileNotFoundException, UnsupportedEncodingException {

        PrintWriter writer = new PrintWriter(filename, "UTF-8");

        int nPolyCorners = 5;
        String headerStr = "\"the_geom\",\"reflectivity\"";
        writer.println(headerStr);

        for (int iBin = 0; iBin < numberOfRangeBins; iBin++) {

            for (int iRay = 0; iRay < numberOfAzimuthBins; iRay++) {

                String polyStr = "\"{\"\"type\"\":\"\"MultiPolygon\"\",\"\"coordinates\"\":[[[";

                for (int iPolyCorner = 0; iPolyCorner < nPolyCorners; iPolyCorner++) {
                    polyStr = polyStr
                            + String.format("[%.6f,%.6f]",
                                    polygons[iBin][iRay][iPolyCorner][0],
                                    polygons[iBin][iRay][iPolyCorner][1]);
                    if (iPolyCorner < nPolyCorners - 1) {

                        polyStr = polyStr + ",";

                    }
                }

                polyStr = polyStr + "]]]}\"";

                polyStr = polyStr + "," + scanDataRaw[iRay][iBin];

                writer.println(polyStr);

            }
        }

        writer.close();

    }



    public void printAsWKTToCSV() throws FileNotFoundException,
            UnsupportedEncodingException {

        String filename = this.filename + ".wkt.csv";
        printAsWKTToCSV(filename);

    }



    public void printAsWKTToCSV(String filename) throws FileNotFoundException,
            UnsupportedEncodingException {

        PrintWriter writer = new PrintWriter(filename, "UTF-8");

        int nPolyCorners = 5;
        int iBinMin = 0;
        int iBinMax = numberOfRangeBins;
        int iRayMin = 0;
        int iRayMax = numberOfAzimuthBins;

        String headerStr = "\"the_geom\",\"reflectivity\"";
        writer.println(headerStr);

        for (int iBin = 0; iBin < numberOfRangeBins; iBin++) {

            for (int iRay = 0; iRay < numberOfAzimuthBins; iRay++) {

                if (iBinMin <= iBin & iBin <= iBinMax & iRayMin <= iRay
                        & iRay <= iRayMax) {
                    String polyStr = "\"POLYGON((";

                    for (int iPolyCorner = 0; iPolyCorner < nPolyCorners; iPolyCorner++) {
                        polyStr = polyStr
                                + String.format("%.6f %.6f",
                                        polygons[iBin][iRay][iPolyCorner][0],
                                        polygons[iBin][iRay][iPolyCorner][1]);
                        if (iPolyCorner < nPolyCorners - 1) {

                            polyStr = polyStr + ",";

                        }
                    }
                    polyStr = polyStr + "))\"";

                    polyStr = polyStr + "," + scanDataRaw[iRay][iBin];

                    writer.println(polyStr);

                }

            }
        }

        writer.close();

    }

}
