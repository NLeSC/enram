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

;
;FIND_FILES.PRO
;
;FIND radar bird profile files for a specific date and time, and specific radars

PRO find_files,files,$
date,time,radar_ids,  $
directory=directory

dummy=TEMPORARY(files)

index = INTARR(N_ELEMENTS(radar_ids))
FOR id=0,N_ELEMENTS(radar_ids)-1 DO BEGIN
  radar_definitions,radar_ids[id],radar_definition

  dir = KEYWORD_SET(directory) ? directory : radar_definition.BIRD_DATA_PATH
  dir += radar_definition.PATH_ID+'/'
  dir += date+'/'
  search_string=dir + '*'+radar_definition.PATH_ID+'*'+date+time+'*'
  file = FILE_SEARCH(search_string,count=n1files)
  
  IF n1files eq 0 THEN BEGIN
    thistime=STRMID(time,0,3)
    search_string=dir + '*'+radar_definition.PATH_ID+'*'+date+thistime+'*'
    file = FILE_SEARCH(search_string,count=n2files)
    IF n2files gt 1 THEN BEGIN
      ifile=FLOOR(n2files/2)
      file=file[ifile]
    ENDIF ELSE BEGIN
      IF n2files eq 0 THEN BEGIN
	thistime=STRMID(time,0,2)
	search_string=dir + '*'+radar_definition.PATH_ID+'*'+date+thistime+'*'
	file = FILE_SEARCH(search_string,count=n3files)
	IF n3files gt 1 THEN BEGIN
	  ifile=FLOOR(n3files/2)
	  file=file[ifile]
	ENDIF
      ENDIF	
    ENDELSE 
  ENDIF
  
  IF n1files gt 1 THEN BEGIN
    ifile=FLOOR(n1files/2)
    file=file[ifile]
  ENDIF 
  
  IF file ne '' THEN BEGIN 
   files = KEYWORD_SET(files) ? [files,file] : file 
   index[id]=1
  ENDIF ELSE print, 'No file found for '+radar_definition.RADAR_FULL_NAME+' on '+date +':'+time

ENDFOR

radar_ids = radar_ids[WHERE(index eq 1)]
;
END
