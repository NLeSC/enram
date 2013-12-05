#!/bin/bash

#WHAT files?
id='SK_KH'
#move and unpack to:
destination='/nobackup/users/graafdem/DATA/RADAR/FLYSAFE2/odim/SK_koh/'
#REMOVE original file?
delete=1

#
#
#

files=`ls -1 $id*` 

thisdir=`pwd`
mkdir -p $destination

#initialise counter
i=`expr 0`
for file in $files
do
  ifile=$i
  
  name=${file%.tar.gz}		#strip file extention
  date=${name:6}		#get only the date indication, use for path
  
  echo -en '\r' `date`": Unpacking $date"
  thisdest=$destination$date
  mkdir -p $thisdest

  cp $file $thisdest
  cd $thisdest
  tar xfz $file
  rm -f $file
  
  cd $thisdir
  
  if [ $delete ] 
  then 
    echo -en '\r' `date`": Deleting $file"
  fi
  
  let i=i+1
done 

echo -en '\r' `date`": Moved $i files of id $id to $destination"

exit 0
