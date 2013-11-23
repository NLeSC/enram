PRO map_ppi,files,limit,$
definitions=definitions,$
plot_lon=plot_lon,$
plot_lat=plot_lat,$
scan=scan,$
ppz=ppz,$
ppu=ppu,$
ppv=ppv,$
ppw=ppw,$
dx=dx,$
maxr=maxr,$
maxz=maxz,$
BgColor=BgColor,$
seacolor=seacolor,$
landcolor=landcolor,$
high_resolution=high_resolution,$
coast=coast,$
grid=grid,$
radar_cross=radar_cross,$
_EXTRA=extra,$
psfile=psfile
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;SETTINGS
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
RADIUS=6378.137     ;Earth radius 
;
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
seacolor  = N_ELEMENTS(seacolor ) eq 3 ?  seacolor : [224,255,255] 	;default sea color is light cyan
landcolor = N_ELEMENTS(landcolor) eq 3 ? landcolor : [255,239,213] 	;default land color is papaya
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
caldat,SYSTIME(/JULIAN,/UTC),mm,dd,yy,hh,mi,ss	;GET current date/time
psfile = N_ELEMENTS(psfile) ne 0 ? psfile : 'map_ppi_'+STRTRIM(DD,2)+STRTRIM(MM,2)+STRTRIM(YY,2)+'.ps'
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
a4xsize=21  	;center the image on an a4
a4ysize=29.7
;
DEVICE,/COLOR,BITS=8,FILE=psfile,XSIZE=ps_xsize,ysize=ps_ysize,   $
/Encapsulated, Preview=0, 	    	    	    	    $
XOFFSET=(a4xsize-ps_xsize)/2.,YOFFSET=(a4ysize-ps_ysize)/2.,SCALE=1
;
;DEFINE a map projection 
MAP_SET,plot_lon,plot_lat,limit=limit,position=p,/noborder,_EXTRA=extra
;
;GET ppi images and WARP them to the current map projection
dummy=TEMPORARY(composite)      ;EMPTY composite image
dummy=TEMPORARY(radar_locations)      ;EMPTY composite image
FOR ifile=0,N_ELEMENTS(files)-1 DO BEGIN
  ;
  get_ppi,files[ifile],image,limit=ppilimit,$
  io_file=definitions[ifile].io_file,$
  radar_loc=radar_loc,scan=scan,elevation=elevation,$
  ppz=ppz,ppu=ppu,ppv=ppv,ppw=ppw,dx=dx,maxr=maxr,maxz=maxz,meta=meta
  ;
  patch=BYTE(map_patch(image,lat0=ppilimit[6],lat1=ppilimit[2],lon0=ppilimit[1],lon1=ppilimit[5],$
        xstart=xstart,ystart=ystart,xsize=xsize,ysize=ysize,MAX_VALUE=254,MISSING=255))
  ;
  IF N_ELEMENTS(composite) eq 0 THEN composite=patch
  ;
  ;REMEMBER radar locations
  radar_locations = N_ELEMENTS(radar_locations) eq 0 ? radar_loc :[radar_locations,radar_loc]
  ;
  ;ADD images to composite where there's not yet data (0 and 255)
  data_index=WHERE(patch ne 255 AND patch ne 0,count)
  IF count gt 0 THEN composite[data_index]=patch[data_index]
  ;
;  map=map_image(image,xstart,ystart,latmin=limit[6],latmax=limit[2],lonmin=limit[1],lonmax=limit[5])
;  TV,map,xstart,ystart
  ;
ENDFOR
radar_locations=REFORM(radar_locations,2,N_ELEMENTS(files),/OVERWRITE)
;
;COLOR the sea and continents with the sea and land colors
;if BgColor is set
IF KEYWORD_SET(BgColor) THEN BEGIN
  ;
  ;MASK a background image: bg[254]=sea, bg[253]=land
  ;
  ;temoporarily SWITCH to memory to build the image
  ;DO this here, to match the resolution of the background image 
  ;to the composite image
  tmp_device=!D.NAME
  SET_PLOT,'Z'
  DEVICE,SET_RESOLUTION=[size(composite,/dim)]
  ;
  ;SET up a map projection identical to the final one,
  ;but filling the entire image/z buffer

  map_set,plot_lon,plot_lat,limit=limit,position=[0,0,1,1],/noborder,_EXTRA=extra
  POLYFILL,/normal,[[0,0],[0,1],[1,1],[1,0],[0,0]],color=254
  MAP_CONTINENTS,COLOR=255,/FILL,HIRES=high_resolution,COAST=coast
  bg=tvrd()
  ;
  ;RESTORE the plot device and pojection
  SET_PLOT,tmp_device
  map_set,plot_lon,plot_lat,limit=limit,position=p,/noborder,/noerase,_EXTRA=extra
  ;
  land=WHERE(bg eq 253 and composite eq 255,lcount)
  sea=WHERE(bg eq 254 and composite eq 255,scount)

  IF scount gt 0 THEN composite[sea ]=254
  IF lcount gt 0 THEN composite[land]=255
  ;
  ;LOAD seacolor
  load_color, seacolor,254  ;
  load_color,landcolor,255  ;
  ;
ENDIF ELSE load_color,missing_c,255  ;
;
;PLOT the composite image
TV,composite,xstart,ystart,xsize=xsize,ysize=ysize
;TV,bg,xstart,ystart,xsize=xsize,ysize=ysize
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
;ADD a cross to indicate the radar
IF KEYWORD_SET(radar_cross) THEN BEGIN

  load_color,crosscolor,0  ;black
  dist=crossdistance/Radius
  FOR i=0,N_ELEMENTS(files)-1 DO BEGIN
    radar_loc=[radar_locations[0,i],radar_locations[1,i]]
    cr0=LL_ARC_DISTANCE(radar_loc,dist,270,/DEGREES)
    cr1=LL_ARC_DISTANCE(radar_loc,dist,0,/DEGREES)
    cr2=LL_ARC_DISTANCE(radar_loc,dist,90,/DEGREES)
    cr3=LL_ARC_DISTANCE(radar_loc,dist,180,/DEGREES)
    PLOTS,/DATA,[radar_loc[0],cr0[0]],[radar_loc[1],cr0[1]],color=0,thick=crossthick
    PLOTS,/DATA,[radar_loc[0],cr1[0]],[radar_loc[1],cr1[1]],color=0,thick=crossthick
    PLOTS,/DATA,[radar_loc[0],cr2[0]],[radar_loc[1],cr2[1]],color=0,thick=crossthick
    PLOTS,/DATA,[radar_loc[0],cr3[0]],[radar_loc[1],cr3[1]],color=0,thick=crossthick
  ENDFOR

ENDIF
;
;ADD date, time and elevation of the data
;FIND the time and date info of the image
YY=STRMID(STRING(LONG(meta.date)),4,4)
MM=STRMID(STRING(LONG(meta.date)),8,2)
DD=STRMID(STRING(LONG(meta.date)),10,2)
hh=STRMID(STRING(LONG(meta.time)),6,2)
mi=STRMID(STRING(LONG(meta.time)),8,2)
ss=STRMID(STRING(LONG(meta.time)),10,2)
;
;REPLACE ' ' with '0' where necessary
FOR i=0,STRLEN(hh) DO IF STRMID(hh,i,1) eq ' ' THEN STRPUT,hh,'0',i
FOR i=0,STRLEN(mi) DO IF STRMID(mi,i,1) eq ' ' THEN STRPUT,mi,'0',i
FOR i=0,STRLEN(ss) DO IF STRMID(ss,i,1) eq ' ' THEN STRPUT,ss,'0',i

print, elevation
thedatestring=STRING(FORMAT='(a2,"-",a2,"-",a4,", ",i02,":",i02," UTC; ",f4.2," km alt. ")',DD,MM,YY,fix(hh),fix(mi),elevation)
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
PSTRACKER,psfile    ;replace the ps internal title with something more informative 
SET_PLOT,thisDevice
;
!P=bangp

END
