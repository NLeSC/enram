#!/bin/bash
#
#Move all untared and gunziped files to a temporary directory, 
#with matching times. 
output_directory='/usr/people/graafdem/FLYSAFE/process/data/odim/'


files=`find mf_binary/ -name *_2011*.*` 		#find h5 (vol) files
number_files=`find mf_binary/ -name *_2011*.* | wc -l`	#find number of files
#
echo `date`": Converting $number_files mf binary files to hdf5"
#

i=`expr 0`
#
radars=`ls -1 mf_binary/`
#
#find the radar name
for radar in $radars
do
  #
  #DEFINE radar ids for radars
  if [ $radar == 'Abbeville' ]
  then
    radarid='T_PAGZ40_C_frabb'
  fi 
  if [ $radar == 'Aleria' ]
  then
    radarid='T_PAGZ61_C_frale'
  fi 
  if [ $radar == 'Arcis' ]
  then
    radarid='T_PAGZ52_C_frtro'
  fi 
  if [ $radar == 'Avesnes' ]
  then
    radarid='T_PAGZ63_C_frave'
  fi 
  if [ $radar == 'Blaisy' ]
  then
    radarid='T_PAGZ65_C_frbla'
  fi 
  if [ $radar == 'Bollene' ]
  then
    radarid='T_PAGZ55_C_frbol'
  fi 
  if [ $radar == 'Bordeaux' ]
  then
    radarid='T_PAGZ41_C_frbor'
  fi 
  if [ $radar == 'Bourges' ]
  then
    radarid='T_PAGZ42_C_frbou'
  fi 
  if [ $radar == 'Cherves' ]
  then
    radarid='T_PAGZ64_C_frche'
  fi 
  if [ $radar == 'Collobrieres' ]
  then
    radarid='T_PAGZ59_C_frcol'
  fi 
  if [ $radar == 'Falaise' ]
  then
    radarid='T_PAGZ45_C_frcae'
  fi 
  if [ $radar == 'Grezes' ]
  then
    radarid='T_PAGZ44_C_frgre'
  fi 
  if [ $radar == 'Momuy' ]
  then
    radarid='T_PAGZ66_C_frmom'
  fi 
  if [ $radar == 'Montancy' ]
  then
    radarid='T_PAGZ67_C_frmtc'
  fi 
  if [ $radar == 'Montclar' ]
  then
    radarid='T_PAGZ62_C_frmcl'
  fi 
  if [ $radar == 'Nancy' ]
  then
    radarid='T_PAGZ47_C_frnan'
  fi 
  if [ $radar == 'Nimes' ]
  then
    radarid='T_PAGZ49_C_frnim'
  fi 
  #
#  radarfiles=`find mf_binary/$radar/ -name *_2011*.*`
#  number_files=`find mf_binary/$radar/ -name *_2011*.* | wc -l`	#find number of files
  #
  #find the cdates
  dates=`ls -1 mf_binary/$radar/`
  #
  for date in $dates
  do
    #
    #find the hour
    hhs=`ls -1 mf_binary/$radar/$date/`
    #
    for hh in $hhs 
    do
      #find the minutes
      mms=`ls -1 mf_binary/$radar/$date/$hh/`
      #
      for mm in $mms 
      do 
	#
	#convert all the files in this directory to one hdf5 file
	files=`ls -1 "mf_binary/$radar/$date/$hh/$mm/"`
	#
	#Construct code from radarid
	docode=`echo $radarid | tr '[A-Z]' '[a-z]'`	#convert to lower case
	upcode=`echo $radarid | tr '[a-z]' '[A-Z]'`	#convert to upper case
	#
	code=${upcode:11:2}$'_'${docode:13:3}
	#
	#CREATE file and directory names for these files
	template='templates/'$radarid'.h5'
	h5file=$output_directory$code'/'$date/$radarid'_'$date$hh$mm'.h5'
	current_directory=mf_binary/$radar/$date/$hh/$mm
	
	binary_files=`find $current_directory -name *_2011*.*`
	#
	#CONVERT and move
        mkdir -p "$output_directory$code/$date/"
        MF_binary_to_ODIM_vol_h5 $template $h5file $binary_files
	echo -en '\r'`date`": Creating hdf5 file ${h5file##*/} in $code/$date"
	#
        let i=i+1
        #
	#  if [ $i == 5 ]
	#  then 
	#    exit 0
	#  fi  

      done
    done
  done
done
echo
echo "Created $i hdf files out of $number_files binary files"
exit 0
