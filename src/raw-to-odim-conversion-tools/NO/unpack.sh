#!/bin/bash

#OPEN norway tgz files and redistribute rainbow file to rainbow directoty. 
#no conversion yet.
#
#WHAT files?
id='*.tgz'

#move and unpack to:
destination='/nobackup_1/users/graafdem/DATA/FLYSAFE2/rainbow/NO/'
#REMOVE original file?
#delete=1
#
#UNZIP? or skip this step
#zip=1
#
unzip_directory='rainbow5' 
#
#

#UNZIP all tgz

if [ $zip ] 
then 
 echo 'Unzipping all'
 mkdir -p $unzip_directory

 files=`ls -1 $id` 
 for file in $files
 do
   echo $file
   tar xfz $file
 done
fi


#REDISTRIBUTE all

thisdir=`pwd`
#cd $unzip_directory
files=`find ./ -name 2011*.vol` 		#find rainbow (vol) files
number_files=`find ./ -name 2011*.vol | wc -l`	#find number of files

echo `date`": Redistributing and repacking $number_files files to $thisdestination"


#initialise counter
i=`expr 0`
#
#LOOP through all the unpacked files
#
for file in $files
do

  let i=i+1			#count
  name=${file##*/}		#get the date, use for path
  date=${name:0:8}		#get the date, use for path
  code=${file:11:3}
  
  path_id=$code

  #Set the destination
  #
  thisdestination=$destination$path_id'/'$date
  #
  echo -en '\r' `date`": Moving $name to $thisdestination"
  #
# if [ $donotdothisyet ]
# then
  #
  #
  #Copy and repack
  #  
  mkdir -p $thisdestination		#Make sure destination dir exists
  #
  mv $file  $thisdestination'/'$name
  #
#  if [ $i == 5 ]
#  then 
#    exit 0
#  fi  
 
# fi
   
done
echo
 
#
#REMOVE the unpacked files
#cd $thisdir 
rm -fr $unzip_directory
#
#EXIT
echo -en '\r' `date`": Moved $i files of id $id to $destination"'xxx'
echo

exit 0
