#!/bin/bash

#OPEN dutch tar files and redistribute hdf5 files to odim directory
#NOTE: Dutch HDF5 are NOT ODIM, but will be treated that way.
#
#WHAT files?
id='*.tar'

#move and unpack to:
destination='/nobackup_1/users/graafdem/DATA/FLYSAFE2/odim/NL_'
#REMOVE original file?
#delete=1
#
#UNZIP? or skip this step
#zip=1
#
#

#UNZIP all tar

if [ $zip ] 
then 
 echo 'Untaring all'

 files=`find ./ -name $id` 
 for file in $files
 do
   echo $file
   tar xfv $file
 done
fi

#REDISTRIBUTE all

thisdir=`pwd`
findid="*.h5"
#cd $unzip_directory
files=`find ./ -name $findid` 		#find h5 (vol) files
number_files=`find ./ -name $findid | wc -l`	#find number of files

echo `date`": Redistributing and repacking $number_files files to $destination"

#initialise counter
i=`expr 0`
j=`expr 0`
#
#LOOP through all the unpacked files
#
for file in $files
do

  let i=i+1			#count
  name=${file##*/}		#get the date, use for path
  date=${name:16:8}		#get the date, use for path
  code=${name:6:6}

  if [ $code == '60_VOL' ] 		#assign path based on id number
  then 
    path_id='dbl'
  elif [ $code == '61_VOL' ]
  then 
    path_id='dhl' 
  else 
    path_id='xxx'
  fi
  #
  if [ ! $path_id == 'xxx' ]
  then
  #
  let j=j+1			#count
  #Set the destination
  thisdestination=$destination$path_id'/'$date
  #
  echo -en '\r' `date`": Moving $name to $thisdestination"
  #
# if [ $donotdothisyet ]
# then
  #
  #Copy and repack
  #  
  mkdir -p $thisdestination		#Make sure destination dir exists
  #
  mv $file  $thisdestination'/'$name
# fi
  #

  fi

#  if [ $i == 50 ]
#  then 
#    exit 0
#  fi  


done
echo

#REMOVE DIRECTORIES<
# rm -fr 08 09

#EXIT
echo -en '\r' `date`": Moved $j of $i files to $destination"'xxx'
echo

exit 0
