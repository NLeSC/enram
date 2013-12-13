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

;SET the radar ids of the radars to be processed
radar_ids = READSTATIONLIST()
radar_ids = radar_names(radar_ids)

; SPECIFY the time interval to process.
date='20110815'
time='0000'


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


find_volume_files,files,date,time,radar_ids

IF KEYWORD_SET(definitions) THEN dummy=TEMPORARY(definitions)
FOR id=0,N_ELEMENTS(radar_ids)-1 DO BEGIN
  radar_definitions,radar_ids[id],radar_definition
  definitions= KEYWORD_SET(definitions) ? [definitions,radar_definition] : radar_definition
ENDFOR

scan=1
grid=1

plot_lon=0
plot_lat=45
limit=[40,-15,70,40]


; Variable 'psfile' needs to be undefined for map_ppi to work properly. I know! Totally weird.

map_ppi,files,limit,$
definitions=definitions,$
coast=0,$
bg=1,$
plot_lon=plot_lon,$
plot_lat=plot_lat,$
scan=scan,$
grid=grid,$
psfile=psfile,$
radar_cross=20

END
