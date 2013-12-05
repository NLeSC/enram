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
