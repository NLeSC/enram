#!/bin/bash

#WHAT files?
id='Sweden'
#move and unpack to:
unpack='/nobackup/users/graafdem/DATA/RADAR/FLYSAFE2/raw/SE/tmp/'
destination='/nobackup/users/graafdem/DATA/RADAR/FLYSAFE2/odim/SE_'
#REMOVE original file?
#delete=1

#
#
#

files=`ls -1 $id*` 

thisdir=`pwd`

#initialise counter
i=`expr 0`
for file in $files
do

  ifile=$i
  
  name=${file%.tar.gz}		#strip file extention
  date=${name:7}		#get only the date indication, use for path
  
  echo -en '\r' `date`": Unpacking $date"

  mkdir -p $unpack
  cp $file $unpack
  cd $unpack
  tar xfz $file

  #FiND the different system files and redistribute them
  cd $unpack$date
  
  allfiles=`ls -1 *.h5` 

  for ifile in $allfiles
  do 
    system=${ifile:2:3}
    
    thisdestination=$destination$system'/'$date
#    echo $thisdestination
    mkdir -p $thisdestination
    mv $ifile  $thisdestination
  done   
  
  cd $unpack
  rm -fr $date
  rm -f $file
  
  cd $thisdir
  rmdir $unpack
   
#  if [ $delete ] 
#  then 
#    echo -en '\r' `date`": Deleting $file"
#    rm -f $file
#  fi
  
  let i=i+1
done 

echo -en '\r' `date`": Moved $i files of id $id to $destination"'xxx'
echo

exit 0
