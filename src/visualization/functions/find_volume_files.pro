;
;FIND_VOLUME_FILES.PRO
;
;FIND radar volume files for a specific date and time, and specific radars

PRO find_volume_files,files,$
date,time,radar_ids,  $
directory=directory

dummy=TEMPORARY(files)
FOR id=0,N_ELEMENTS(radar_ids)-1 DO BEGIN
  radar_definitions,radar_ids[id], radar_definition

  dir = KEYWORD_SET(directory) ? directory : radar_definition.INPUT_DATA_PATH
  
  dir += radar_definition.PATH_ID+'/'
  dir += date+'/'
  search_string=dir + '*'+radar_definition.RADAR_ID+'*'+date+time+'*'
  ;Exception for swedish file names
  IF STRUPCASE(STRMID(radar_definition.RADAR_FULL_NAME,0,6)) eq 'SWEDEN' THEN BEGIN
    search_string=dir + '*'+radar_definition.RADAR_ID+'*'+date+'T'+time+'*'
  ENDIF
  file = FILE_SEARCH(search_string,count=nfiles)
  
  IF nfiles eq 0 THEN BEGIN
    thistime=STRMID(time,0,3)
    search_string=dir + '*'+radar_definition.RADAR_ID+'*'+date+thistime+'*'
    IF STRUPCASE(STRMID(radar_definition.RADAR_FULL_NAME,0,6)) eq 'SWEDEN' THEN BEGIN
      search_string=dir + '*'+radar_definition.RADAR_ID+'*'+date+'T'+time+'*'
    ENDIF
    morefiles = FILE_SEARCH(search_string,count=ofiles)
    
    IF ofiles gt 1 THEN BEGIN
      ifile=FLOOR(ofiles/2)
      file=morefiles[ifile]
    ENDIF ELSE BEGIN
      IF ofiles eq 0 THEN BEGIN
	thistime=STRMID(time,0,2)
	search_string=dir + '*'+radar_definition.RADAR_ID+'*'+date+thistime+'*'
	IF STRUPCASE(STRMID(radar_definition.RADAR_FULL_NAME,0,6)) eq 'SWEDEN' THEN BEGIN
	  search_string=dir + '*'+radar_definition.RADAR_ID+'*'+date+'T'+time+'*'
	ENDIF
	morefiles = FILE_SEARCH(search_string,count=mfiles)
	IF mfiles gt 1 THEN BEGIN
	  ifile=FLOOR(mfiles/2)
	  file=morefiles[ifile]
	ENDIF
      ENDIF
      IF ofiles eq 1 THEN file=morefiles	
    ENDELSE
  ENDIF
  
  IF nfiles gt 1 THEN BEGIN
    ifile=FLOOR(nfiles/2)
    file=file[ifile]
  ENDIF 
  
  IF file eq '' THEN BEGIN 
    PRINT, 'No files found for '+radar_definition.RADAR_FULL_NAME+' on '+date+' at '+time
  ENDIF ELSE BEGIN 
    files = KEYWORD_SET(files) ? [files,file] : file
  ENDELSE

ENDFOR
;
END
