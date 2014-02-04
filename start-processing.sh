#!/bin/bash

# execute this script in a terminal by:
# . set-idl-enram-test-environment.sh
# (don't omit the dot at the beginning)
# then stat idl or idlde from that same terminal with
# idl &
# or
# idlde &

# location of the enram directory
enramroot='/home/wbouten/enram/'

# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 
#             NO NEED TO EDIT ANYTHING BELOW THIS LINE                #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 




echo "Do you want an interactive session or a batch processing session? (Enter 1 or 2)"
select answer1 in "Interactive" "Batch"; do
    case $answer1 in
        "Interactive" ) 
           isInteractive=true
           break
           ;;
        "Batch" )
           isInteractive=false
           break
           ;;
    esac
done


echo "Do you wish to set up the environment for processing the test data or the full data? (Enter 1 or 2)"
select answer2 in "Test data only" "Full data"; do
    case $answer2 in
        "Test data only" ) 
           dataStr='testdata'
           break
           ;;
        "Full data" )
           dataStr='data'
           break
           ;;
    esac
done


# set IDL paths
. /home/wbouten/exelis/idl/bin/idl_setup.bash

# define where the IDL binary is located:
export IDL_DIR=~/exelis/idl82;

# define in which directories IDL will search for functions, scripts, and so on:
export IDL_PATH=+${enramroot}src:+$IDL_DIR;

# define the location of the startup file:
export IDL_STARTUP=${enramroot}'src/idl_startup.pro'

# define where the different ENRAM directories are:
export IDL_ENRAM_VOL2BIRDLIB=${enramroot}'src/vol2bird/vol2bird.so'
export IDL_ENRAM_SRC_IO=${enramroot}'src/io/'
export IDL_ENRAM_DATA=${enramroot}${dataStr}'/'
export IDL_ENRAM_RAW_DATA=${enramroot}${dataStr}'/raw/'
export IDL_ENRAM_ODIM_DATA=${enramroot}${dataStr}'/harmonized/odim/'
export IDL_ENRAM_BIRD_DATA=${enramroot}${dataStr}'/harmonized/bird/'
export IDL_ENRAM_CLUTTERMAPS=${enramroot}${dataStr}'/harmonized/cluttermaps/'
export IDL_ENRAM_STATISTICS=${enramroot}${dataStr}'/harmonized/statistics/'
export IDL_ENRAM_CLUTTER_SENSITIVITY=${enramroot}${dataStr}'/harmonized/clutter-sensitivity/'
export IDL_ENRAM_VISUALIZATION_OUTPUT=${enramroot}'visualization-output/'
export IDL_ENRAM_TEMPLATES=${enramroot}'other-data/templates.sav'


if $isInteractive; then  

   idlde &
   
else

   
    outfile=${enramroot}'log/stdout.txt'
    errfile=${enramroot}'log/stderr.txt'


	echo `date`": Started processing based on "$answer2"..." | tee $outfile
	echo "(processing logs are located in "$outfile" and "$errfile")"
	echo "You can use the browser to inspect those files while the ENRAM scripts are running, but leave this window open."
	
	echo "IDL_ENRAM_VOL2BIRDLIB="${IDL_ENRAM_VOL2BIRDLIB} 1>>$outfile
	echo "IDL_ENRAM_SRC_IO="${IDL_ENRAM_SRC_IO} 1>>$outfile
	echo "IDL_ENRAM_RAW_DATA="${IDL_ENRAM_RAW_DATA} 1>>$outfile
	echo "IDL_ENRAM_ODIM_DATA="${IDL_ENRAM_ODIM_DATA} 1>>$outfile
	echo "IDL_ENRAM_BIRD_DATA="${IDL_ENRAM_BIRD_DATA} 1>>$outfile
	echo "IDL_ENRAM_CLUTTERMAPS="${IDL_ENRAM_CLUTTERMAPS} 1>>$outfile
	echo "IDL_ENRAM_STATISTICS="${IDL_ENRAM_STATISTICS} 1>>$outfile
	echo "IDL_ENRAM_CLUTTER_SENSITIVITY="${IDL_ENRAM_CLUTTER_SENSITIVITY} 1>>$outfile
	echo "IDL_ENRAM_VISUALIZATION_OUTPUT="${IDL_ENRAM_VISUALIZATION_OUTPUT} 1>>$outfile
	echo "IDL_ENRAM_TEMPLATES="${IDL_ENRAM_TEMPLATES} 1>>$outfile
	
	idl -e ".r histogram.pro" 1>>$outfile 2>>$errfile
	
	idl -e ".r cluttermap.pro" 1>>$outfile 2>>$errfile
	
	idl -e ".r flysafe2.pro" 1>>$outfile 2>>$errfile
	
	idl -e ".r clutter_sensitivity.pro" 1>>$outfile 2>>$errfile
	
	idl -e ".r make_colored_donut_maps.pro" 1>>$outfile 2>>$errfile
	
	idl -e ".r make_ppi_time_space_bird_rain.pro" 1>>$outfile 2>>$errfile
	
	idl -e ".r multi_ppi.pro" 1>>$outfile 2>>$errfile
	
	idl -e ".r make_bird_density_profiles.pro" 1>>$outfile 2>>$errfile
	
	echo `date`": Started processing based on "$answer2"...Done" | tee 1>>$outfile

fi


