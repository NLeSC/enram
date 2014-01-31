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

PRO map_prof,files,limit,$
definitions=definitions,$
date=date,time=time,$
plot_lon=plot_lon,$
plot_lat=plot_lat,$
BgColor=BgColor,$
seacolor=seacolor,$
landcolor=landcolor,$
high_resolution=high_resolution,$
coast=coast,$
grid=grid,$
radar_cross=radar_cross,$
radar_title=radar_title,$
_EXTRA=extra,$
psfile=psfile
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;SETTINGS
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;

COMMON constants, NSCANX,RADIUS43,RADIUS,HLAYER,NLAYER,NDATA,RANGMIN,RANGMINSTDEV,    $
  RANGMAXSTDEV,RANGMAX,AZIMMIN,AZIMMAX,VRADMIN,NGAPBIN,NGAPMIN,   $
  NDBZMIN,VDIFMAX,VMASKMAX,EMASKMAX,RHOMIN,ZDRMIN,DBZMIN,         $
  DBZMAX,DBZNOISE,DBZRAIN,DBZCELL,STDEVCELL,AREACELL,CLUTPERCCELL,$
  NEIGHBOURS,VTEXSCALE,VTEXOFFSET,STDEVSCALE,NTEXBINAZIM,         $
  NTEXBINRANG,NTEXMIN,TEXCV,TEXSTDEV,DBZCLUTTER,DBZFACTOR,        $
  SIGMABIRD,STDEVBIRD,XOFFSET,XSCALE,XMEAN


;SET the reflectivity levels
;label=["-33","-29","-25","-21","-17","-13","-9","-5","-1","3","7","11","15","19","23","27","31","35","39","43","47","51","55","59"]
Thresh=[-33.0,-29.0,-25.0,-21.0,-17.0,-13.0,-9.0,-5.0,-1.0,3.0,7.0,11.0,15.0,19.0,23.0,27.0,31.0,35.0,39.0,43.0,47.0,51.0,55.0,59.0]
label =STRING(thresh)
;
;DEFINE the color table for the reflectivity levels
R=[000,000,000,000,000,003,003,003,000,000,054,120,184,247,255,255,199,135,184,245,255,255,255,237]
G=[000,000,000,000,107,237,199,135,184,247,201,135,184,247,201,135,074,010,000,000,000,000,000,013]
B=[000,061,148,242,255,255,145,013,000,000,000,000,000,005,000,000,000,000,000,000,056,122,181,232]
;
;DEFINE colors for nodata and missing
nodata_c=[240,240,240]
missing_c=[255,255,255]
;
seacolor  = N_ELEMENTS(seacolor ) eq 3 ?  seacolor : [224,255,255]  ;default sea color is light cyan
landcolor = N_ELEMENTS(landcolor) eq 3 ? landcolor : [255,239,213]  ;default land color is papaya
;
;COLORBAR settings
cb_Title='Reflectivity [dBz]'
cb_xOffset=0.06     ; colorbar position
cb_yOffset=0.03     ; "
;
;RADAR cross indicator settings
crossdistance = N_ELEMENTS(crossdistance) ne 0 ? crossdistance: 10 ;km
crossthick = N_ELEMENTS(radar_cross) ne 0 ? radar_cross : 2
crosscolor = N_ELEMENTS(crosscolor) eq 3 ? crosscolor : [000,000,000]

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;PARSE the KEYWORDS
caldat,SYSTIME(/JULIAN,/UTC),mm,dd,yy,hh,mi,ss  ;GET current date/time
psfile = N_ELEMENTS(psfile) ne 0 ? psfile : 'map_prof_'+STRING([DD,MM,YY],FORMAT='(I02,I02,I04)')+'.ps'
;
plot_lon = N_ELEMENTS(plot_lon) ne 0 ? plot_lon : 0
plot_lat = N_ELEMENTS(plot_lat) ne 0 ? plot_lat : 0
;
limit = N_ELEMENTS(limit) eq 4 ? limit : [45,0,60,10]
;
scan = N_ELEMENTS(scan) ne 0 ? scan : 1
;
;POSITION KEYWORD for the plot in the plot window
p = N_ELEMENTS(position) eq 4 ? position : [0.04,0.04,0.88,0.88]

high_resolution = N_ELEMENTS(high_resolution) ne 0 ? high_resolution : 1
;
IF N_ELEMENTS(coast) ne 0 THEN BEGIN
  ;
  coast=coast
  ;
ENDIF ELSE BEGIN
  ;
  IF limit[2]-limit[0] gt 30 THEN coast=0 ELSE coast=1
  ;
ENDELSE
;
bgcolor = N_ELEMENTS(bgcolor) ne 0 ? bgcolor : 1
;
ps_xsize = N_ELEMENTS(ps_xsize) ne 0 ? ps_xsize : 20
ps_ysize = N_ELEMENTS(ps_ysize) ne 0 ? ps_ysize : 20
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;COMPUTE the color table: Define a step function to cover the entire 256 colors for the
;colors defined above
nboxes=n_elements(R)
ncolors=!D.TABLE_SIZE
step = ncolors/nboxes
index=INTARR(ncolors)
;
;FILL the 256 colors step wise
j=0
FOR i=0,ncolors-1 DO BEGIN
  if i mod step eq 1 THEN j+=1
  index[i]=j-1
ENDFOR
;
index[WHERE(index gt N_ELEMENTS(r)-2,/NULL)]=N_ELEMENTS(R)-2
Rr=R[index+1]
Gg=G[index+1]
Bb=B[index+1]
;
TVLCT,Rr,Gg,Bb
;
;SET environment variables
bangp=!p
!P.FONT=0
;
;OPEN a postscript file
thisDevice = !D.Name
SET_PLOT,'ps'
a4xsize=21      ;center the image on an a4
a4ysize=29.7
;
DEVICE,/COLOR,BITS=8,FILE=psfile,XSIZE=ps_xsize,ysize=ps_ysize,   $
/Encapsulated, Preview=0,                               $
XOFFSET=(a4xsize-ps_xsize)/2.,YOFFSET=(a4ysize-ps_ysize)/2.,SCALE=1
;
;DEFINE a map projection
MAP_SET,plot_lon,plot_lat,limit=limit,position=p,/noborder,_EXTRA=extra
;
;COLOR the sea and continents with the sea and land colors
;if BgColor is set
IF KEYWORD_SET(BgColor) THEN BEGIN
  ;
  ;LOAD seacolor
  load_color, seacolor,254  ;
  load_color,landcolor,255  ;
  ;
  POLYFILL,/normal,[[p[0],p[1]],[p[0],p[3]],[p[2],p[3]],[p[2],p[1]],[p[0],p[1]]],color=254
  MAP_CONTINENTS,COLOR=255,/FILL,HIRES=high_resolution,COAST=coast
  ;
ENDIF ELSE load_color,missing_c,255  ;

;
;GET ppi images and WARP them to the current map projection
dummy=TEMPORARY(composite)      ;EMPTY composite image
dummy=TEMPORARY(radar_locations)      ;EMPTY composite image
FOR ifile=0,N_ELEMENTS(files)-1 DO BEGIN
  ;
  get_prof,files[ifile],$
  profile_bird,profile_height,$
  height=height,$
  limit=ppilimit,$
  io_file=definitions[ifile].io_file,$
  radar_location=radar_location

  lon=radar_location[0]
  lat=radar_location[1]
  ;
  circle = FINDGEN(37)*10
  mindistance =  5/RADIUS
  maxdistance = 25/RADIUS
  minpolygon = FLTARR(2,N_ELEMENTS(circle))
  maxpolygon = FLTARR(2,N_ELEMENTS(circle))

  FOR i=0,N_ELEMENTS(circle)-1 DO BEGIN
    minpolygon[*,i]=LL_ARC_DISTANCE(radar_location,mindistance,circle[i],/DEGREES)
    maxpolygon[*,i]=LL_ARC_DISTANCE(radar_location,maxdistance,circle[i],/DEGREES)
  ENDFOR
  ;
  TVLCT,R,G,B

;  print, mean(profile_bird)
  mean_bird_profile = MEAN(profile_bird)
  thiscolor = WHERE(thresh gt mean_bird_profile)
  thiscolor = thiscolor[0]-1
;  print, thiscolor

  IF thiscolor ne -1 THEN BEGIN
    polyfill,maxpolygon,color=thiscolor
    polyfill,minpolygon,color=255
  ENDIF

  ;ADD a cross to indicate the radar
  IF KEYWORD_SET(radar_cross) THEN BEGIN

    load_color,crosscolor,0  ;black
    dist=5/RADIUS
    FOR i=0,N_ELEMENTS(files)-1 DO BEGIN
      cr0=LL_ARC_DISTANCE(radar_location,dist,270,/DEGREES)
      cr1=LL_ARC_DISTANCE(radar_location,dist,0,/DEGREES)
      cr2=LL_ARC_DISTANCE(radar_location,dist,90,/DEGREES)
      cr3=LL_ARC_DISTANCE(radar_location,dist,180,/DEGREES)
      PLOTS,/DATA,[radar_location[0],cr0[0]],[radar_location[1],cr0[1]],color=0,thick=crossthick
      PLOTS,/DATA,[radar_location[0],cr1[0]],[radar_location[1],cr1[1]],color=0,thick=crossthick
      PLOTS,/DATA,[radar_location[0],cr2[0]],[radar_location[1],cr2[1]],color=0,thick=crossthick
      PLOTS,/DATA,[radar_location[0],cr3[0]],[radar_location[1],cr3[1]],color=0,thick=crossthick
    ENDFOR

  ENDIF

  IF KEYWORD_SET(radar_title) THEN BEGIN
    radar_code = definitions[ifile].PATH_ID
    radar_name = definitions[ifile].RADAR_FULL_NAME
    IF radar_title eq 1 THEN BEGIN
      radar_id = STRMID(radar_code,0,2)+' '+STRMID(radar_code,3,3)
    ENDIF
    IF radar_title eq 2 THEN BEGIN
      tmp=STRSPLIT(radar_name,'_',/EXTRACT)
      radar_id = TMP[1]
    ENDIF

    dist=27/RADIUS
    azim=45
    tloc=LL_ARC_DISTANCE(radar_location,dist,azim,/DEGREES)
    XYOUTS,/DATA,tloc[0],TLOC[1],radar_id
  ENDIF
  ;
ENDFOR
;
;ADD continents and grid.
;NOTE: all colors are passed through index 0, this should be modified before, and no other index than 0
load_color,[110,110,110],0  ;SET the coast lines: dark grey
MAP_CONTINENTS,COLOR=0,THICK=0.5,COASTS=coast,HIRES=high_resolution
load_color,[200,200,240],0  ;SET the river color: light grey
IF KEYWORD_SET(coast) THEN MAP_CONTINENTS,/RIVERS,COLOR=0,THICK=0.1,HIRES=high_resolution
load_color,[200,200,200],0  ;SET the country lines: light grey
MAP_CONTINENTS,COLOR=0,/COUNTRIES,HIRES=high_resolution
;ADD a rectangle or box-axis grid around the image
load_color,[0,0,0],0  ;black
IF KEYWORD_SET(grid) THEN $
MAP_GRID,/BOX,n=180,CHARTHICK=2 ELSE $
PLOTS,/NORMAL,[p[0],p[2],p[2],p[0],p[0]],[p[1],p[1],p[3],p[3],p[1]],color=0,THICK=2
;
;ADD date, time and elevation of the data
;FIND the time and date info of the image
YY=STRMID(date,0,4)
MM=STRMID(date,4,2)
DD=STRMID(date,6,2)
hh=STRMID(time,0,2)
mi=STRMID(time,2,2)
ss=STRMID(time,4,2)
;
;REPLACE ' ' with '0' where necessary
FOR i=0,STRLEN(hh) DO IF STRMID(hh,i,1) eq ' ' THEN STRPUT,hh,'0',i
FOR i=0,STRLEN(mi) DO IF STRMID(mi,i,1) eq ' ' THEN STRPUT,mi,'0',i
FOR i=0,STRLEN(ss) DO IF STRMID(ss,i,1) eq ' ' THEN STRPUT,ss,'0',i

thedatestring=STRING(FORMAT='(a2,"-",a2,"-",a4,", ",i02,":",i02," UTC")',DD,MM,YY,fix(hh),fix(mi))
XYOUTS,/NORMAL,p[0],p[3]+0.04, thedatestring
;
;COMPUTE the size of the colorbar (along the entire plot window)
cb_Size=(p[3]-p[1])-2*cb_yOffset

;LOAD the colors for the colordivbar only
TVLCT,R,G,B
colors=INDGEN(N_ELEMENTS(thresh)-1)
;PLOT the color bar

colordivbar,thresh,colors,color=0,format='(i0)',ticklen=0,chartick=2,$
lowleft=[p[2]+cb_xOffset,p[1]+cb_yOffset],ysize=cb_Size,/col,$
title=cb_Title
;
;CLOSE device, return to original device
DEVICE,/CLOSE
SET_PLOT,thisDevice
;
!P=bangp


END
