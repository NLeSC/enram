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

RESOLVE_ROUTINE, 'common_definition', /compile_full_file
common_definition
;
COMMON constants,$
  NSCANX,RADIUS43,RADIUS,HLAYER,NLAYER,NDATA,RANGMIN,RANGMINSTDEV,    $
  RANGMAXSTDEV,RANGMAX,AZIMMIN,AZIMMAX,VRADMIN,NGAPBIN,NGAPMIN,   $
  NDBZMIN,VDIFMAX,VMASKMAX,EMASKMAX,RHOMIN,ZDRMIN,DBZMIN,         $
  DBZMAX,DBZNOISE,DBZRAIN,DBZCELL,STDEVCELL,AREACELL,CLUTPERCCELL,$
  NEIGHBOURS,VTEXSCALE,VTEXOFFSET,STDEVSCALE,NTEXBINAZIM,         $
  NTEXBINRANG,NTEXMIN,TEXCV,TEXSTDEV,DBZCLUTTER,DBZFACTOR,        $
  SIGMABIRD,STDEVBIRD,XOFFSET,XSCALE,XMEAN


;PLOT multiple radars' ppi's

;SET the radar ids of the radars to be processed
radar_ids = READSTATIONLIST()
radar_ids = radar_names(radar_ids)

; SPECIFY the time interval to process.
dates='20110815'

scan=1
grid=1

plot_lon=0
plot_lat=45
limit=[41,-15,71,40]

fringe=0
rain=1

minutes=STRING(FORMAT='(i0,"0")',INDGEN(6))
hours=STRING(FORMAT='(i02)',INDGEN(8))

FOR ihour=0,N_ELEMENTS(hours)-1 DO BEGIN

  FOR iminute=0,N_ELEMENTS(minutes)-1 DO BEGIN

    date=dates[0]
    time=hours[ihour]+minutes[iminute]

    thething = KEYWORD_SET(rain) ? 'rain' : 'bird'
    print, FORMAT='("Plotting ",a," ppi on ",a,":",a)',thething,date,time

    find_files,files,date,time,radar_ids;,directory=directory

    IF KEYWORD_SET(definitions) THEN dummy=TEMPORARY(definitions)
    FOR id=0,N_ELEMENTS(radar_ids)-1 DO BEGIN
      radar_definitions,radar_ids[id],radar_definition
      definitions= KEYWORD_SET(definitions) ? [definitions,radar_definition] : radar_definition
    ENDFOR

    psfile = thething+'_ppi_'+date+time
    giffile=psfile+'.gif'
    psfile+='.ps'

    map_bird_ppi,files,limit,$
    date=date,time=time,$
    definitions=definitions,$
    fringe=fringe,$
    rain=rain,$
    coast=0,$
    bg=1,$
    plot_lon=plot_lon,$
    plot_lat=plot_lat,$
    grid=grid,$
    psfile=psfile,$
    radar_cross=0,$
    radar_title=0

    PRINT
    h5_close

    SPAWN, 'convert '+psfile+' '+giffile
    SPAWN, 'rm -f '+psfile

  ENDFOR
ENDFOR

END
