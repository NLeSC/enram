FUNCTION READSTATIONLIST

file = GETENV('IDL_ENRAM_DATA') + 'stations.txt'
OPENR, lun, file, /GET_LUN
; Read one line at a time, saving the result into array
radar_ids = ''
line = ''
WHILE NOT EOF(lun) DO BEGIN & $
  READF, lun, line & $
  radar_ids = [radar_ids, line] & $
ENDWHILE
; Close the file and free the file unit
FREE_LUN, lun

RETURN, radar_ids

END