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
id='FMI_PPI_20110*'
#move and unpack to:
destination='/nobackup_1/users/graafdem/DATA/FLYSAFE2/odim/FI_'
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

  name=${file%.tar.gz}		#strip file extention
  date=${file:8:8}		#get only the date indication, use for path
  
  echo `date`": Unpacking $date"
  tar xfz $file
  
  #FiND the different system files and redistribute them
  cd 'HDF5'
  
  allfiles=`ls -1 *.h5`  

  for ifile in $allfiles
  do 

    let i=i+1

    datetime=${ifile:0:12}
    filescancode=${ifile:13:10}
    code=${ifile:13:3}				#get the station code
    system=`echo $code | tr '[A-Z]' '[a-z]'`	#convert to lower case
    
    thisdestination=$destination$system'/'$date
    
    destinationfile=$filescancode'_'$datetime'.h5'
    
    mkdir -p $thisdestination
#    mv $ifile  $thisdestination

    echo -en '\r' `date`": Repacking $ifile to $thisdestination"/"$destinationfile"
    h5repack -f GZIP=9 $ifile  $thisdestination'/'$destinationfile

  #
#  if [ $i == 5 ]
#  then 
#    exit 0
#  fi  

    
  done   

  cd $thisdir  
  rm -fr 'HDF5'
   
done 

echo -en '\r' `date`": Moved $i files of id $id to $destination"'xxx'
echo

exit 0
