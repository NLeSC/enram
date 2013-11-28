#!/bin/bash

#Czech Republic 
#RUN for id='T_PAHZ60' and destination=CZ_brd
#AND     id='T_PAHZ50' and destination=CZ_ska


#move and unpack to:
destination='/nobackup_1/users/graafdem/DATA/FLYSAFE2/odim/CZ_'
#REMOVE original file?
#delete=1

#
#
#

files=`ls -1 czech*` 
thisdir=`pwd`

echo $thisdir


#initialise counter
i=`expr 0`
for file in $files
do

  name=${file%.tar.gz}		#strip file extention
  date=${name:6}		#get only the date indication, use for path
  $stationname
  echo -en '\r' `date`": Unpacking $date"
  #
  tar xfz $file
  #
  #FiND the different system files and redistribute them
  cd 'hdf5/'$date
  
  allfiles=`ls -1 *.hdf`
  number_of_files=`ls -1 *.hdf | wc -l`
  #
  #PROCESS all files
  for ifile in $allfiles
  do 
    #
    name=${ifile%.hdf}
    stationcode=${name:6:2}	
    #
    #REDISTRIBUTE according to station name
    if [ $stationcode == 60 ] 
    then
      code='ska'
    fi
    if [ $stationcode == 50 ] 
    then
      code='brd'
    fi
    #
    #create destination directory name
    thisdestination=$destination$code"/"$date
    #
    mkdir -p $thisdestination
    mv $ifile  $thisdestination
    #
    let i=i+1
  #
  #if [ $i == 350 ]
  #then 
  # exit 0
  #fi  
  #
  done   
  #
  #
  cd $thisdir
  # 
#  if [ $delete ] 
#  then 
#    echo -en '\r' `date`": Deleting $file"
#    rm -f $file
#  fi
done 
#
#remove unpack directory
rm -fr 'hdf5'

echo -en '\r' `date`": Moved $i files of id $id to $destination"
echo

exit 0
