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
