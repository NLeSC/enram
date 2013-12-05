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



#
#Move all french untared and gunziped binary files to a temporary directory, 
#with matching times. 
#
#
id='*.tar'
#
#

files=`find ./ -name *_2011*.*` 		#find h5 (vol) files
number_files=`find ./ -name *_2011*.* | wc -l`	#find number of files

echo `date`": Moving $number_files mf binary files to a mf_binary/"

thisdir=`pwd`

#initialise counter
i=`expr 0`
j=`expr 0`
for file in $files
do

  name=${file##*/}		#get the file basename
  date=${file:2:8}		#get only the date indication, use for path

  let i=i+1

   #Get the station name and the time stamp of each file
  if [ ${name:0:9} == 'Abbeville' ]
  then 
       time=${name:18:4}
       radarname='Abbeville'
  fi
  if [ ${name:0:6} == 'Aleria' ]
  then 
       time=${name:15:4}
       radarname='Aleria'
  fi
  if [ ${name:0:5} == 'Arcis' ]
  then 
       time=${name:14:4}
       radarname='Arcis'
  fi
  if [ ${name:0:7} == 'Avesnes' ]
  then 
       time=${name:16:4}
       radarname='Avesnes'
  fi
  if [ ${name:0:6} == 'Blaisy' ]
  then 
       time=${name:15:4}
       radarname='Blaisy'
  fi
  if [ ${name:0:7} == 'Bollene' ]
  then 
       time=${name:16:4}
       radarname='Bollene'
  fi
  if [ ${name:0:8} == 'Bordeaux' ]
  then 
       time=${name:17:4}
       radarname='Bordeaux'
  fi
  if [ ${name:0:7} == 'Bourges' ]
  then 
       time=${name:16:4}
       radarname='Bourges'
  fi
  if [ ${name:0:7} == 'Cherves' ]
  then 
       time=${name:16:4}
       radarname='Cherves'
  fi
  if [ ${name:0:12} == 'Collobrieres' ]
  then 
       time=${name:21:4}
       radarname='Collobrieres'
  fi
  if [ ${name:0:7} == 'Falaise' ]
  then 
       time=${name:16:4}
       radarname='Falaise'
  fi
  if [ ${name:0:6} == 'Grezes' ]
  then 
       time=${name:15:4}
       radarname='Grezes'
  fi
  if [ ${name:0:5} == 'Momuy' ]
  then 
       time=${name:14:4}
       radarname='Momuy'
  fi
  if [ ${name:0:8} == 'Montancy' ]
  then 
       time=${name:17:4}
       radarname='Montancy'
  fi
  if [ ${name:0:8} == 'Montclar' ]
  then 
       time=${name:17:4}
       radarname='Montclar'
  fi
  if [ ${name:0:5} == 'Nancy' ]
  then 
       time=${name:14:4}
       radarname='Nancy'
  fi
  if [ ${name:0:5} == 'Nimes' ]
  then 
       time=${name:14:4}
       radarname='Nimes'
       let j=j+1
  fi
  #
  #from the time, get the hour and minutes and put each file 
  #in a separate directory according to the time. 
  hh=${time:0:2}
  mm=${time:2:2}
  #
  #Move all files with the same time stamp to the same directory
  temp_dir=$thisdir'/mf_binary/'$radarname'/'$date$'/'$hh'/'$mm'/'
  mkdir -p $temp_dir
  mv $file $temp_dir
  #
  echo -en '\r'`date`": Moving $name to /mf_binary/$radarname/$date$/$hh/$mm/"
  #
done 

echo
echo -en '\r'`date`": Moved $i files of to mf_binary"
echo

exit 0
