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




#A bunch of 'loose' files have been packed into 2011081800_IE.zip
#
#All files are scattered over the zip files, with no structure whatsoever 
#to the date. So unpack EVERYTHING, and then sort and redistribute from
#there. Remove unpacked files after complete run, and leave zip files
#
#WHAT files?
id='*EIDB_20110*'

#move and unpack to:
destination='/nobackup_1/users/graafdem/DATA/FLYSAFE2/odim/IE_'
#REMOVE original file?
#delete=1
#
#UNZIP? or skip this step
#zip=1
#
unzip_directory='IE_hdf5' 
#
#

#UNZIP all zip 

if [ $zip ] 
then 
 echo 'Unzipping all'
 mkdir -p $unzip_directory

 files=`ls -1 *.zip` 
 for file in $files
 do
   echo $file
   unzip -qq $file
   mv *.h5 $unzip_directory
 done
fi


#REDISTRIBUTE all

thisdir=`pwd`
cd $unzip_directory
files=`ls -1 $id*` 			#find files
number_files=`ls -1 $id* | wc -l`	#find number of files

echo `date`": Redistributing and repacking $number_files files to $thisdestination"

#initialise counter
i=`expr 0`
#
#LOOP through all the unpacked files
#
for file in $files
do

  let i=i+1			#count
  
  name=${file%.tar.gz}		#strip file extention
  date=${file:16:8}		#get the date, use for path
  code=${file:6:2}		#get the id 
  
  if [ $code == 40 ] 		#assign path based on id number
  then 
    path_id='sha'
  elif [ $code == 41 ]
  then 
    path_id='dub' 
  else 
    echo
    echo 'wrong id encountered, please check'
    exit 0
  fi  
  #
  #Set the destination
  #
  thisdestination=$destination$path_id'/'$date
  #
  echo -en '\r' `date`": Repacking $file to $thisdestination"
  #

# if [ $donotdothisyet ]
# then
  #
  #Copy and repack
  #  
  mkdir -p $thisdestination		#Make sure destination dir exists
  #
  echo -en '\r' `date`": Repacking $file to $thisdestination"
  h5repack -f GZIP=9 $file  $thisdestination'/'$file
  #
#  if [ $i == 5 ]
#  then 
#    exit 0
#  fi  
    

# fi
   
done 
#
#REMOVE the unpacked files
cd $thisdir 
rm -fr $unzip_directory
#
#EXIT
echo -en '\r' `date`": Moved $i files of id $id to $destination"'xxx'
echo

exit 0
