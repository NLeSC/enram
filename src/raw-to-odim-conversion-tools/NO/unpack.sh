#!/bin/bash


# Copyright 2013 Netherlands eScience Center
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.



# next line needed for ShellEd Bash debugger for Eclipse:
. ../_DEBUG.sh


# script requires that ENRAM_* environment variables have been set
: ${ENRAM_RAW_DATA:?"script requires ENRAM_RAW_DATA to have been set."}
: ${ENRAM_CONVERTED_DATA:?"script requires ENRAM_CONVERTED_DATA to have been set."}
: ${ENRAM_TMP:?"script requires ENRAM_TMP to have been set."}



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
