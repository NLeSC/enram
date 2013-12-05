#!/bin/bash

#WHAT files?
id='si1*'
#move and unpack to:
destination='/nobackup/users/graafdem/DATA/RADAR/FLYSAFE2/odim/SI_lis/'
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
  let i=i+1
  
  name=${file%.zip}		#strip file extention
  date=${name:4:8}		#get only the date indication, use for path
  
  echo -en '\r' `date`": Unpacking $date"
  thisdest=$destination$date
  
  mkdir -p $thisdest

  cp $file $thisdest
  cd $thisdest
  unzip -qqo $file
  rm -f $file
  
  cd $thisdir
   
done 

echo -en '\r' `date`": Moved $i files of id $id to $destination"

exit 0
