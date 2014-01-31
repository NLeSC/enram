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

;PLOT multiple radars' ppi's

; add the global variables
RESOLVE_ROUTINE, 'common_definition', /compile_full_file
common_definition
COMMON constants,$
  NSCANX,RADIUS43,RADIUS,HLAYER,NLAYER,NDATA,RANGMIN,RANGMINSTDEV,    $
  RANGMAXSTDEV,RANGMAX,AZIMMIN,AZIMMAX,VRADMIN,NGAPBIN,NGAPMIN,   $
  NDBZMIN,VDIFMAX,VMASKMAX,EMASKMAX,RHOMIN,ZDRMIN,DBZMIN,         $
  DBZMAX,DBZNOISE,DBZRAIN,DBZCELL,STDEVCELL,AREACELL,CLUTPERCCELL,$
  NEIGHBOURS,VTEXSCALE,VTEXOFFSET,STDEVSCALE,NTEXBINAZIM,         $
  NTEXBINRANG,NTEXMIN,TEXCV,TEXSTDEV,DBZCLUTTER,DBZFACTOR,        $
  SIGMABIRD,STDEVBIRD,XOFFSET,XSCALE,XMEAN
  
  
  
  
FOR iMap = 0,2 DO BEGIN  

  IF iMap eq 0 THEN BEGIN 
    PRINT,'making colored-donuts map of central europe'
    radar_ids = ['NO','PL','SK','FR']
    limit = [47,10,56,28]
    domainStr = 'map_centraleu_'
  ENDIF

  IF iMap eq 1 THEN BEGIN
    PRINT,'making colored-donuts map of scandinavia'
    radar_ids = ['NO','SE','FI','PL']
    limit = [52,0,72,36]
    domainStr = 'map_scandinavia_'
  ENDIF


  IF iMap eq 2 THEN BEGIN
    PRINT,'making colored-donuts map of southwest europe'
    radar_ids = ['NL','PL','SK','FR']
    limit = [42,-4,54,15]
    domainStr = 'map_sweu_'
  ENDIF
    
  radar_ids = radar_names(radar_ids)
  
  subdir = '0'
  directory = GETENV('IDL_ENRAM_CLUTTER_SENSITIVITY') + subdir + '/'
  
  ; SPECIFY the time interval to process.
  date='20110815'
  time='0000'
  
  find_files,files,date,time,radar_ids,directory=directory
  IF N_ELEMENTS(files) eq 0 THEN BEGIN
    PRINT, "I can''t plot anything because I don''t see any files corresponding to the given datetime"
    CONTINUE
  ENDIF

  IF KEYWORD_SET(definitions) THEN dummy=TEMPORARY(definitions)
  FOR id=0,N_ELEMENTS(radar_ids)-1 DO BEGIN
    radar_definitions,radar_ids[id],radar_definition
    definitions= KEYWORD_SET(definitions) ? [definitions,radar_definition] : radar_definition
  ENDFOR
  
  psfile = GETENV('IDL_ENRAM_VISUALIZATION_OUTPUT')+domainStr+date+time+'_'+subdir+'.ps'
  
  scan=1
  grid=1
  
  plot_lon=0
  plot_lat=45

  
  map_donut,files,limit,$
    date=date,time=time,$
    definitions=definitions,$
    coast=0,$
    bg=1,$
    plot_lon=plot_lon,$
    plot_lat=plot_lat,$
    grid=grid,$
    psfile=psfile,$
    radar_cross=0,$
    radar_title=2
    

ENDFOR

END
