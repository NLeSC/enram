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



#Czech Republic 
#RUN for id='T_PAHZ60' and destination=CZ_brd
#AND     id='T_PAHZ50' and destination=CZ_ska

# script requires that ENRAM_* environment variables have been set
: ${ENRAM_RAW_DATA:?"script requires ENRAM_RAW_DATA to have been set."}
: ${ENRAM_CONVERTED_DATA:?"script requires ENRAM_CONVERTED_DATA to have been set."}
: ${ENRAM_TMP:?"script requires ENRAM_TMP to have been set."}

# set source and destination
source=$ENRAM_RAW_DATA'CZ/czech*'
destination=$ENRAM_CONVERTED_DATA'CZ_'

files=`ls -1 $source`
 
thisdir=`pwd`

echo $thisdir


#initialize counter
i=`expr 0`
for file in $files
do

  name=${file%.tar.gz}      #strip file extension
  date=${name:6}            #get only the date indication, use for path
  $stationname
  echo -en '\r' `date`": Unpacking $date"
  #
  tar --directory $ENRAM_TMP -xzf $file
  #
  #Find the different system files and redistribute them
  cd 'hdf5/'$date
  #
  allfiles=`ls -1 $source*.hdf`
  number_of_files=`ls -1 $source*.hdf | wc -l`
  #
  #PROCESS all files
  for ifile in $allfiles
  do 
    #
    name=${ifile%.hdf}
    stationcode=${name:6:2} 
    #
    #REDISTRIBUTE according to station name
    if [ $stationcode == 60 ] 
    then
      code='ska'
    fi
    if [ $stationcode == 50 ] 
    then
      code='brd'
    fi
    #
    #create destination directory name
    thisdestination=$destination$code"/"$date
    #
    mkdir -p $thisdestination
    mv $ifile  $thisdestination
    #
    let i=i+1
  done   
  #
  cd $thisdir
  #
done 

#remove unpack directory
rm -fr ${ENRAM_TMP}'hdf5'

echo -en '\r' `date`": Moved $i files of id $id to $destination"
echo

exit 0
