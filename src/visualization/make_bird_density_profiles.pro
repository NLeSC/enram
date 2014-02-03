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



;SET the radar ids of the radars to be processed
radar_ids = READSTATIONLIST()
radar_ids = radar_names(radar_ids)

; SPECIFY the time interval to process.
date='20110815'

;GET the radar definitions from the definitions structure
IF KEYWORD_SET(definitions) THEN dumy=TEMPORARY(definitions)
FOR i=0,N_ELEMENTS(radar_ids)-1 DO BEGIN
  radar_definitions,radar_ids[i],radar_definition
  definitions= KEYWORD_SET(definitions) ? [definitions,radar_definition] : radar_definition
ENDFOR

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

PRINT,'============================================================================
PRINT,'                   PLOT PROFILES
PRINT,'                 '+SYSTIME(0)
PRINT,'============================================================================
; Loop over the requested radar systems

YYYY=STRMID(date,0,4)
MM=STRMID(date,4,2)
DD=STRMID(date,6,2)

months=['Jan','Feb','Mar','Apr','May','Jun','Jul','Aug','Sep','Oct','Nov','Dec']
themonth = months[FIX(MM)-1]

startdate=YYYY+'-'+themonth+'-'+DD
enddate=YYYY+'-'+themonth+'-'+DD

print, startdate+' - '+ enddate

FOR iradar=0,N_ELEMENTS(radar_ids)-1 DO BEGIN
  ;
  this_rd=definitions[iradar]
  countryplace=STRSPLIT(this_rd.RADAR_FULL_NAME,'_',/EXTRACT)
  thiscountry=countryplace[0]
  thisplace=countryplace[1]

  ;FIND the ascii *DAT file
  search_string=this_rd.BIRD_DATA_PATH+this_rd.path_id+'/'+this_rd.path_id+'*'+DD+MM+YYYY+'*.DAT'
  ascii_data=FILE_SEARCH(search_string,count=ascii_count)
  IF ascii_count eq 0 THEN BEGIN
    PRINT, search_string+' not found'
    CONTINUE
  ENDIF ELSE ascii_data=ascii_data[0]

  psfile = GETENV('IDL_ENRAM_VISUALIZATION_OUTPUT') + this_rd.path_id+'_'+date+'_profile.ps'

  bangp=!p
  loadct,5,/silent
  !P.Multi=0
  !P.NOERASE=1
  !P.FONT=0
  !P.CHARSIZE=0.8

  thisDevice = !D.Name
  Set_Plot, 'PS'
  DEVICE, BITS_PER_PIXEL=8, COLOR=1,filename=psfile,$
  xsize=0.75*20,ysize=0.75*13*sqrt(2)*2/3

  ;define time axis format
  DateRange=date2jd([startdate, enddate])
  Daterange=[Daterange[0]-0.5D,dateRange[1]+0.5D]
  dummy=LABEL_DATE(DATE_FORMAT=['%D/%M'])

  message=' Plotting Bird Profile for '
  message+=thisplace+', '+thiscountry
  data=readvp2(ascii_data)

  title=STRJOIN([TeXtoIDL('Bird Density Height Profile'),''])
  bartitle=TeXtoIDL('Reflectivity (log_{10})')
  birdticks=1
  bar=1

  TRESH_PRECIP=1
  YRNG=[0,100]
  TRESH_STDEV=1.85
  ;factor for conversion dBZ to reflectivity
  LAYERHEIGHT=0.2   ;height layer spacing in km

  plotprof_day, data.juldate,data.height,data.dbz/10+ALOG10(DBZFACTOR),$
  data.u,data.v,$
  TITLE=title,$
  /windbarb, NVectors=[40,10],$
  BARTITLE=bartitle,$
  YTICKLEN=1,XTICKLEN=1,$
  CONTOURRANGE=[0.8,3.8],   $
  BIRDTICKS=birdticks,BAR=bar,$
  XRANGE=DateRange,XSTYLE=1,XTITLE=xtitle

  XYOUTS,/NORMAL,0.26,0.9,thisplace+', '+thiscountry,CHARTHICK=10,CHARSIZE=1.2,color=getcolor('black',0)

  PRINT, FORMAT='(a,a)', systime(0),message

Device, /Close
Set_Plot, thisDevice
!P=bangp

ENDFOR

END
