#!/bin/bash

# start this script from the terminal by (make sure to enter it exactly):
# . ./start-processing.sh

outfile='log/stdout.txt'
errfile='log/stderr.txt'


echo "Do you wish to process the test data or the full data? (Enter 1 or 2)"
select answer in "Test data only" "Full data"; do
    case $answer in
        "Test data only" ) 
           echo "Test data only" 1>$outfile 2>$errfile
           echo "The following `cat testdata/stations.txt| wc -w` station codes will be processed:" 1>>$outfile
           cat testdata/stations.txt 1>>$outfile
           echo "" 1>>$outfile
           echo "" 1>>$outfile
           echo "The relevant environment variables are:" 1>>$outfile
           . ./set-idl-enram-test-environment.sh
           break
           ;;
        "Full data" )
           echo "Full data" 1>$outfile 2>$errfile
           echo "The following `cat data/stations.txt| wc -w` station codes will be processed:" 1>>$outfile
           cat data/stations.txt 1>>$outfile
           echo "" 1>>$outline
           echo "" 1>>$outfile
           echo "" 1>>$outfile
           echo "The relevant environment variables are:" 1>>$outfile
           . ./set-idl-enram-environment.sh
           break
           ;;
    esac
done

echo `date`": Started processing based on $answer..."
echo "(processing logs are located in '$outfile' and '$errfile')"
echo "You can use the browser to inspect those files while the ENRAM scripts are running, but leave this window open."


echo "IDL_ENRAM_VOL2BIRDLIB='$IDL_ENRAM_VOL2BIRDLIB'" 1>>$outfile
echo "IDL_ENRAM_SRC_IO='$IDL_ENRAM_SRC_IO'" 1>>$outfile
echo "IDL_ENRAM_RAW_DATA='$IDL_ENRAM_RAW_DATA'" 1>>$outfile
echo "IDL_ENRAM_ODIM_DATA='$IDL_ENRAM_ODIM_DATA'" 1>>$outfile
echo "IDL_ENRAM_BIRD_DATA='$IDL_ENRAM_BIRD_DATA'" 1>>$outfile
echo "IDL_ENRAM_CLUTTERMAPS='$IDL_ENRAM_CLUTTERMAPS'" 1>>$outfile
echo "IDL_ENRAM_STATISTICS='$IDL_ENRAM_STATISTICS'" 1>>$outfile
echo "IDL_ENRAM_CLUTTER_SENSITIVITY='$IDL_ENRAM_CLUTTER_SENSITIVITY'" 1>>$outfile
echo "IDL_ENRAM_VISUALIZATION_OUTPUT='$IDL_ENRAM_VISUALIZATION_OUTPUT'" 1>>$outfile


idl -e ".r histogram.pro" 1>>$outfile 2>>$errfile

idl -e ".r cluttermap.pro" 1>>$outfile 2>>$errfile

idl -e ".r flysafe2.pro" 1>>$outfile 2>>$errfile

idl -e ".r clutter_sensitivity.pro" 1>>$outfile 2>>$errfile

idl -e ".r make_colored_donut_maps.pro" 1>>$outfile 2>>$errfile

idl -e ".r make_ppi_time_space_bird_rain.pro" 1>>$outfile 2>>$errfile

idl -e ".r multi_ppi.pro" 1>>$outfile 2>>$errfile

idl -e ".r multi_prof.pro" 1>>$outfile 2>>$errfile


echo `date`": Started processing based on $answer...Done"



