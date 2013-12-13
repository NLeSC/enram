;
; Copyright 2013 Netherlands eScience Center
;
; Licensed under the Apache License, Version 2.0 (the "License");
; you may not use this file except in compliance with the License.
; You may obtain a copy of the License at
;
; http://www.apache.org/licenses/LICENSE-2.0
;
; Unless required by applicable law or agreed to in writing, software
; distributed under the License is distributed on an "AS IS" BASIS,
; WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
; See the License for the specific language governing permissions and
; limitations under the License.
;
FUNCTION READSTATIONLIST

file = GETENV('IDL_ENRAM_DATA') + 'stations.txt'
OPENR, lun, file, /GET_LUN
; Read one line at a time, saving the result into array
radar_ids = []
line = ''
WHILE NOT EOF(lun) DO BEGIN & $
  READF, lun, line & $
  radar_ids = [radar_ids, line] & $
ENDWHILE
; Close the file and free the file unit
FREE_LUN, lun

RETURN, radar_ids

END
