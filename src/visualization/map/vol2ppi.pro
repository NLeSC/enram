;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; vol2ppi
;
; READ a PRF of VOL radar HDF5 file and plot a slice of data in PPI format
;
; MdG - KNMI November 2012
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
PRO vol2ppi,h5infile,h5outfile,$
scan=scan,$
ppz=ppz,$
ppu=ppu,$
ppv=ppv,$
ppw=ppw,$
dx=dx,$
maxr=maxr,$
maxz=maxz,$
position=position,$
verbose=verbose,$
psfile=psfile,$
pngfile=pngfile,$
help=help
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;Definition of standard parameters.                                          ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
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
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;PARSE KEYWORDS
IF KEYWORD_SET(help) THEN BEGIN
ENDIF
;
scanid = N_ELEMENTS(scan) eq 1 ? scan : 1
IF KEYWORD_SET(ppz) THEN mode='Z' ELSE mode ='Z'
IF KEYWORD_SET(ppu) THEN mode='U'
IF KEYWORD_SET(ppv) THEN mode='V'
IF KEYWORD_SET(ppw) THEN mode='W'
;
dx = N_ELEMENTS(dx) eq 1 ? dx : 1.0
maxr = N_ELEMENTS(maxr) eq 1 ? maxr : -1.0
maxz = N_ELEMENTS(maxz) eq 1 ? maxz : 1.0E10
;
dx=LONG(dx)
maxr=LONG(maxr)
maxl=LONG(maxz)
;
nodata=255
;
position= N_ELEMENTS(position) eq 0 ? [0.02,0.02,0.9,0.9] : position
cb_Title='Reflectivity [dBz]'
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
Rr=R[index+1]
Gg=G[index+1]
Bb=B[index+1]
;
TVLCT,Rr,Gg,Bb
;
;ADD the missing and nodata colors
load_Color,nodata_c,255
Load_Color,missing_c,0
;
t0=SYSTIME(1)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;Opening and reading of HDF5 radar input file.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
PRINT,FORMAT= '("#Opening of HDF5 radar input file : ",a)',h5infile
;
RESOLVE_ROUTINE, 'volbird_knmihdf5', /compile_full_file
h5_id=H5F_OPEN(h5infile)                        ;;OPEN the HDF5 file
;
;;Reading of radial velocity and reflectivity data of the selected scans.

read_knmih5_scan,h5_id,mode,meta,thedata,scanid=scanid,success=success
data=thedata.(1)
;
geographic=H5_PARSE(h5_id,'geographic',/READ_DATA)
;
;CLOSE
H5F_CLOSE,h5_id                                     ;;CLOSE the HDF5 file
;
IF not KEYWORD_SET(success) then BEGIN
  PRINT, 'Scan data could not be loaded from ',h5infile
  RETURN
ENDIF
;
;MAP the data
PRINT,FORMAT='("#Making PPI from ",a," at elevation   : ",g)',mode,meta.elev;
;
;SET geographical projection parameters of Cartesian grid.
if maxr le 0.0 THEN maxr = meta.nrang * meta.rscale;
maxz = (maxz - meta.zoffset) / meta.zscale;
Nrows = LONG(2.0 * maxr / dx);
Ncols = Nrows;
xoffset = -1 * LONG(maxr / dx);
yoffset = xoffset;
xscale = dx;
yscale = -dx;
;
;CALCULATE Cartesian PPI image from scan data.
theimage=BYTARR((Ncols*1L)*(Nrows*1L))
;
x=LINDGEN(Ncols)
y=LINDGEN(Nrows)
rangx=(x+0.5+xoffset)*xscale
rangy=(y+0.5+yoffset)*yscale
rang=SQRT(rangy^2##(LONARR(Ncols)+1)+rangx^2#(LONARR(Nrows)+1))
azim=atan2(rangx#(LONARR(Nrows)+1),rangy##(LONARR(Ncols)+1))*!RADEG
;
ir=LONG(rang/meta.rscale)
ia=(LONG(azim/meta.ascale)+meta.nazim) MOD meta.nazim
;
t0=SYSTIME(1)
;
;FIND data below maximum Z
inodata=WHERE(data gt maxz, count)
IF count gt 0 THEN data[inodata]=nodata
;
;MAP data to image
FOR j=0,Nrows-1 DO BEGIN
   FOR i=0,Ncols-1 DO BEGIN
      their=ir[i,j]
      theia=ia[i,j]
      IF (their lt meta.nrang) THEN BEGIN
;   IF (data[their+theia*meta.nrang] le maxz) THEN BEGIN
      theimage[i+j*Ncols]=data[their+theia*meta.nrang]
;   ENDIF ELSE theimage[i+j*Ncols]=nodata
      ENDIF ELSE theimage[i+j*Ncols]=nodata;
   ENDFOR
ENDFOR
;print, systime(1)-t0
;
image=REFORM(theimage,ncols,nrows)
;
;FIND the center (radar coord.) and corners of the image
Lon_Lat0=[meta.lon,meta.lat]
dist=maxr/Radius
lim0=ROTATE(LL_ARC_DISTANCE(Lon_lat0,dist,270,/DEGREES),2)
lim1=ROTATE(LL_ARC_DISTANCE(Lon_lat0,dist,0,/DEGREES),2)
lim2=ROTATE(LL_ARC_DISTANCE(Lon_lat0,dist,90,/DEGREES),2)
lim3=ROTATE(LL_ARC_DISTANCE(Lon_lat0,dist,180,/DEGREES),2)
limit=[lim0,lim1,lim2,lim3]
;
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
;
;CONSTRUCT a postscript filename from the dat/time string, if it's not explicitely given
scanname= N_ELEMENTS(scanid) eq 1 ? '_scan'+STRTRIM(scanid,2) : ''
psfile= N_ELEMENTS(psfile) NE 1 ? 'vol2ppi_'+DD+MM+YY+'_'+hh+mi+scanname+'.ps' : psfile
;
;OPEN a postscript file
thisDevice = !D.Name
SET_PLOT,'ps'
xsize=20
ysize=20
a4xsize=21      ;center the image on an a4
a4ysize=29.7
;
DEVICE,/COLOR,BITS=8,FILE=psfile,XSIZE=xsize,ysize=ysize,   $
/Encapsulated, Preview=0,                               $
XOFFSET=(a4xsize-xsize)/2.,YOFFSET=(a4ysize-ysize)/2.,SCALE=1
;
;SET environment variable
bangp=!p
!P.FONT=0
;
p=position
cb_xOffset=0.04     ; colorbar position
cb_yOffset=0.03     ; "
cb_Size=(p[3]-p[1])-2*cb_yOffset
;
;SET the map projection with the radar in the middle, and plot the image
map_set,meta.lat,meta.lon,position=p,limit=limit
tvimage,image,order=1,position=p
;
;LOAD the colors for the colordivbar only
TVLCT,R,G,B
colors=INDGEN(N_ELEMENTS(thresh)-1)
;PLOT the color bar
colordivbar,thresh,colors,color=0,format='(i0)',ticklen=0,chartick=2,$
lowleft=[p[2]+cb_xOffset,p[1]+cb_yOffset],ysize=cb_Size,/col,$
title=cb_Title
;
;ADD continents and grid.
;NOTE: all colors are passed through index 0, this should be modified before, and no other index than 0
load_color,[110,110,110],0  ;dark grey
map_continents,color=0,thick=0.5,/hi,/coasts
load_color,[200,200,240],0  ;light grey
map_continents,/rivers,color=0,/hi,thick=0.1
load_color,[160,160,160],0  ;grey
map_continents,color=0,/hi,/countries

;ADD a rectangle around the image
load_color,[0,0,0],0  ;black
PLOTS,/NORMAL,[p[0],p[2],p[2],p[0],p[0]],[p[1],p[1],p[3],p[3],p[1]],color=0,THICK=2
;
;ADD a cross to indicate the radar
crossdistance = 10 ;km
crossthick=1
load_color,[0,0,0],0  ;black
dist=crossdistance/Radius
cr0=LL_ARC_DISTANCE([meta.lon,meta.lat],dist,270,/DEGREES)
cr1=LL_ARC_DISTANCE([meta.lon,meta.lat],dist,0,/DEGREES)
cr2=LL_ARC_DISTANCE([meta.lon,meta.lat],dist,90,/DEGREES)
cr3=LL_ARC_DISTANCE([meta.lon,meta.lat],dist,180,/DEGREES)
PLOTS,/DATA,[meta.lon,cr0[0]],[meta.lat,cr0[1]],color=0,thick=crossthick
PLOTS,/DATA,[meta.lon,cr1[0]],[meta.lat,cr1[1]],color=0,thick=crossthick
PLOTS,/DATA,[meta.lon,cr2[0]],[meta.lat,cr2[1]],color=0,thick=crossthick
PLOTS,/DATA,[meta.lon,cr3[0]],[meta.lat,cr3[1]],color=0,thick=crossthick
;
;ADD date, time and elevation of the data
thedatestring=STRING(FORMAT='(a2,"-",a2,"-",a4,", ",a2,":",a2,"; ",f4.2," km")',DD,MM,YY,hh,mi,meta.elev)
XYOUTS,/NORMAL,p[0],p[3]+0.01, thedatestring
;
;CLOSE device, return to original device
DEVICE,/CLOSE
PSTRACKER,psfile    ;replace the ps internal title with something more informative
SET_PLOT,thisDevice
;
!P=bangp

stop
END
