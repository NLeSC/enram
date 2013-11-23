;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; get_ppi
;
; READ a PRF of VOL radar HDF5 file and plot a slice of data in PPI format
;
; MdG - KNMI November 2012
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
PRO get_ppi,h5infile,image,$
io_file=io_file,$
limit=limit,$
radar_location=radar_location,$
scan=scan,$
elevation=elevation,$
ppz=ppz,$
ppu=ppu,$
ppv=ppv,$
ppw=ppw,$
dx=dx,$
maxr=maxr,$
maxz=maxz,$
meta=meta,$
position=position,$
verbose=verbose,$
psfile=psfile,$
pngfile=pngfile,$
help=help
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
nodata=255    ;MAKE this same as missing, in order to remove the outer limit 'ring' when patching
;
position= N_ELEMENTS(position) eq 0 ? [0.02,0.02,0.9,0.9] : position
;
t0=SYSTIME(1)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;Opening and reading of HDF5 radar input file.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
PRINT,FORMAT= '(%"\r","#Opening of HDF5 radar input file : ",a,$)',FILE_BASENAME(h5infile)
;
;RESOLVE_ROUTINE,compile_iofile_command,/compile_full_file

compile_iofile_command=FILE_BASENAME(io_file,'.pro')
RESOLVE_ROUTINE,compile_iofile_command, /compile_full_file
;
;;Reading of radial velocity and reflectivity data of the selected scans.
;
CALL_PROCEDURE,compile_iofile_command+'_read_scan',h5infile,mode,meta,thedata,$
scanid=scanid,success=success
;
IF not KEYWORD_SET(success) then BEGIN
  PRINT, 'Scan data could not be loaded from '+h5infile
  RETURN
ENDIF
data=thedata.(1)
;
;MAP the data
elevation=meta.elev
;PRINT,FORMAT='("#Making PPI from ",a," at elevation   : ",g)',mode,meta.elev;
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

help, rangx, x, xoffset, xscale
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
image[where(image eq 0,/NULL)]=nodata
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
FOR i=0,STRLEN(ss) DO IF STRMID(ss,i,1) eq ' ' THEN STRPUT,ss,'0',i
;
radar_location=[meta.lon,meta.lat]
;
return

thisDevice = !D.Name
SET_PLOT,'Z',set_resolution=size(image,/dim)
;
;SET environment variable
bangp=!p
!P.FONT=0
;
;
;SET the map projection with the radar in the middle, and plot the image
map_set,meta.lat,meta.lon,position=[0,0,1,1],limit=limit
tvimage,image,order=1,position=[0,0,1,1]
;
;
!P=bangp


END
