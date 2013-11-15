PRO common_definition
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;Definition of standard parameters.                                          ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
COMMON constants,NSCANX,RADIUS

NSCANX=64L  	    ;Maximum number of elevation scans.
RADIUS=6378.137     ;Earth radius 

END
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
PRO get_prof,h5infile,$
profile_bird,profile_height,$
height=height,$
io_file=io_file,$
limit=limit,$
radar_location=radar_location,$
verbose=verbose,$
psfile=psfile,$
pngfile=pngfile,$
help=help
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;PARSE KEYWORDS
IF KEYWORD_SET(help) THEN BEGIN
ENDIF
;
DBZFACTOR=335.4   ;conversion factor reflectivity factor Z to reflectivity eta.
;
nodata=255    ;MAKE this same as missing, in order to remove the outer limit 'ring' when patching
;
position= N_ELEMENTS(position) eq 0 ? [0.02,0.02,0.9,0.9] : position
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;Opening and reading of HDF5 radar input file.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
PRINT,FORMAT= '("#Opening of HDF5 radar input file : ",a)',FILE_BASENAME(h5infile)

h5_id=H5F_OPEN(h5infile)    	    ;;OPEN the current HDF5 scan file  

groups = H5_PARSE(h5_id,'/')
tags=TAG_NAMES(groups)
iscan = 0
idataset = 0
FOR itag=0,N_ELEMENTS(tags)-1 DO BEGIN
  IF STRPOS(STRUPCASE(tags[itag]),'SCAN') ne -1 THEN iscan+=1
  IF STRPOS(STRUPCASE(tags[itag]),'DATASET') ne -1 THEN idataset+=1
ENDFOR

IF iscan eq 0 and idataset eq 0 THEN BEGIN
  PRINT,'No datasets found in file '+h5_infile
  RETURN
ENDIF

profile =H5_PARSE(h5_id,'/profile1' ,/READ_DATA)
where = H5_PARSE(h5_id,'/where')
what = H5_PARSE(h5_id,'/what')

height = where.HEIGHT._DATA
lon = where.LON._DATA
lat = where.LAT._DATA

date = what.date._DATA
time = what.time._DATA

H5F_CLOSE,h5_id   	    ;;CLOSE the HDF5 file

;profile_bird=profile.PROFILE_REFLECTIVITY._DATA
;profile_bird=10*ALOG10(profile_bird/DBZFACTOR)

profile_bird=profile.PROFILE_BIRD_REFLECTIVITY._DATA
profile_height=profile.PROFILE_HEIGHT._DATA

;print, profile_bird;
;stop



radar_location=[lon,lat]

return


END
