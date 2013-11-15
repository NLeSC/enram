;
;IDL routine to read hdf5 ODIM radar files
;
;Author: M. de Graaf (KNMI)
;Date: April 2012
;

PRO vol2bird_odimhdf5
  ;
  ;Call  
  ;
  ; RESOLVE_ROUTINE, 'volbird_odimhdf5', /compile_full_file
  ;
  ; in the main file to compile all the hdf5 routines 
  ; found in this file
  ;
END
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
PRO make_meta,meta
;
COMMON constants
;;
scanmeta=CREATE_STRUCT(     $
   'date'     , 0L  ,	    $		 ;Date of scan data in YYYYMMDD.		   
   'time'     , 0L  ,	    $		 ;Time of scan data in HHMMSS.  		   
   'heig'     , 0.0 ,	    $		 ;Height of radar antenna in km.		   
   'elev'     , 0.0 ,	    $		 ;Elevation of scan in deg.			   
   'nrang'    , 0L  ,	    $		 ;Number of range bins in scan. 		   
   'nazim'    , 0L  ,	    $		 ;Number of azimuth rays in scan.		   
   'rscale'   , 0.0 ,	    $		 ;Size of range bins in scan in km.		   
   'ascale'   , 0.0 ,	    $		 ;Size of azimuth steps in scan in deg. 	   
   'azim0'    , 0L  ,	    $		 ;Raynumber with which radar scan started.	   
   'zoffset'  , 0.0 ,	    $		 ;Offset value of quantity contained by scan.	   
   'zscale'   , 0.0 ,	    $		 ;Scale of value of quantity contained by scan.    
   'missing'  , 0L  ,	    $		 ;Missing value of quantity contained by scan.     
   'PRFh'     , 0.0 ,	    $		 ;High PRF used for scan in Hz. 		   
   'PRFl'     , 0.0 ,	    $		 ;Low PRF used for scan in Hz.  		   
   'pulse'    , 0.0 ,	    $		 ;Pulse length in microsec.			   
   'radcnst'  , 0.0 ,	    $		 ;Radar constant in dB. 			   
   'txnom'    , 0.0 ,	    $		 ;Nominal maximum TX power in kW.		   
   'antvel'   , 0.0 , 	    $		 ;Antenna velocity in deg/s.			   
   'lat'      , 0.0 ,	    $	    	 ;radar latitude
   'lon'      , 0.0 	    $	    	 ;radar longitude
)
meta = REPLICATE(scanmeta,NSCANX)
END
;
PRO string2datetime,datetimestring,date,time
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;This function converts a string according to the KNMI HDF5 specification to ;;
;;a date (yyyymmdd) and time (hhmmss) in (long) integers!!                    ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;   
;
;;Extraction of year, month, day, etc from string.
s = strtok(datetimestring,';',/EXTRACT)	;Separate fields on semi-colon
d = strtok(s[0], '-',  /EXTRACT)	;Date fields separated by colon
t = strtok(s[1], ':',  /EXTRACT)	;Time fields separated by colon
;
;;Returning date and time in integer format.*/
;
mon= where(strupcase(d[1]) eq $	
  ['XXX','JAN','FEB','MAR','APR','MAY','JUN','JUL','AUG','SEP','OCT','NOV','DEC'])
yy=FIX(d[2]) & dd=FIX(d[0]) & mon=FIX(mon[0])  
hh=FIX(t[0]) & mm=FIX(t[1]) & ss=FIX(t[2])
;
date=10000L*yy+100L*mon+dd;
time=10000L*hh+100L*mm+ss;
;
END
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
PRO read_odimh5_scan,h5_id,type,meta,scandata,in,$
imgflag=imgflag,success=success
;
COMMON constants
;
succes=0L
make_meta,meta
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;This function reads radar scan data from a ODIM HDF5 file. The scan number  ;;
;;and the type (Z,U,V,W) should be selected. The metadata and scandata are    ;;
;;returned.                                                                   ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;Reading of metadata from dataset (scan) group in file.
nscan=0     ;FIND number of datasets
for i=0, H5G_GET_NUM_OBJS(h5_id)-1 DO $
  IF 'dataset' eq STRMID(STRLOWCASE(H5G_GET_MEMBER_NAME(h5_id,'/',i)),0,7) THEN nscan+=1
;
IF Nscan le 0 OR Nscan ge NSCANX THEN $ 	    ;;CHECK this number
   MESSAGE,"Number of scan groups is out of range!"
;
scandata=CREATE_STRUCT(type+"scan",Nscan)
;   
FOR iscan=1,Nscan DO BEGIN
  ;
  ;Reading of meta[iscan]data from top-level where group in file.
  scan_id = H5G_OPEN(h5_id,'/')
  ;
  where=H5_PARSE(scan_id,'where')
  meta[iscan].heig=where.HEIGHT._DATA*1e-3
  meta[iscan].lat=where.LAT._DATA
  meta[iscan].lon=where.LON._DATA
  ;
  what=H5_PARSE(scan_id,STRING(FORMAT='("/dataset",i0,"/what")',iscan))
  meta[iscan].date=what.startdate._DATA
  meta[iscan].time=what.starttime._DATA
  where=H5_PARSE(scan_id,STRING(FORMAT='("/dataset",i0,"/where")',iscan))
  meta[iscan].elev=where.elangle._DATA
  meta[iscan].nrang=where.nbins._DATA
  meta[iscan].nazim=where.nrays._DATA
  meta[iscan].rscale=where.rscale._DATA/1000.0
  meta[iscan].ascale=360./meta[iscan].nazim
  
  ;OPEN numbered dataset?? groups and find correct /data field
  read=0
  variable=0
  
  WHILE not read DO BEGIN
   variable+=1
    ;
    object=STRING(FORMAT='("/dataset",i0,"/data",i0)',iscan,variable)
    dset=H5_PARSE(scan_id,object)
    quantity=dset.what.quantity._data
    ;
    CASE  STRUPCASE(type) OF 
      'Z' : BEGIN
        IF STRUPCASE(quantity) eq 'DBZH' THEN BEGIN
  	  meta[iscan].zscale=dset.what.gain._data
	  meta[iscan].zoffset=dset.what.offset._data
	  meta[iscan].missing=LONG(dset.what.undetect._data)
	  read=1
	ENDIF
      END
      'U' : BEGIN
        IF STRUPCASE(quantity) eq 'TH' THEN BEGIN
  	  meta[iscan].zscale=dset.what.gain._data
	  meta[iscan].zoffset=dset.what.offset._data
	  meta[iscan].missing=LONG(dset.what.undetect._data)
	  read=1
	ENDIF
      END
      'V' : BEGIN
        IF STRUPCASE(quantity) eq 'VRAD' THEN BEGIN
  	  meta[iscan].zscale=dset.what.gain._data
	  meta[iscan].zoffset=dset.what.offset._data
	  meta[iscan].missing=LONG(dset.what.undetect._data)
	  read=1
	ENDIF
      END
      'W' : BEGIN
        IF STRUPCASE(quantity) eq 'WRAD' THEN BEGIN
  	  meta[iscan].zscale=dset.what.gain._data
	  meta[iscan].zoffset=dset.what.offset._data
	  meta[iscan].missing=LONG(dset.what.undetect._data)
	  read=1
	ENDIF
      END
      'R' : BEGIN
        IF STRUPCASE(quantity) eq 'RHO' THEN BEGIN
  	  meta[iscan].zscale=dset.what.gain._data
	  meta[iscan].zoffset=dset.what.offset._data
	  meta[iscan].missing=LONG(dset.what.undetect._data)
	  read=1
	ENDIF
      END
      'D' : BEGIN
        IF STRUPCASE(quantity) eq 'ZDR' THEN BEGIN
  	  meta[iscan].zscale=dset.what.gain._data
	  meta[iscan].zoffset=dset.what.offset._data
	  meta[iscan].missing=LONG(dset.what.undetect._data)
	  read=1
	ENDIF
      END
      ELSE: BEGIN
         PRINT, "Please select a type 'Z','U','V','W','R',or 'D' in read_odimh5_scan'
	 STOP,"Check code"
      END
    ENDCASE
    ;
    IF variable eq 3 THEN BEGIN
      read=1
      success=0
    ENDIF
    ;
  ENDWHILE
  ;
  object=STRING(FORMAT='("/dataset",i0,"/data",i0)',iscan,variable)
  scan_data=H5_PARSE(scan_id,object,/READ_DATA)
  ;
  ;CREATE an IDL structure that contains the data 
  ;from each scan, which may contain different array
  ;lengths. Each scan can be accessed by index, using
  ;the indexed structure format: scan2_data = scandata.(2)
  ;The number of scans is contained in the first field:
  ;number_of_scans = scandata.(0)
  scandata=CREATE_STRUCT(scandata,"scan"+STRTRIM(iscan,2),scan_data.DATA._DATA)	;;UPDATE the IDL structure
  ;
ENDFOR
;
H5G_CLOSE,scan_id    	    ;;CLOSE the current scan
;
IF N_PARAMS() eq 5 THEN BEGIN
  ;
  ;READ the groups that will be copied to the HDF5 output file
  gid = H5G_OPEN(h5_id, '/')
  what =H5_PARSE(gid,'what' ,/READ_DATA)
  where=H5_PARSE(gid,'where',/READ_DATA)
  ;
  in=CREATE_STRUCT( $
  "what",what,      $
  "where",where)
  ;
  IF 0 eq 1 THEN BEGIN
  nscan=scandata.(0)
  thescans=CREATE_STRUCT("SCAN_DATA_STRUCTURE",nscan)
  IF KEYWORD_SET(imgflag) THEN BEGIN
    FOR i=1,nscan DO BEGIN
      scanname = STRING(FORMAT='("scan",i0)',i)
      scan= H5_PARSE(gid,scanname,FILE=h5infile,/READ_DATA)
      in=CREATE_STRUCT(in,scanname,scan)
    ENDFOR
  ENDIF
  
  ENDIF
  ;
  H5G_CLOSE,gid
  ;
ENDIF
;
success= SIZE(meta,/TYPE) eq 8 and SIZE(scandata,/TYPE) eq 8 ? 1 : 0
;
END
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
PRO read_knmih5_scan,h5_id,type,meta,scandata,in,$
imgflag=imgflag,success=success
;
COMMON constants
;
succes=0L
make_meta,meta
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;This function reads radar scan data from a KNMI HDF5 file. The scan number  ;;
;;and the type (Z,U,V,W) should be selected. The metadata and scandata are    ;;
;;returned.                                                                   ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
scandata=CREATE_STRUCT("scan_"+type+"_DATA",h5_id)   	; CREATE an IDL structure to hold the data
;
;;Reading of metadata from radar group in file.
radar1=H5_PARSE(h5_id,'radar1',/READ_DATA)
radar1_tags=TAG_NAMES(radar1)

;FIND number of scan groups 'overview' group;
ovw_id=H5G_OPEN(h5_id,'overview')   	    	    ;;OPEN 'overview' group
nsg_id=H5A_OPEN_NAME(ovw_id,'number_scan_groups')   ;;OPEN 'scan' group 
Nscanin = H5A_READ(nsg_id) & Nscanin=Nscanin[0]     ;;READ number of scans
H5A_CLOSE,nsg_id    	    	    	    	    ;;CLOSE 'scan' group
H5G_CLOSE,ovw_id    	    	    	    	    ;;CLOSE 'overview' group
;
IF Nscanin le 0 OR Nscanin ge NSCANX THEN $ 	    ;;CHECK this number
   MESSAGE,"Number of scan groups is out of range!"
;
FOR iscan=1,Nscanin DO BEGIN
  ;
  ;OPEN numbered scan?? groups 
  object=STRING(FORMAT='("scan",i0)',iscan)
  scn_id=H5G_OPEN(h5_id,object)
  ;
  ;snr_id=H5A_OPEN_NAME(scn_id,"scan_number_range")
  ;nrange=H5A_READ(snr_id)	    	    	;; READ the number range
  ;
  scanmeta=CREATE_STRUCT(object,scn_id)   	; CREATE an IDL structure to hold all attributes
  ;
  ;SCAN and load all attributes
  n_attr=H5A_GET_NUM_ATTRS(scn_id)    	;;FIND number of attributes
  FOR i=0,n_attr-1 DO BEGIN
    atr_id=H5A_OPEN_IDX(scn_id,i)     	;;OPEN each attribute
    atr_name=H5A_GET_NAME(atr_id)     	;;FIND its name 
    attr=H5A_READ(atr_id)     	    	;;READ the data
  ;
  ;PRINT the attributes
  ;  print, FORMAT='(a," = ",$)',idx_name & print, meta
  ;
  ;CLOSE and update
    H5A_CLOSE,atr_id  	    	    	    	    ;;CLOSE the attribute
    scanmeta = CREATE_STRUCT(scanmeta,atr_name,attr);;UPDATE IDL structure 
    scanmeta_tags = tag_names(scanmeta)     	    ;;GET all field tags
  ENDFOR
  ;
  ;CONVERT "scan_number_range" to appropriate values 
  IF TOTAL(scanmeta_tags eq 'SCAN_NUMBER_RANGE') eq 1 THEN $ 
    nrange = scanmeta.scan_number_range
  CASE  STRUPCASE(type) OF 
    'U' : IF TOTAL(scanmeta_tags eq 'SCAN_UZ_NUMBER_RANGE') eq 1 THEN nrange=scanmeta.scan_uZ_number_range
    'V' : IF TOTAL(scanmeta_tags eq 'SCAN_V_NUMBER_RANGE')  eq 1 THEN nrange=scanmeta.scan_V_number_range
    'Z' : IF TOTAL(scanmeta_tags eq 'SCAN_Z_NUMBER_RANGE')  eq 1 THEN nrange=scanmeta.scan_z_number_range
    ELSE: 
  ENDCASE
;;CHECK the above portion of the code when a file with these fields is encountered  
  IF STRUPCASE(atr_name) eq 'SCAN_V_NUMBER_RANGE' THEN BEGIN
    MESSAGE, 'Found a file with "SCAN_V_NUMBER_RANGE", please check this portion of the code!'  
  ENDIF
  ;
  ;CONVERT the date string
  string2datetime,scanmeta.scan_datetime,date,time  ;; CONVERT to (long) integer (YYYYMnDD) 
    	    	    	    	    	    	    ;; and (HHMMSS)
  ;OPEN the groups and read the data. 
  ngroup = H5G_GET_NUM_OBJS(scn_id)
  FOR igroup=0,ngroup-1 DO BEGIN
    ;
    object_name = H5G_GET_OBJ_NAME_BY_IDX(scn_id,igroup)
;    print, object_name
    ;
    ;READ calibration metadata
    IF STRUPCASE(object_name) eq "CALIBRATION" THEN BEGIN
      ;
      cal_id=H5G_OPEN(scn_id,object_name)     	;;OPEN the calibration group
      cal=CREATE_STRUCT('calibration',cal_id)	;;CREATE an IDL structure
      ;
      ;SCAN and load all attributes
      n_attr=H5A_GET_NUM_ATTRS(cal_id)    	;;FIND number of attributes
      FOR iattr=0,n_attr-1 DO BEGIN
	atr_id=H5A_OPEN_IDX(cal_id,iattr)     	;;OPEN each attribute
	atr_name=H5A_GET_NAME(atr_id)     	;;FIND its name 
	attr=H5A_READ(atr_id) 	    	    	;;READ the data
	H5A_CLOSE,atr_id  	    	    	;;CLOSE the attribute
	cal = CREATE_STRUCT(cal,atr_name,attr)  ;;UPDATE IDL structure 
	cal_tags = TAG_NAMES(cal)	    	;;GET the field names
      ENDFOR
      ;
      H5G_CLOSE,cal_id	    	    	    	;;CLOSE the calibration group
      ;
      ;ANALYSE the GEO string
      CASE STRUPCASE(type) of 
	'U' : string = cal.calibration_uZ_formulas
	'V' : string = cal.calibration_V_formulas
	'W' : string = cal.calibration_W_formulas
	'Z' : string = cal.calibration_Z_formulas
	'M' : string = cal.calibration_clutter_formulas
	'R' : string = cal.calibration_Rho_formulas
	'D' : string = cal.calibration_Zdr_formulas
	ELSE: MESSAGE, 'Type "'+type+'" not found'
      ENDCASE
      ;
      ;PROCESS the GEO string    
      strstr='='
      endstr='*'
      zscale=STRMID(string,STRPOS(string,strstr)+1,STRPOS(string,endstr)-(STRPOS(string,strstr)+1))
      ;
      strstr='+'
      zoffset=STRMID(string,STRPOS(string,strstr)+1)
      ;
;      zscale=STREGEX(string,'GEO\=(.[^\*])*',/extract) & zscale=FLOAT(STRMID(zscale,4))
;      zoffset=STREGEX(string,'PV\+(.*)',/extract) & zoffset=FLOAT(STRMID(zoffset,3))
      ;
    ENDIF
    ;
    ;READ scan data.
    ;
    CASE STRUPCASE(type) of 
      'U' : scan_data_name = "scan_uZ_data"
      'V' : scan_data_name = "scan_V_data"
      'W' : scan_data_name = "scan_W_data"
      'Z' : scan_data_name = "scan_Z_data"
      'M' : scan_data_name = "scan_clutter_data"
      'R' : scan_data_name = "scan_Rho_data"
      'D' : scan_data_name = "scan_Rho_data"
      ELSE: MESSAGE, 'Type "'+type+'" not found'
    ENDCASE
    ;
    ;READ the scandata of the appropriate type
    IF object_name eq scan_data_name THEN BEGIN
  ;    print,'Opening '+scan_data_name+' data'
      ;
;      scan=CREATE_STRUCT("scan_data",scn_id) 	  ;;CREATE an IDL structure
      dat_id = H5D_OPEN(scn_id,scan_data_name)	  ;;OPEN the data space
      this_scan_data = H5D_READ(dat_id)    	  ;;READ the data
      H5D_CLOSE,dat_id	    	    	    	  ;;CLOSE the data space
      ;
      ;CREATE an IDL structure that contains the data 
      ;from each scan, which may contain different array
      ;lengths. Each scan can be accessed by index, using
      ;the indexed structure format: scan2_data = scandata.(2)
      ;The number of scans is contained in the first field:
      ;number_of_scans = scandata.(0)
      scandata=CREATE_STRUCT(scandata,object+"_data",this_scan_data)	;;UPDATE the IDL structure
      scandata.(0) = iscan
      ;
    ENDIF
    ;
  ENDFOR
  ;
  ;LOAD the meta structure
  meta[iscan].date       = date       	    	    	    ;;LOAD date
  meta[iscan].time       = time       	    	    	    ;;LOAD time
  IF TOTAL(scanmeta_tags eq 'SCAN_ELEVATION') eq 1 THEN $   ;;if scan_elevation exists ..
    meta[iscan].elev     = scanmeta.scan_elevation    	    ;;.. LOAD it
    meta[iscan].nrang    = nrange   	   	    	    ;;LOAD appropriate scan_number_range
  IF TOTAL(scanmeta_tags eq 'SCAN_NUMBER_AZIM') eq 1 THEN $ ;;if scan_number_range exists ..
    meta[iscan].nazim    = scanmeta.scan_number_azim  	    ;;.. LOAD it
  IF TOTAL(scanmeta_tags eq 'SCAN_RANGE_BIN') eq 1 THEN $   ;;if scan_range_bin exists ..
    meta[iscan].rscale   = scanmeta.scan_range_bin  	    ;;.. LOAD it
  IF TOTAL(scanmeta_tags eq 'SCAN_AZIM_BIN') eq 1 THEN $    ;;if scan_azim_bin exists ..
    meta[iscan].ascale   = scanmeta.scan_azim_bin  	    ;;.. LOAD it
  IF TOTAL(scanmeta_tags eq 'SCAN_START_AZIM') eq 1 THEN $  ;;if scan_start_azim exists ..
    meta[iscan].azim0    = scanmeta.scan_start_azim   	    ;;.. LOAD it
  IF TOTAL(scanmeta_tags eq 'SCAN_LOW_PRF') eq 1 THEN $     ;;if scan_low_PRF exists ..
    meta[iscan].PRFl     = scanmeta.scan_low_PRF	    ;;.. LOAD it
  IF TOTAL(scanmeta_tags eq 'SCAN_HIGH_PRF') eq 1 THEN $    ;;if scan_high_PRF exists ..
    meta[iscan].PRFh     = scanmeta.scan_high_PRF	    ;;.. LOAD it
  IF TOTAL(scanmeta_tags eq 'SCAN_PULSE_WIDTH') eq 1 THEN $ ;;if scan_pulse_width exists ..
    meta[iscan].pulse     = scanmeta.scan_pulse_width       ;;.. LOAD it
  IF TOTAL(scanmeta_tags eq 'SCAN_RADAR_CONSTANT') eq 1 THEN $;;if scan_radar_constant exists ..
    meta[iscan].radcnst   = scanmeta.scan_radar_constant  ;;.. LOAD it
  IF TOTAL(scanmeta_tags eq 'SCAN_TX_POWER_NOM') eq 1 THEN $;;if scan_tx_power_nom exists ..
    meta[iscan].txnom     = scanmeta.scan_tx_power_nom      ;;.. LOAD it
  IF TOTAL(scanmeta_tags eq 'SCAN_ANTENNA_VELOCITY') eq 1 THEN $;;if scan_antenna_velocity exists ..
    meta[iscan].antvel    = scanmeta.scan_antenna_velocity  ;;.. LOAD it
    meta[iscan].zscale    = zscale    	    	    	    ;;LOAD scale
    meta[iscan].zoffset   = zoffset   	    	    	    ;;LOAD zoffset
  IF TOTAL(cal_tags eq 'CALIBRATION_MISSING_DATA') eq 1 THEN $;;if calibration_missing_data exists ..
    meta[iscan].missing  = cal.calibration_missing_data     ;;.. LOAD it
  IF TOTAL(radar1_tags eq 'RADAR_HEIGHT') eq 1 THEN $ 	    ;;if radar_height tag exists
    meta[iscan].heig     = radar1.radar_height._data*1e3    ;;.. LOAD it
  IF TOTAL(radar1_tags eq 'RADAR_LOCATION') eq 1 THEN BEGIN ;;if radar_location tag exists ..
    meta[iscan].lon      = radar1.radar_location._data[0]   ;;.. LOAD lon
    meta[iscan].lat      = radar1.radar_location._data[1]   ;;.. and lat
  ENDIF  
  ;
ENDFOR
;
H5G_CLOSE,scn_id    	    ;;CLOSE the current scan
;
IF N_PARAMS() eq 5 THEN BEGIN
  ;
  ;READ the groups that will be copied to the HDF5 output file
  gid = H5G_OPEN(h5_id, '/')
  radar1    =H5_PARSE(gid,'radar1'    ,/READ_DATA)
  image1    =H5_PARSE(gid,'image1'    ,/READ_DATA)
  geographic=H5_PARSE(gid,'geographic',/READ_DATA)
  overview  =H5_PARSE(gid,'overview'  ,/READ_DATA)
  visualisation1=H5_PARSE(gid,'visualisation1',/READ_DATA)
  visualisation2=H5_PARSE(gid,'visualisation2',/READ_DATA)
  visualisation3=H5_PARSE(gid,'visualisation3',/READ_DATA)
  ;
  in=CREATE_STRUCT(  	    	    $
  "image1"        ,image1,    	    $
  "radar1"        ,radar1,    	    $
  "overview"      ,overview,  	    $
  "geographic"    ,geographic,	    $
  "visualisation1",visualisation1,  $
  "visualisation2",visualisation2,  $
  "visualisation3",visualisation3)
  ;
  nscan=scandata.(0)
  thescans=CREATE_STRUCT("SCAN_DATA_STRUCTURE",nscan)
  IF KEYWORD_SET(imgflag) THEN BEGIN
    FOR i=1,nscan DO BEGIN
      scanname = STRING(FORMAT='("scan",i0)',i)
      scan= H5_PARSE(gid,scanname,FILE=h5infile,/READ_DATA)
      in=CREATE_STRUCT(in,scanname,scan)
    ENDFOR
  ENDIF
  ;
  H5G_CLOSE,gid
  ;
ENDIF
;
success= SIZE(meta,/TYPE) eq 8 and SIZE(scandata,/TYPE) eq 8 ? 1 : 0
;
END
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
PRO read_knmicm,h5cluttermap,cmmeta,cmscan
;
COMMON constants
;
make_meta,cmmeta
;
;READ a given cluttermap, and return the meta data and the data
;in the cmmeta and cmscan structures
;
;READ the HDF5 file
cm=H5_PARSE(h5cluttermap,/READ_DATA)
;
;REARRANGE structures to match the c-structures
;
; CREATE an IDL structure to hold the data
tmpscan=CREATE_STRUCT("SCAN_DATA",0)
;
;GET the tag_names and loop through them
tags=TAG_NAMES(cm)
ntags=N_ELEMENTS(tags)
nscan=0
;
FOR itag=0,ntags-1 DO BEGIN
  ;
  ;CHECK whether this is the (sub)structure we're looking for
  IF SIZE(cm.(itag),/TYPE) eq 8 THEN BEGIN	;size=8 is structure
    ;
    ;_NAME field holds the scan number
    iscan=FIX(STRMID(cm.(itag)._NAME,4))
    ;REMEMBER the total number of scan (highest number)
    nscan=nscan>(iscan)
    ;
    ;CONVERT the date string
    string2datetime,cm.(itag).scan_datetime._DATA,$ ;; CONVERT to 
    date,time     	;; (long) integer (YYYYMnDD) and (HHMMSS)
    ;
    ;PROCESS the GEO string    
    string=cm.(itag).calibration.CALIBRATION_CLUTTER_FORMULAS._data
    ;
    strstr='='
    endstr='*'
    zscale=STRMID(string,STRPOS(string,strstr)+1,STRPOS(string,endstr)-(STRPOS(string,strstr)+1))
    ;
    strstr='+'
    zoffset=STRMID(string,STRPOS(string,strstr)+1)
    ;
    ;LOAD the meta structure
    thisscan_tags=TAG_NAMES(cm.(itag))
    ;
    cmmeta[iscan].date       = date				;;LOAD date
    cmmeta[iscan].time       = time				;;LOAD time
    IF TOTAL(thisscan_tags eq 'SCAN_ELEVATION') eq 1 THEN $	;;if scan_elevation exists ..
      cmmeta[iscan].elev     = cm.(itag).SCAN_ELEVATION._DATA 	;;.. LOAD it
    IF TOTAL(thisscan_tags eq 'SCAN_NUMBER_RANGE') eq 1 THEN $	;;if scan_elevation exists ..
      cmmeta[iscan].nrang    = cm.(itag).SCAN_NUMBER_RANGE._DATA;;LOAD appropriate scan_number_range
    IF TOTAL(thisscan_tags eq 'SCAN_NUMBER_AZIM') eq 1 THEN $ ;;if scan_number_range exists ..
      cmmeta[iscan].nazim    = cm.(itag).SCAN_NUMBER_AZIM._DATA	;;.. LOAD it
    IF TOTAL(thisscan_tags eq 'SCAN_RANGE_BIN') eq 1 THEN $	;;if scan_range_bin exists ..
      cmmeta[iscan].rscale   = cm.(itag).SCAN_RANGE_BIN._DATA 	;;.. LOAD it
    IF TOTAL(thisscan_tags eq 'SCAN_AZIM_BIN') eq 1 THEN $	;;if scan_azim_bin exists ..
      cmmeta[iscan].ascale   = cm.(itag).SCAN_AZIM_BIN._DATA		;;.. LOAD it
    IF TOTAL(thisscan_tags eq 'SCAN_START_AZIM') eq 1 THEN $  ;;if scan_start_azim exists ..
      cmmeta[iscan].azim0    = cm.(itag).SCAN_START_AZIM._DATA 	;;.. LOAD it
    IF TOTAL(thisscan_tags eq 'SCAN_LOW_PRF') eq 1 THEN $	;;if scan_low_PRF exists ..
      cmmeta[iscan].PRFl     = cm.(itag).SCAN_LOW_PRF._DATA		;;.. LOAD it
    IF TOTAL(thisscan_tags eq 'SCAN_HIGH_PRF') eq 1 THEN $	;;if scan_high_PRF exists ..
      cmmeta[iscan].PRFh     = cm.(itag).SCAN_HIGH_PRF._DATA		;;.. LOAD it
    IF TOTAL(thisscan_tags eq 'SCAN_PULSE_WIDTH') eq 1 THEN $ ;;if scan_pulse_width exists ..
      cmmeta[iscan].pulse     = cm.(itag).SCAN_PULSE_WIDTH._DATA	;;.. LOAD it
    IF TOTAL(thisscan_tags eq 'SCAN_RADAR_CONSTANT') eq 1 THEN $;;if scan_radar_constant exists ..
      cmmeta[iscan].radcnst	= cm.(itag).SCAN_RADAR_CONSTANT._DATA  ;;.. LOAD it
    IF TOTAL(thisscan_tags eq 'SCAN_TX_POWER_NOM') eq 1 THEN $;;if scan_tx_power_nom exists ..
      cmmeta[iscan].txnom     = cm.(itag).SCAN_TX_POWER_NOM._DATA	;;.. LOAD it
    IF TOTAL(thisscan_tags eq 'SCAN_ANTENNA_VELOCITY') eq 1 THEN $;;if scan_antenna_velocity exists ..
      cmmeta[iscan].antvel    = cm.(itag).SCAN_ANTENNA_VELOCITY._DATA  ;;.. LOAD it
      cmmeta[iscan].zscale    = zscale  			;;LOAD scale
      cmmeta[iscan].zoffset   = zoffset 			;;LOAD zoffset
    ;
    ;CREATE an IDL structure that contains the data 
    ;from each scan, which may contain different array
    ;lengths. Each scan can be accessed by index, using
    ;the indexed structure format: scan2_data = scandata.(2)
    ;The number of scans is contained in the first field:
    ;number_of_scans = scandata.(0)
    ;
    this_scan_data=cm.(itag).SCAN_CLUTTER_DATA._DATA
    ;
    object=STRING(cm.(itag)._NAME)
    tmpscan=CREATE_STRUCT(tmpscan,object+"_data",this_scan_data)	;;UPDATE the IDL structure
    ;
  ENDIF  
  ;
ENDFOR 
;
;; CREATE the final IDL structure to hold the data
cmscan=CREATE_STRUCT("SCAN_CM_DATA",0) 	
cmscan.(0) = nscan  ;;first field holds the number of scans
;
;REARRANGE to make scan increase monotonously
tagnames=TAG_NAMES(tmpscan)
FOR iscan=1,nscan DO BEGIN
  ;  
  object='SCAN'+STRTRIM(iscan,2)+'_DATA'
  itag = WHERE(object eq tagnames)
  data=tmpscan.(itag)
  cmscan=CREATE_STRUCT(cmscan,object,data)	;;UPDATE the IDL structure    
  ;
ENDFOR
;
END
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
PRO write_birdprof_hdf5,h5outfile,in,	    $
vmeta,zmeta,uzmeta,   	    	    	    $
vscan,zscan,uZscan,cellmap,vtex,	    $
hdata,udata,vdata,wdata,xdata,zdata,nndata, $
VTEXSCALE,VTEXOFFSET,NLEV,MISSING,imgflag
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;CREATE HDF5 radar output file 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
what	      =in.what
where	      =in.where
;
hdataset = {_NAME:'profile_height',_TYPE:'Dataset',_DATA:REFORM(hdata[2,0:Nlev-1])}
udataset = {_NAME:'profile_u_wind',_TYPE:'Dataset',_DATA:REFORM(udata[2,0:Nlev-1])}
vdataset = {_NAME:'profile_v_wind',_TYPE:'Dataset',_DATA:REFORM(vdata[2,0:Nlev-1])}
wdataset = {_NAME:'profile_w_wind',_TYPE:'Dataset',_DATA:REFORM(wdata[2,0:Nlev-1])}
xdataset = {_NAME:'profile_radial_stddev',_TYPE:'Dataset',_DATA:REFORM(xdata[2,0:Nlev-1])}
zdataset = {_NAME:'profile_reflectivity',_TYPE:'Dataset',_DATA:REFORM(zdata[0,0:Nlev-1])}
zbdataset= {_NAME:'profile_bird_reflectivity',_TYPE:'Dataset',_DATA:REFORM(zdata[2,0:Nlev-1])}
ndataset = {_NAME:'profile_number',_TYPE:'Dataset',_DATA:REFORM(nndata[2,0:Nlev-1])}
;
profile1= {_NAME:'profile1',_TYPE:'GROUP',_COMMENT:"Bill",  $
ATTR1:{_NAME:'profile_missing_data',_TYPE:'ATTRIBUTE',_DATA:missing},$
ATTR2:{_NAME:'profile_number_levels',_TYPE:'ATTRIBUTE',_DATA:Nlev},$
HDATA:hdataset,UDATA:udataset,VDATA:vdataset,WDATA:wdataset,$
XDATA:xdataset,ZDATA:zdataset,ZBDATA:zbdataset,NDATA:ndataset}

out=CREATE_STRUCT(  	    	    $
"_NAME"         ,h5outfile, 	    $
"_ICONTYPE"     ,"hdf",     	    $
"_TYPE"         ,"GROUP",   	    $
"_FILE"         ,h5outfile, 	    $
"_PATH"         ,"/", 	    	    $
"_COMMENT"      ,"IDL vol2bird",    $
"what"          ,what,    	    $
"where"         ,where,    	    $
"profile1"  	,profile1)
;
;color_palette=visualisation1.color_palette
Nscan=vtex.(0)
IF KEYWORD_SET(imgflag) THEN BEGIN
  FOR iscan=1,Nscan DO BEGIN
    ;
    ;Write calibration information.
    vcaltxt=STRING(FORMAT='("GEO=",f0,"*PV+",f0)',vmeta[iscan].zscale,vmeta[iscan].zoffset)
    wcaltxt=STRING(FORMAT='("GEO=",f0,"*PV+",f0)',wmeta[iscan].zscale,wmeta[iscan].zoffset)
    zcaltxt=STRING(FORMAT='("GEO=",f0,"*PV+",f0)',zmeta[iscan].zscale,zmeta[iscan].zoffset)
   uzcaltxt=STRING(FORMAT='("GEO=",f0,"*PV+",f0)',uzmeta[iscan].zscale,uzmeta[iscan].zoffset)
    vtextxt=STRING(FORMAT='("GEO=",f0,"*PV+",f0)',VTEXSCALE,VTEXOFFSET)
    celltxt=STRING(FORMAT='("GEO=",f0,"*PV+",f0)',1,0.0)
    ;
    vcal={_NAME:'calibration_V_formulas',_TYPE:'ATTRIBUTE',_DATA:vcaltxt}
    wcal={_NAME:'calibration_W_formulas',_TYPE:'ATTRIBUTE',_DATA:wcaltxt}
    zcal={_NAME:'calibration_Z_formulas',_TYPE:'ATTRIBUTE',_DATA:zcaltxt}
   uzcal={_NAME:'calibration_uZ_formulas',_TYPE:'ATTRIBUTE',_DATA:uzcaltxt}
    tcal={_NAME:'calibration_vtex_formulas',_TYPE:'ATTRIBUTE',_DATA:vtextxt}
   cmcal={_NAME:'calibration_cellmap_formulas',_TYPE:'ATTRIBUTE',_DATA:celltxt}
     ;     
    calibration={_NAME:'calibration',_TYPE:'GROUP',$
                 ATTR1:vcal,ATTR2:wcal,ATTR3:zcal,ATTR4:uzcal,$
		 ATTR5:tcal,ATTR6:cmcal}
     ;
     ;CREATE the data structures
     scanvdata={_NAME:'scan_V_data',_TYPE:'Dataset',_DATA:vscan.(iscan),    $
    	   CLASS:{_NAME:'CLASS',_TYPE:'ATTRIBUTE',_DATA:'IMAGE'},  	    $
   IMAGE_VERSION:{_NAME:'IMAGE_VERSION',_TYPE:'ATTRIBUTE',_DATA:'1.2'},     $
  DISPLAY_ORIGIN:{_NAME:'DISPLAY_ORIGIN',_TYPE:'ATTRIBUTE',_DATA:'UL'}}
     scanwdata={_NAME:'scan_W_data',_TYPE:'Dataset',_DATA:wscan.(iscan),    $
    	   CLASS:{_NAME:'CLASS',_TYPE:'ATTRIBUTE',_DATA:'IMAGE'},  	    $
   IMAGE_VERSION:{_NAME:'IMAGE_VERSION',_TYPE:'ATTRIBUTE',_DATA:'1.2'},     $
  DISPLAY_ORIGIN:{_NAME:'DISPLAY_ORIGIN',_TYPE:'ATTRIBUTE',_DATA:'UL'}}
     scanzdata={_NAME:'scan_Z_data',_TYPE:'Dataset',_DATA:zscan.(iscan),    $
    	   CLASS:{_NAME:'CLASS',_TYPE:'ATTRIBUTE',_DATA:'IMAGE'},  	    $
   IMAGE_VERSION:{_NAME:'IMAGE_VERSION',_TYPE:'ATTRIBUTE',_DATA:'1.2'},     $
  DISPLAY_ORIGIN:{_NAME:'DISPLAY_ORIGIN',_TYPE:'ATTRIBUTE',_DATA:'UL'}}
    scanuZdata={_NAME:'scan_uZ_data',_TYPE:'Dataset',_DATA:uzscan.(iscan),  $
    	   CLASS:{_NAME:'CLASS',_TYPE:'ATTRIBUTE',_DATA:'IMAGE'},  	    $
   IMAGE_VERSION:{_NAME:'IMAGE_VERSION',_TYPE:'ATTRIBUTE',_DATA:'1.2'},     $
  DISPLAY_ORIGIN:{_NAME:'DISPLAY_ORIGIN',_TYPE:'ATTRIBUTE',_DATA:'UL'}}
     vtexdata={_NAME:'scan_vtex_data',_TYPE:'Dataset',_DATA:vtex.(iscan),   $
    	   CLASS:{_NAME:'CLASS',_TYPE:'ATTRIBUTE',_DATA:'IMAGE'},  	    $
   IMAGE_VERSION:{_NAME:'IMAGE_VERSION',_TYPE:'ATTRIBUTE',_DATA:'1.2'},     $
  DISPLAY_ORIGIN:{_NAME:'DISPLAY_ORIGIN',_TYPE:'ATTRIBUTE',_DATA:'UL'}}
    cellmdata={_NAME:'scan_cellmap_data',_TYPE:'DATASET',_DATA:cellmap.(iscan),$
    	   CLASS:{_NAME:'CLASS',_TYPE:'ATTRIBUTE',_DATA:'IMAGE'},  	    $
   IMAGE_VERSION:{_NAME:'IMAGE_VERSION',_TYPE:'ATTRIBUTE',_DATA:'1.2'},     $
  DISPLAY_ORIGIN:{_NAME:'DISPLAY_ORIGIN',_TYPE:'ATTRIBUTE',_DATA:'UL'}}
    ;
    ;CREATE the HDF5 scanXX group
    scanname=STRING(FORMAT='("scan",i0)',iscan)
    scan=CREATE_STRUCT(     	    $
    "_NAME"        ,scanname,	    $
    "_ICONTYPE"    ,"hdf",          $
    "_TYPE"        ,"GROUP",        $
    "_FILE"        ,h5outfile,      $
    "_PATH"        ,"/", 	    $
    "_COMMENT"    ,"vol2bird scans",$
    "calibration"  ,calibration,    $
    "scan_V_data"  ,scanvdata, 	    $
    "scan_W_data"  ,scanwdata, 	    $
    "scan_Z_data"  ,scanzdata, 	    $
    "scan_uZ_data" ,scanuZdata,	    $
    "scan_vtex_data",vtexdata,	    $
    "scan_cellmap_data",cellmdata)
    ;
    out=CREATE_STRUCT(out,scanname,scan)
    ;
  ENDFOR
ENDIF
;
;WRITE the HDF5 file
H5_CREATE,h5outfile,out
;	
END
