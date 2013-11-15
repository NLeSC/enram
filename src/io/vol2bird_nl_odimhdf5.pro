;
;IDL routine to read hdf5 radar files in KNMI format (NOT ODIM!)
;
;Author: M. de Graaf (KNMI)
;Date: April 2012
;

PRO vol2bird_nl_odimhdf5
  ;
  ;Call  
  ;
  ; RESOLVE_ROUTINE, 'vol2bird_nl_odimhdf5', /compile_full_file
  ;
  ; in the main file to compile all the hdf5 routines found in this file
  ; 
  ; All callable routines in this file are named the same as the 
  ; main routine with added names describing the function
  ; (e.g. vol2bird_nl_odimhdf5_read_scan for the scan read procedure)
  ; to distinguish routines for different systems
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
   'antvel'   , 0.0 ,	    $		 ;Antenna velocity in deg/s.
   'id'       , " " , 	    $	    	 ;radar id
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
PRO vol2bird_nl_odimhdf5_get_datetime,file,this_date,this_time
;
IF SIZE(file[0],/TYPE) ne 7 THEN BEGIN
  PRINT, 'please, provide a string (array) of filename(s)'
  stop
ENDIF
this_date=STRMID(FILE_BASENAME(file),16,8)
this_time=STRMID(FILE_BASENAME(file),24,4)+'00'
;
END
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
PRO vol2bird_nl_odimhdf5_read_scan, 	$
h5infile,type,meta,scandata,in,	    	$
processed_datetime=processed_datetime,	$
scanid=scanid,imgflag=imgflag,    	$
success=success
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
this_path=FILE_DIRNAME(h5infile,/MARK_DIRECTORY)
;
;GET the date / time string, this will be used to find all
;other files/scans.
;
filebase=FILE_BASENAME(h5infile,'.h5')
this_code=STRMID(filebase,4,4)
thisdatetime=STRMID(filebase,16,12)
;
;IF the hdf5 file to read is not present, return silently.
;Checking should be performed in the calling routine.
;Both this, and the overhead of opening and closing is 
;necessary for compatibility with other radar systems and
;formats
;
IF FILE_TEST(h5infile) eq 0 THEN RETURN
h5_id=H5F_OPEN(h5infile)	    	    	    ;;OPEN the HDF5 file
;
scandata=CREATE_STRUCT("scan_"+type+"_DATA",h5_id)  ;CREATE an IDL structure to hold the data
;
;;Reading of metadata from radar group in file.
radar1=H5_PARSE(h5_id,'radar1',/READ_DATA)
radar1_tags=TAG_NAMES(radar1)

;FIND number of scan groups 'overview' group;
ovw_id=H5G_OPEN(h5_id,'overview')   	    	    ;;OPEN 'overview' group
nsg_id=H5A_OPEN_NAME(ovw_id,'number_scan_groups')   ;;OPEN 'scan' group 
Nscanin = H5A_READ(nsg_id) & Nscanin=Nscanin[0]     ;;READ number of scans
H5A_CLOSE,nsg_id    	    	    	    	    ;;CLOSE 'scan' group

pgn_id=H5A_OPEN_NAME(ovw_id,'product_group_name')   ;;OPEN product_group_name 
pgn = H5A_READ(pgn_id) & pgn=pgn[0]         	    ;;READ product name group
H5A_CLOSE,pgn_id    	    	    	    	    ;;CLOSE product_group_name
H5G_CLOSE,ovw_id    	    	    	    	    ;;CLOSE 'overview' group
;
IF Nscanin le 0 OR Nscanin ge NSCANX THEN $ 	    ;;CHECK this number
   MESSAGE,"Number of scan groups is out of range!"
;
IF KEYWORD_SET(scanid) THEN BEGIN

  IF scanid le 0 OR scanid gt Nscanin THEN $ 	    ;;CHECK given scanid
     MESSAGE,'Selected scan group ('+STRTRIM(scanid)+') is out of range!'

  beginscan=scanid
  endscan=scanid
ENDIF ELSE BEGIN
  beginscan=1
  endscan=Nscanin
ENDELSE
;2.1
FOR iscan=beginscan,endscan DO BEGIN
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
  IF KEYWORD_SET(pgn) THEN $
    meta[iscan].id = STRMID(pgn,4,4)  	    	    	    ;;.. and radar id
  ;
ENDFOR
;
H5G_CLOSE,scn_id    	    ;;CLOSE the current scan
;
IF N_PARAMS() eq 5 THEN BEGIN
  ;
  ;READ the groups that will be copied to the HDF5 output file
  gid = H5G_OPEN(h5_id, '/')
  radar1    =H5_PARSE(gid,'radar1'    ,FILE=h5infile,/READ_DATA)
  image1    =H5_PARSE(gid,'image1'    ,FILE=h5infile,/READ_DATA)
  geographic=H5_PARSE(gid,'geographic',FILE=h5infile,/READ_DATA)
  overview  =H5_PARSE(gid,'overview'  ,FILE=h5infile,/READ_DATA)
  visualisation1=H5_PARSE(gid,'visualisation1',FILE=h5infile,/READ_DATA)
  visualisation2=H5_PARSE(gid,'visualisation2',FILE=h5infile,/READ_DATA)
  visualisation3=H5_PARSE(gid,'visualisation3',FILE=h5infile,/READ_DATA)
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
  ;CREATE an ODIM compatible structure
  ;
  IF STRUPCASE(radar1.RADAR_NAME._data) eq "DE_BILT" THEN $
    source = "RAD:NL50,NOD:nldbl,PLC:De Bilt"
  IF STRUPCASE(radar1.RADAR_NAME._data) eq "DEN_HELDER" THEN $
    source = "RAD:NL51,NOD:nldhl,PLC:Den Helder"
  what= {_NAME:'what',_TYPE:"GROUP",$
          date:{_NAME:'date',_TYPE:'ATTRIBUTE',_DATA:STRMID(thisdatetime,0,8)},$
          object:{_NAME:'object',_TYPE:'ATTRIBUTE',_DATA:'PVOL'},$
          source:{_NAME:'SOURCE',_TYPE:'ATTRIBUTE',_DATA:source},$
	  time:{_NAME:'time',_TYPE:'ATTRIBUTE',_DATA:STRMID(thisdatetime,8,8)},$
          version:{_NAME:'version',_TYPE:'ATTRIBUTE',_DATA:"H5rad 2.1"}}
  where = {_NAME:'where',_TYPE:"GROUP",$
          height:{_NAME:'height',_TYPE:'ATTRIBUTE',_DATA:meta[1].heig},$
	  lat:{_NAME:'lat',_TYPE:'ATTRIBUTE',_DATA:meta[1].lat},$
          lon:{_NAME:'lon',_TYPE:'ATTRIBUTE',_DATA:meta[1].lon}}

  odim = {what:what,where:where}  
  
  FOR iscan=1,nscan DO BEGIN
    ;
    ;GET the scan data
    scanname = STRING(FORMAT='("scan",i0)',iscan)
    scan= H5_PARSE(gid,scanname,FILE=h5infile,/READ_DATA)
    ;
    ;PUT TH data in
    uzdata=scan.SCAN_UZ_Data._DATA
    ; 
    ;PROCESS the calibration string    
    strstr='='
    endstr='*'
    uzscale=STRMID(cal.CALIBRATION_UZ_FORMULAS,STRPOS(cal.CALIBRATION_UZ_FORMULAS,strstr)+1,STRPOS(cal.CALIBRATION_UZ_FORMULAS,endstr)-(STRPOS(cal.CALIBRATION_UZ_FORMULAS,strstr)+1))
    ;
    strstr='+'
    uzoffset=STRMID(cal.CALIBRATION_UZ_FORMULAS,STRPOS(cal.CALIBRATION_UZ_FORMULAS,strstr)+1)
    ;  
    what = {_NAME:'what',_TYPE:"GROUP",$
            gain:{_NAME:'gain',_TYPE:'ATTRIBUTE',_DATA:uzscale},$
            nodata:{_NAME:'nodata',_TYPE:'ATTRIBUTE',_DATA:255},$
            offset:{_NAME:'offset',_TYPE:'ATTRIBUTE',_DATA:uzoffset},$
	    quantity:{_NAME:'quantity',_TYPE:'ATTRIBUTE',_DATA:'TH'},$
            undetect:{_NAME:'undetect',_TYPE:'ATTRIBUTE',_DATA:cal.CALIBRATION_MISSING_DATA}}

       uz_data={_NAME:'data',_TYPE:'DATASET',_DATA:uzdata,    $
    	   CLASS:{_NAME:'CLASS',_TYPE:'ATTRIBUTE',_DATA:'IMAGE'},  	    $
   IMAGE_VERSION:{_NAME:'IMAGE_VERSION',_TYPE:'ATTRIBUTE',_DATA:'1.2'},     $
  DISPLAY_ORIGIN:{_NAME:'DISPLAY_ORIGIN',_TYPE:'ATTRIBUTE',_DATA:'UL'}}

    uzdata_group = {_NAME:'data1',_TYPE:"GROUP",DATA:uz_data,WHAT:what}
    ;
    ;PUT DBZH data in
    zdata=scan.SCAN_Z_Data._DATA
    ; 
    ;PROCESS the calibration string    
    strstr='='
    endstr='*'
    zscale=STRMID(cal.CALIBRATION_Z_FORMULAS,STRPOS(cal.CALIBRATION_Z_FORMULAS,strstr)+1,STRPOS(cal.CALIBRATION_Z_FORMULAS,endstr)-(STRPOS(cal.CALIBRATION_Z_FORMULAS,strstr)+1))
    ;
    strstr='+'
    zoffset=STRMID(cal.CALIBRATION_Z_FORMULAS,STRPOS(cal.CALIBRATION_Z_FORMULAS,strstr)+1)
    ;  
    what = {_NAME:'what',_TYPE:"GROUP",$
            gain:{_NAME:'gain',_TYPE:'ATTRIBUTE',_DATA:zscale},$
            nodata:{_NAME:'nodata',_TYPE:'ATTRIBUTE',_DATA:255},$
            offset:{_NAME:'offset',_TYPE:'ATTRIBUTE',_DATA:zoffset},$
	    quantity:{_NAME:'quantity',_TYPE:'ATTRIBUTE',_DATA:'DBZH'},$
            undetect:{_NAME:'undetect',_TYPE:'ATTRIBUTE',_DATA:cal.CALIBRATION_MISSING_DATA}}

        z_data={_NAME:'data',_TYPE:'DATASET',_DATA:zdata,    $
    	   CLASS:{_NAME:'CLASS',_TYPE:'ATTRIBUTE',_DATA:'IMAGE'},  	    $
   IMAGE_VERSION:{_NAME:'IMAGE_VERSION',_TYPE:'ATTRIBUTE',_DATA:'1.2'},     $
  DISPLAY_ORIGIN:{_NAME:'DISPLAY_ORIGIN',_TYPE:'ATTRIBUTE',_DATA:'UL'}}
    ;
    zdata_group = {_NAME:'data2',_TYPE:"GROUP",DATA:z_data,WHAT:what}
    ;
    ;PUT VRAD data in
    vdata=scan.SCAN_V_Data._DATA
    ; 
    ;PROCESS the calibration string    
    strstr='='
    endstr='*'
    vscale=STRMID(cal.CALIBRATION_V_FORMULAS,STRPOS(cal.CALIBRATION_V_FORMULAS,strstr)+1,STRPOS(cal.CALIBRATION_V_FORMULAS,endstr)-(STRPOS(cal.CALIBRATION_V_FORMULAS,strstr)+1))
    ;
    strstr='+'
    voffset=STRMID(cal.CALIBRATION_V_FORMULAS,STRPOS(cal.CALIBRATION_V_FORMULAS,strstr)+1)
    ;  
    what = {_NAME:'what',_TYPE:"GROUP",$
            gain:{_NAME:'gain',_TYPE:'ATTRIBUTE',_DATA:vscale},$
            nodata:{_NAME:'nodata',_TYPE:'ATTRIBUTE',_DATA:255},$
            offset:{_NAME:'offset',_TYPE:'ATTRIBUTE',_DATA:voffset},$
	    quantity:{_NAME:'quantity',_TYPE:'ATTRIBUTE',_DATA:'VRAD'},$
            undetect:{_NAME:'undetect',_TYPE:'ATTRIBUTE',_DATA:cal.CALIBRATION_MISSING_DATA}}
    ;
        v_data={_NAME:'data',_TYPE:'DATASET',_DATA:vdata,    $
    	   CLASS:{_NAME:'CLASS',_TYPE:'ATTRIBUTE',_DATA:'IMAGE'},  	    $
   IMAGE_VERSION:{_NAME:'IMAGE_VERSION',_TYPE:'ATTRIBUTE',_DATA:'1.2'},     $
  DISPLAY_ORIGIN:{_NAME:'DISPLAY_ORIGIN',_TYPE:'ATTRIBUTE',_DATA:'UL'}}
    ;
    vdata_group = {_NAME:'data3',_TYPE:"GROUP",DATA:v_data,WHAT:what}
    ;
    string2datetime,scan.scan_datetime._DATA,scandate,scantime
    scanstartdate=STRING(FORMAT='(i08)',scandate)
    scanstarttime=STRING(FORMAT='(i06)',scantime)     
    ;
    ;CREATE a new dataset structure to hold the data
    what = {_NAME:'what',_TYPE:"GROUP",$
            startdate:{_NAME:'startdate',_TYPE:'ATTRIBUTE',_DATA:scanstartdate},$
;            enddate:{_NAME:'enddate',_TYPE:'ATTRIBUTE',_DATA:scanstartdate},$
            starttime:{_NAME:'starttime',_TYPE:'ATTRIBUTE',_DATA:scanstarttime},$
;            endtime:{_NAME:'endtime',_TYPE:'ATTRIBUTE',_DATA:scanstarttime},$
            product:{_NAME:'product',_TYPE:'ATTRIBUTE',_DATA:'SCAN'}}

    where = {_NAME:'where',_TYPE:"GROUP",$
            elangle:{_NAME:'elangle',_TYPE:'ATTRIBUTE',_DATA:meta[iscan].ELEV},$
            nbins:{_NAME:'nbins',_TYPE:'ATTRIBUTE',_DATA:meta[iscan].NRANG},$
            nrays:{_NAME:'nrays',_TYPE:'ATTRIBUTE',_DATA:meta[iscan].NAZIM},$
            rstart:{_NAME:'rstart',_TYPE:'ATTRIBUTE',_DATA:0.0},$
            rscale:{_NAME:'rscale',_TYPE:'ATTRIBUTE',_DATA:meta[iscan].RSCALE*1000.},$
            a1gate:{_NAME:'a1gate',_TYPE:'ATTRIBUTE',_DATA:meta[iscan].azim0}}

    dataset_name=STRING(FORMAT='("dataset",i0)',iscan)
    dataset = {what:what,   	    $
	       where:where, 	    $
               data1:uzdata_group,  $
	       data2:zdata_group,   $
	       data3:vdata_group}
    ;
    ;ADD dataset to the structure
    odim=CREATE_STRUCT(odim,dataset_name,dataset)
    ;
  ENDFOR
  ;
  ;CLOSE the hdf5 file
  H5G_CLOSE,gid
  ;
  ;REPLACE the olf KNMI style with the ODIM structure
  in = odim
  ;  
ENDIF
;
H5F_CLOSE,h5_id	    	    	    	    	    ;;CLOSE the HDF5 file
;
IF KEYWORD_SET(scanid) THEN meta=meta[beginscan:endscan] ELSE $
 meta = [meta[0],meta[beginscan:endscan]]
;meta= meta[beginscan:endscan]
;
success= SIZE(meta,/TYPE) eq 8 and SIZE(scandata,/TYPE) eq 8 ? 1L : 0L
;
IF success THEN processed_datetime=thisdatetime
;
END
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
PRO vol2bird_nl_odimhdf5_read_cluttermap,h5cluttermap,cmmeta,cmscan
;
COMMON constants
;
make_meta,cmmeta
;
;READ a given cluttermap, and return the meta data and the data
;in the cmmeta and cmscan structures
;
IF FILE_TEST(h5cluttermap) eq 0 THEN RETURN
h5_cm=H5F_OPEN(h5cluttermap)	    	    	    ;;OPEN the HDF5 file
;
;READ the HDF5 file
cm=H5_PARSE(h5cluttermap,/READ_DATA)
;
; CREATE an IDL structure to hold the data
tmpscan=CREATE_STRUCT("SCAN_DATA",0)
;
nscan=0
;
dataset_found=0
what_found=0
where_found=0
tagnames=TAG_NAMES(cm)
FOR itag=0,N_ELEMENTS(tagnames)-1 DO BEGIN
  ;  
  ;CHECK for meta data
  IF STRPOS(tagnames[itag],'WHERE') ne -1 THEN where_found+=1
  IF STRPOS(tagnames[itag],'WHAT') ne -1 THEN what_found+=1
  IF STRPOS(tagnames[itag],'DATASET') ne -1 THEN dataset_found+=1
  ;  
ENDFOR
IF dataset_found le 0 THEN BEGIN
  PRINT, 'No datasets found in clutterfile: '+h5cluttermap
  RETURN
ENDIF
;
FOR itag=0,N_ELEMENTS(tagnames)-1 DO BEGIN
  ;
  IF STRPOS(tagnames[itag],'DATASET') ne -1 THEN BEGIN
    ;
    iscan=FIX(STRMID(tagnames[itag],7,2))
    ;REMEMBER the total number of scan (highest number)
    nscan=nscan>(iscan)
    ; 
    ;CHECK for data and necessary meta-data
    data1_found=0
    dataset_what_found=0
    dataset_where_found=0
    datasettags=TAG_NAMES(cm.(itag))
    FOR jtag=0,N_ELEMENTS(datasettags)-1 DO BEGIN
      IF STRPOS(datasettags[jtag],'DATA1' ) ne -1 THEN data1_found +=1
      IF STRPOS(datasettags[jtag],'WHAT' ) ne -1 THEN dataset_what_found +=1
      IF STRPOS(datasettags[jtag],'WHERE') ne -1 THEN dataset_where_found+=1
    ENDFOR
    ;
    IF data1_found gt 0 THEN BEGIN
      ;
      ;CHECK for calibration meta-data 
      data1_what_found=0
      datatags=TAG_NAMES(cm.(itag).data1)
      FOR jtag=0,N_ELEMENTS(datatags)-1 DO BEGIN
        IF STRPOS(datatags[jtag],'WHAT' ) ne -1 THEN data1_what_found +=1
      ENDFOR
      ;
      IF dataset_what_found THEN BEGIN
       cmmeta[iscan].date     = cm.(itag).what.STARTDATE._DATA ;;LOAD date
       cmmeta[iscan].time     = cm.(itag).what.STARTTIME._DATA ;;LOAD time
      ENDIF
      IF dataset_where_found THEN BEGIN
       cmmeta[iscan].elev     = cm.(itag).where.ELANGLE._DATA  ;;LOAD scan_elevation 
       cmmeta[iscan].nrang    = cm.(itag).where.NBINS._DATA    ;;LOAD scan_elevation
       cmmeta[iscan].nazim    = cm.(itag).where.NRAYS._DATA    ;;LOAD NRAYS
       cmmeta[iscan].rscale   = cm.(itag).where.RSCALE._DATA   ;;LOAD rscale
       cmmeta[iscan].azim0    = cm.(itag).where.A1GATE._DATA   ;;LOAD azim0
      ENDIF
      IF data1_what_found THEN BEGIN
       cmmeta[iscan].zscale   = cm.(itag).DATA1.what.GAIN._DATA     ;;LOAD zscale
       cmmeta[iscan].zoffset  = cm.(itag).DATA1.what.OFFSET._DATA   ;;LOAD zoffset
       cmmeta[iscan].missing  = cm.(itag).DATA1.what.UNDETECT._DATA ;;LOAD missing
      ENDIF
      IF where_found THEN BEGIN
       cmmeta[iscan].heig     = cm.where.HEIGHT._data	    ;;LOAD altitude
       cmmeta[iscan].lon      = cm.where.LON._data     	    ;;LOAD longitude
       cmmeta[iscan].lat      = cm.where.LAT._data     	    ;;LOAD latitude
      ENDIF
      ;
      ;CREATE an IDL structure that contains the data 
      ;from each scan, which may contain different array
      ;lengths. Each scan can be accessed by index, using
      ;the indexed structure format: scan2_data = scandata.(2)
      ;The number of scans is contained in the first field:
      ;number_of_scans = scandata.(0)
      ;
      this_scan_data=cm.(itag).data1.DATA._DATA
      ;
      object=STRING('SCAN'+STRTRIM(iscan,2))
      tmpscan=CREATE_STRUCT(tmpscan,object+"_data",this_scan_data)	;;UPDATE the IDL structure;
      ;
    ENDIF ELSE PRINT, 'No data found in scan '+STRTRIM(iscan,2)+' in clutterfile: '+h5cluttermap
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
PRO vol2bird_nl_odimhdf5_write_profile,     $
h5outfile,in,	    	    	    	    $
success=success,    	    	    	    $
vmeta,zmeta,uzmeta,   	    	    	    $
vscan,zscan,uZscan,cellmap,vtex,	    $
udata,vdata,wdata,xdata,zdata,nndata,nzdata,$
cdata,hdata,dd,ff,etabird,rhobird,   	    $
fracbird,fracclut,fracfringe,fracrain,	    $
VTEXSCALE,VTEXOFFSET,NLEV,MISSING,imgflag
;
success=0
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;CREATE HDF5 radar output file 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;CREATE the output structure
out=CREATE_STRUCT(  	    	    $
"_NAME"         ,h5outfile, 	    $
"_ICONTYPE"     ,"hdf",     	    $
"_TYPE"         ,"GROUP",   	    $
"_FILE"         ,h5outfile, 	    $
"_PATH"         ,"/", 	    	    $
"_COMMENT"      ,"IDL nl vol2bird", $
"what"          ,in.what,    	    $
"where"         ,in.where)
;
;CREATE the profile1 (birds) data set
h1dataset = {_NAME:'profile_height',_TYPE:'Dataset',_DATA:REFORM(hdata[0,0:Nlev-1])}
u1dataset = {_NAME:'profile_u_bird',_TYPE:'Dataset',_DATA:REFORM(udata[0,0:Nlev-1])}
v1dataset = {_NAME:'profile_v_bird',_TYPE:'Dataset',_DATA:REFORM(vdata[0,0:Nlev-1])}
w1dataset = {_NAME:'profile_w_bird',_TYPE:'Dataset',_DATA:REFORM(wdata[0,0:Nlev-1])}
ff1dataset = {_NAME:'profile_speed',_TYPE:'Dataset',_DATA:REFORM(ff[0,0:Nlev-1])}
dd1dataset = {_NAME:'profile_direction',_TYPE:'Dataset',_DATA:REFORM(dd[0,0:Nlev-1])}
x1dataset = {_NAME:'profile_radial_stddev',_TYPE:'Dataset',_DATA:REFORM(xdata[0,0:Nlev-1])}
x21dataset = {_NAME:'profile_radial_stddev_wind',_TYPE:'Dataset',_DATA:REFORM(xdata[2,0:Nlev-1])}
z1dataset = {_NAME:'profile_reflectivity',_TYPE:'Dataset',_DATA:REFORM(zdata[0,0:Nlev-1])}
eta1dataset= {_NAME:'profile_bird_reflectivity',_TYPE:'Dataset',_DATA:REFORM(etabird[0:Nlev-1])}
rho1dataset= {_NAME:'profile_bird_density',_TYPE:'Dataset',_DATA:REFORM(rhobird[0:Nlev-1])}
fb1dataset= {_NAME:'profile_fraction_bird',_TYPE:'Dataset',_DATA:REFORM(fracbird[0:Nlev-1])}
fnb1dataset= {_NAME:'profile_fraction_nonbird',_TYPE:'Dataset',_DATA:REFORM(fracrain[0:Nlev-1])}
fr1dataset= {_NAME:'profile_fraction_fringe',_TYPE:'Dataset',_DATA:REFORM(fracfringe[0:Nlev-1])}
cl1dataset= {_NAME:'profile_fraction_clutter',_TYPE:'Dataset',_DATA:REFORM(fracclut[0:Nlev-1])}
cm1dataset= {_NAME:'profile_fraction_cellmap',_TYPE:'Dataset',_DATA:REFORM(cdata[2,0:Nlev-1])}
n1dataset = {_NAME:'profile_number',_TYPE:'Dataset',_DATA:REFORM(nndata[0,0:Nlev-1])}
nz1dataset = {_NAME:'profile_dbz_number',_TYPE:'Dataset',_DATA:REFORM(nzdata[0,0:Nlev-1])}
;
profile1= {_NAME:'profile1',_TYPE:'GROUP',_COMMENT:"bird profile",  	$
ATTR1:{_NAME:'profile_missing_data',_TYPE:'ATTRIBUTE',_DATA:missing},	$
ATTR2:{_NAME:'profile_number_levels',_TYPE:'ATTRIBUTE',_DATA:Nlev}, 	$
HDATA:h1dataset,UDATA:u1dataset,VDATA:v1dataset,WDATA:w1dataset,	    	$
FFDATA:ff1dataset,DDDATA:dd1dataset,XDATA:x1dataset,X2DATA:x21dataset,  	$
ZDATA:z1dataset,ETADATA:eta1dataset,RHODATA:rho1dataset,FBDATA:fb1dataset,	$
FNBDATA:fnb1dataset,FRDATA:fr1dataset,CLDATA:cl1dataset,CMDATA:cm1dataset,	$
NDATA:n1dataset,NZDATA:nz1dataset}
;
;CREATE the profile2 (non-birds) data set
h2dataset = {_NAME:'profile_height',_TYPE:'Dataset',_DATA:REFORM(hdata[1,0:Nlev-1])}
u2dataset = {_NAME:'profile_u_nonbird',_TYPE:'Dataset',_DATA:REFORM(udata[1,0:Nlev-1])}
v2dataset = {_NAME:'profile_v_nonbird',_TYPE:'Dataset',_DATA:REFORM(vdata[1,0:Nlev-1])}
w2dataset = {_NAME:'profile_w_nonbird',_TYPE:'Dataset',_DATA:REFORM(wdata[1,0:Nlev-1])}
ff2dataset = {_NAME:'profile_speed',_TYPE:'Dataset',_DATA:REFORM(ff[1,0:Nlev-1])}
dd2dataset = {_NAME:'profile_direction',_TYPE:'Dataset',_DATA:REFORM(dd[1,0:Nlev-1])}
x2dataset = {_NAME:'profile_radial_stddev',_TYPE:'Dataset',_DATA:REFORM(xdata[1,0:Nlev-1])}
z2dataset = {_NAME:'profile_reflectivity',_TYPE:'Dataset',_DATA:REFORM(zdata[1,0:Nlev-1])}
n2dataset = {_NAME:'profile_number',_TYPE:'Dataset',_DATA:REFORM(nndata[1,0:Nlev-1])}
nz2dataset = {_NAME:'profile_dbz_number',_TYPE:'Dataset',_DATA:REFORM(nzdata[1,0:Nlev-1])}
;
profile2= {_NAME:'profile2',_TYPE:'GROUP',_COMMENT:"non-bird profile",  $
ATTR1:{_NAME:'profile_missing_data',_TYPE:'ATTRIBUTE',_DATA:missing},	$
ATTR2:{_NAME:'profile_number_levels',_TYPE:'ATTRIBUTE',_DATA:Nlev}, 	$
HDATA:h2dataset,UDATA:u2dataset,VDATA:v2dataset,WDATA:w2dataset,	$
FFDATA:ff2dataset,DDDATA:dd2dataset,XDATA:x2dataset,   	    	    	$
ZDATA:z2dataset,NDATA:n2dataset,NZDATA:nz2dataset}
;
;CREATE the profile3 (wind) data set
h3dataset = {_NAME:'profile_height',_TYPE:'Dataset',_DATA:REFORM(hdata[2,0:Nlev-1])}
u3dataset = {_NAME:'profile_u_wind',_TYPE:'Dataset',_DATA:REFORM(udata[2,0:Nlev-1])}
v3dataset = {_NAME:'profile_v_wind',_TYPE:'Dataset',_DATA:REFORM(vdata[2,0:Nlev-1])}
w3dataset = {_NAME:'profile_w_wind',_TYPE:'Dataset',_DATA:REFORM(wdata[2,0:Nlev-1])}
ff3dataset = {_NAME:'profile_speed',_TYPE:'Dataset',_DATA:REFORM(ff[2,0:Nlev-1])}
dd3dataset = {_NAME:'profile_direction',_TYPE:'Dataset',_DATA:REFORM(dd[2,0:Nlev-1])}
x3dataset = {_NAME:'profile_radial_stddev',_TYPE:'Dataset',_DATA:REFORM(xdata[2,0:Nlev-1])}
z3dataset = {_NAME:'profile_reflectivity',_TYPE:'Dataset',_DATA:REFORM(zdata[2,0:Nlev-1])}
n3dataset = {_NAME:'profile_number',_TYPE:'Dataset',_DATA:REFORM(nndata[2,0:Nlev-1])}
nz3dataset = {_NAME:'profile_dbz_number',_TYPE:'Dataset',_DATA:REFORM(nzdata[2,0:Nlev-1])}
;
profile3= {_NAME:'profile3',_TYPE:'GROUP',_COMMENT:"wind profile",  $
ATTR1:{_NAME:'profile_missing_data',_TYPE:'ATTRIBUTE',_DATA:missing},	$
ATTR2:{_NAME:'profile_number_levels',_TYPE:'ATTRIBUTE',_DATA:Nlev}, 	$
HDATA:h3dataset,UDATA:u3dataset,VDATA:v3dataset,WDATA:w3dataset,	$
FFDATA:ff3dataset,DDDATA:dd3dataset,XDATA:x3dataset,   	    	    	$
ZDATA:z3dataset,NDATA:n3dataset,NZDATA:nz3dataset}
;
out=CREATE_STRUCT(out,'profile1',profile1,'profile2',profile2,'profile3',profile3)
;
;color_palette=visualisation1.color_palette
;
;IF imgflag is set then ADD the data in the scans/datasets
IF KEYWORD_SET(imgflag) THEN BEGIN
  ;
  Nscan=vtex.(0)	    ;FIND the number of scans
  datasetnames=TAG_NAMES(in)
  ;
  FOR iscan=1,Nscan DO BEGIN
    ;
    ;FIND the number of existing data groups in this dataset
    names=TAG_NAMES(in)
    dataset_name=STRING(FORMAT='("dataset",i0)',iscan)
    iset = WHERE(STRUPCASE(dataset_name) eq TAG_NAMES(in),n)
    IF n lt 1 THEN BREAK
    group_names=TAG_NAMES(in.(iset))
    ;
    ;FIND the number of DATAx groups
    ndata=0
    FOR igroup=0,N_ELEMENTS(group_names)-1 DO $
      IF STRUPCASE(STRMID(group_names[igroup],0,4)) eq 'DATA' THEN ndata+=1
    ;
    ;CREATE a dataset group and add all data groups. 
    dataset=CREATE_STRUCT(     	    $
    "_NAME"        ,dataset_name,   $
    "_ICONTYPE"    ,"hdf",          $255
    "_TYPE"        ,"GROUP",        $
    "_FILE"        ,h5outfile,      $
    "_PATH"        ,"/", 	    $
    "_COMMENT"     ,"IDL nl vol2bird")
    FOR igroup=0,N_ELEMENTS(group_names)-1 DO BEGIN
      dataset=CREATE_STRUCT(dataset,group_names(igroup),in.(iset).(igroup))
    ENDFOR
    ;
    ;ADD new data as new dataXX groups
    ;
    ;ADD VTEX 
         vtexdata={_NAME:'data',_TYPE:'Dataset',_DATA:vtex.(iscan),   $
    	   CLASS:{_NAME:'CLASS',_TYPE:'ATTRIBUTE',_DATA:'IMAGE'},  	    $
   IMAGE_VERSION:{_NAME:'IMAGE_VERSION',_TYPE:'ATTRIBUTE',_DATA:'1.2'},     $
  DISPLAY_ORIGIN:{_NAME:'DISPLAY_ORIGIN',_TYPE:'ATTRIBUTE',_DATA:'UL'}}
    ;  
    what = {_NAME:'what',_TYPE:"GROUP",$
            gain:{_NAME:'gain',_TYPE:'ATTRIBUTE',_DATA:VTEXSCALE},$
            nodata:{_NAME:'nodata',_TYPE:'ATTRIBUTE',_DATA:255.0},$
            offset:{_NAME:'offset',_TYPE:'ATTRIBUTE',_DATA:VTEXOFFSET},$
	    quantity:{_NAME:'quantity',_TYPE:'ATTRIBUTE',_DATA:'VTEX'},$
            undetect:{_NAME:'undetect',_TYPE:'ATTRIBUTE',_DATA:0.0}}
    ;
    group_name = STRING(FORMAT='("data",i0)',ndata+1)
    data_group = {_NAME:group_name,_TYPE:"GROUP",DATA:vtexdata,WHAT:what}
    ;
    dataset=CREATE_STRUCT(dataset,group_name,data_group)
    ;
    ;ADD CELLMAP
    cellmdata={_NAME:'data',_TYPE:'DATASET',_DATA:cellmap.(iscan),$
    	   CLASS:{_NAME:'CLASS',_TYPE:'ATTRIBUTE',_DATA:'IMAGE'},  	    $
   IMAGE_VERSION:{_NAME:'IMAGE_VERSION',_TYPE:'ATTRIBUTE',_DATA:'1.2'},     $
  DISPLAY_ORIGIN:{_NAME:'DISPLAY_ORIGIN',_TYPE:'ATTRIBUTE',_DATA:'UL'}}
    ;
    what = {_NAME:'what',_TYPE:"GROUP",$
           gain:{_NAME:'gain',_TYPE:'ATTRIBUTE',_DATA:1},$
           nodata:{_NAME:'nodata',_TYPE:'ATTRIBUTE',_DATA:255.0},$
           offset:{_NAME:'offset',_TYPE:'ATTRIBUTE',_DATA:0.0},$
	   quantity:{_NAME:'quantity',_TYPE:'ATTRIBUTE',_DATA:'CELLMAP'},$
           undetect:{_NAME:'undetect',_TYPE:'ATTRIBUTE',_DATA:0.0}}
    ;
    group_name = STRING(FORMAT='("data",i0)',ndata+2)
    data_group = {_NAME:group_name,_TYPE:"GROUP",DATA:cellmdata,WHAT:what}
    ;
    dataset=CREATE_STRUCT(dataset,group_name,data_group)
    ;
    ;CREATE the dataset for this scan
    out=CREATE_STRUCT(out,dataset_name,dataset)
    ;
  ENDFOR
ENDIF
;
;WRITE the HDF5 file
;
IF NOT FILE_TEST(FILE_DIRNAME(h5outfile,/MARK_DIRECTORY),/DIRECTORY) THEN FILE_MKDIR,FILE_DIRNAME(h5outfile)
H5_CREATE,h5outfile,out
;
success=1
;	
END
