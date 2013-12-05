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

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; get_bird_ppi
;
; READ a PRF of VOL radar HDF5 file and plot a slice of data in PPI format
;
; MdG - KNMI November 2012
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
PRO get_bird_ppi,h5infile,image,$
profile_bird,profile_height,$
height=height,$
scan=scan,$
elevation=elevation,$
fringe=fringe,$
rain=rain,$
dx=dx,$
maxr=maxr,$
maxz=maxz,$
meta=meta,$
io_file=io_file,$
limit=limit,$
radar_location=radar_location,$
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
DBZFACTOR=335.4   ; FIXME ;conversion factor reflectivity factor Z to reflectivity eta.
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
scan=N_ELEMENTS(scan) eq 0 ? 1 : scan
;
cellvalue=0
IF KEYWORD_SET(fringe) THEN cellvalue=1
;
rainmask = KEYWORD_SET(rain) ? 0 : 1
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;Opening and reading of HDF5 radar input file.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
PRINT,FORMAT= '(%"\r","#Opening of HDF5 radar input file : ",a,$)',FILE_BASENAME(h5infile)

h5_id=H5F_OPEN(h5infile)            ;;OPEN the current HDF5 scan file

groups = H5_PARSE(h5_id,'/')
tags=TAG_NAMES(groups)
iscan = 0
idataset = 0
FOR itag=0,N_ELEMENTS(tags)-1 DO BEGIN
  IF STRPOS(STRUPCASE(tags[itag]),'SCAN') ne -1 THEN iscan+=1
  IF STRPOS(STRUPCASE(tags[itag]),'DATASET') ne -1 THEN idataset+=1
ENDFOR
;
IF iscan eq 0 and idataset eq 0 THEN BEGIN
  PRINT,'No datasets found in file '+h5_infile
  RETURN
ENDIF
lon = groups.WHERE.lon._DATA & lon=lon[0]
lat = groups.WHERE.lat._DATA & lat=lat[0]
heigh = groups.WHERE.height._DATA & heigh=heigh[0]
;
nz=0
ncellmap=0
scan_object = iscan gt 0 ? '/scan' : '/dataset'
scan_object += strtrim(scan,2)
datagroups=H5_PARSE(h5_id,scan_object)
datatags=TAG_NAMES(datagroups)
FOR itag=0,N_ELEMENTS(datatags)-1 DO BEGIN
  ;
  ;FIND the datax groups
  IF STRPOS(STRUPCASE(datatags[itag]),'DATA') ne -1 THEN  BEGIN
    ;
    quantity=datagroups.(itag).what.QUANTITY._DATA
    ;
    ;FIND 'cellmap' datax group
    IF STRUPCASE(quantity) eq 'CELLMAP' THEN BEGIN
      cm_object=scan_object+'/'+STRLOWCASE(datatags[itag])
      cellmap_data =  H5_PARSE(h5_id,cm_object,/READ)
      cellmap = cellmap_data.data._DATA
      ncellmap+=1
    ENDIF
    ;
    ;FIND 'DBZH' datax group
    IF STRUPCASE(quantity) eq 'DBZH' THEN BEGIN
     ;
      z_object=scan_object+'/'+STRLOWCASE(datatags[itag])
      z_data =  H5_PARSE(h5_id,z_object,/READ)
      zdata = z_data.data._DATA
      nz+=1
      ;
      zoffset = z_data.what.OFFSET._DATA
      zscale = z_data.what.GAIN._DATA
      ;
    ENDIF
    ;
  ENDIF
  ;
  ;FIND 'where' group
  IF STRPOS(STRUPCASE(datatags[itag]),'WHERE') ne -1 THEN BEGIN
    ;
    rscale = datagroups.(itag).RSCALE._DATA/1000.   & rscale=rscale[0]
    nrang = datagroups.(itag).NBINS._DATA           & nrang=nrang[0]
    nazim = datagroups.(itag).NRAYS._DATA           & nazim=nazim[0]
    elevation = datagroups.(itag).ELANGLE._DATA     & elevation=elevation[0]
    ascale = 360./nazim                             & ascale=ascale[0]
    ;
  ENDIF
ENDFOR
;
IF ncellmap eq 0 THEN BEGIN
  PRINT, 'No cellmap data found in '+h5infile
ENDIF
;
IF nz eq 0 THEN BEGIN
  PRINT, 'No DHBZ data found in '+h5infile
ENDIF
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;CREATING image
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;SET geographical projection parameters of Cartesian grid.
if maxr le 0.0 THEN maxr = nrang * rscale;
maxz = (maxz - zoffset) / zscale;
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
ir=LONG(rang/rscale)
ia=(LONG(azim/ascale)+nazim) MOD nazim
;
t0=SYSTIME(1)
;
;FIND data below maximum Z
inodata=WHERE(zdata gt maxz, count)
IF count gt 0 THEN zdata[inodata]=nodata
;
;MAP data to image
FOR j=0,Nrows-1 DO BEGIN
   FOR i=0,Ncols-1 DO BEGIN
      their=ir[i,j]
      theia=ia[i,j]
      IF (their lt nrang) THEN BEGIN

    IF rainmask THEN BEGIN
     ;
      IF (cellmap[their+theia*nrang] le cellvalue) THEN BEGIN
        ;
        theimage[i+j*Ncols]=zdata[their+theia*nrang]
        ;
      ENDIF ELSE theimage[i+j*Ncols]=nodata
      ;
    ENDIF ELSE theimage[i+j*Ncols]=zdata[their+theia*nrang]
    ;
      ENDIF ELSE theimage[i+j*Ncols]=nodata;
   ENDFOR
ENDFOR
;
image=REFORM(theimage,ncols,nrows)
image[where(image eq 0,/NULL)]=nodata
;
;FIND the center (radar coord.) and corners of the image
Lon_Lat0=[lon,lat]
dist=maxr/RADIUS
lim0=ROTATE(LL_ARC_DISTANCE(Lon_lat0,dist,270,/DEGREES),2)
lim1=ROTATE(LL_ARC_DISTANCE(Lon_lat0,dist,0,/DEGREES),2)
lim2=ROTATE(LL_ARC_DISTANCE(Lon_lat0,dist,90,/DEGREES),2)
lim3=ROTATE(LL_ARC_DISTANCE(Lon_lat0,dist,180,/DEGREES),2)
limit=[lim0,lim1,lim2,lim3]
;
radar_location=[lon,lat]
;
return
;
profile =H5_PARSE(h5_id,'/profile1' ,/READ_DATA)
where = H5_PARSE(h5_id,'/where')

height = where.HEIGHT._DATA
lon = where.LON._DATA
lat = where.LAT._DATA
;
date = what.date._DATA
time = what.time._DATA

H5F_CLOSE,h5_id         ;;CLOSE the HDF5 file

profile_bird=profile.PROFILE_BIRD_REFLECTIVITY._DATA
profile_height=profile.PROFILE_HEIGHT._DATA


help, data
print, profile_bird;
stop



radar_location=[lon,lat]

return


END
