;
;IDL routine to read hdf5 radar files in ODIM format from Croatia
;
;Author: M. de Graaf (KNMI)
;Date: April 2012
;

PRO vol2bird_hr_odimhdf5
  ;
  ;Call  
  ;
  ; RESOLVE_ROUTINE, 'vol2bird_hr_odimhdf5', /compile_full_file
  ;
  ; in the main file to compile all the hdf5 routines found in this file
  ; 
  ; All callable routines in this file are named the same as the 
  ; main routine with added names describing the function
  ; (e.g. vol2bird_hr_odimhdf5_read_scan for the scan read procedure)
  ; to distinguish routines for different systems
  ;
END
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
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
PRO vol2bird_hr_odimhdf5_get_datetime,file,this_date,this_time
;
IF SIZE(file[0],/TYPE) ne 7 THEN BEGIN
  PRINT, 'please, provide a string (array) of filename(s)'
  stop
ENDIF
this_date=STRMID(FILE_BASENAME(file),16,8)
this_time=STRMID(FILE_BASENAME(file),24,6)
;
END
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
PRO vol2bird_hr_odimhdf5_read_scan, 	$
h5infile,type,meta,scandata,in,	    	$
processed_datetime=processed_datetime,	$
scanid=scanid,imgflag=imgflag,    	$
success=success
;
COMMON constants
;
success=0L
make_meta,meta
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;This function reads radar scan data from a ODIM HDF5 file. The scan number  ;;
;;and the type (Z,U,V,W) should be selected. The metadata and scandata are    ;;
;;returned.                                                                   ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
IF FILE_TEST(h5infile) eq 0 THEN BEGIN 
  PRINT, 'The file '+h5infile+' does not exist'
  RETURN
ENDIF
  
h5_id=H5F_OPEN(h5infile)	    	    	    ;;OPEN the HDF5 file
;
;Reading of metadata from dataset (scan) group in file.
nscan=0     ;FIND number of datasets
for i=0, H5G_GET_NUM_OBJS(h5_id)-1 DO $
  IF 'dataset' eq STRMID(STRLOWCASE(H5G_GET_MEMBER_NAME(h5_id,'/',i)),0,7) THEN nscan+=1
;
IF Nscan le 0 OR Nscan ge NSCANX THEN $ 	    ;;CHECK this number
   MESSAGE,"Number of scan groups is out of range!"
;
IF KEYWORD_SET(scanid) THEN BEGIN

  IF scanid le 0 OR scanid gt Nscan THEN $ 	    ;;CHECK given scanid
     MESSAGE,'Selected scan group ('+STRTRIM(scanid)+') is out of range!'

  beginscan=scanid
  endscan=scanid
ENDIF ELSE BEGIN
  beginscan=1
  endscan=Nscan
ENDELSE
;
scandata=CREATE_STRUCT(type+"scan",Nscan)
;
FOR iscan=beginscan,endscan DO BEGIN
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
    IF read THEN BEGIN
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
      success=1L
      ;
    ENDIF  
    ;
    IF variable eq 2 THEN BEGIN
      read=1
    ENDIF
    ;
  ENDWHILE
  ;
  H5G_CLOSE,scan_id    	    ;;CLOSE the current scan
  ;
ENDFOR
;
;COPY file structure to 'in' for output 
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
H5F_CLOSE,h5_id	    	    	    	    	    ;;CLOSE the HDF5 file
;
IF success THEN BEGIN
IF KEYWORD_SET(scanid) THEN meta=meta[beginscan:endscan] ELSE $
 meta = [meta[0],meta[beginscan:endscan]]
 
ENDIF ELSE BEGIN
  meta=0L
  scandata=0L
ENDELSE 
;meta=meta[beginscan:endscan]
;
END
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
PRO vol2bird_hr_odimhdf5_read_cluttermap,h5cluttermap,cmmeta,cmscan
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
PRO vol2bird_hr_odimhdf5_write_profile,     $
h5outfile,in,	    	    	    	    $
success=success,    	    	    	    $
vmeta,zmeta,uzmeta,   	    	    	    $
vscan,zscan,uZscan,cellmap,vtex,	    $
hdata,udata,vdata,wdata,xdata,zdata,nndata, $
VTEXSCALE,VTEXOFFSET,NLEV,MISSING,imgflag
;
success=0
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
    zcaltxt=STRING(FORMAT='("GEO=",f0,"*PV+",f0)',zmeta[iscan].zscale,zmeta[iscan].zoffset)
   uzcaltxt=STRING(FORMAT='("GEO=",f0,"*PV+",f0)',uzmeta[iscan].zscale,uzmeta[iscan].zoffset)
    vtextxt=STRING(FORMAT='("GEO=",f0,"*PV+",f0)',VTEXSCALE,VTEXOFFSET)
    celltxt=STRING(FORMAT='("GEO=",f0,"*PV+",f0)',1,0.0)
    ;
    vcal={_NAME:'calibration_V_formulas',_TYPE:'ATTRIBUTE',_DATA:vcaltxt}
    zcal={_NAME:'calibration_Z_formulas',_TYPE:'ATTRIBUTE',_DATA:zcaltxt}
   uzcal={_NAME:'calibration_uZ_formulas',_TYPE:'ATTRIBUTE',_DATA:uzcaltxt}
    tcal={_NAME:'calibration_vtex_formulas',_TYPE:'ATTRIBUTE',_DATA:vtextxt}
   cmcal={_NAME:'calibration_cellmap_formulas',_TYPE:'ATTRIBUTE',_DATA:celltxt}
     ;     
    calibration={_NAME:'calibration',_TYPE:'GROUP',$
                 ATTR1:vcal,ATTR2:zcal,ATTR3:uzcal,ATTR4:tcal,$
		 ATTR5:cmcal}
     ;
     ;CREATE the data structures
     scanvdata={_NAME:'scan_V_data',_TYPE:'Dataset',_DATA:vscan.(iscan),    $
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
    "scan_Z_data"  ,scanzdata, 	    $
;    "scan_uZ_data" ,scanuZdata,	    $
    "scan_vtex_data",vtexdata,	    $
    "scan_cellmap_data",cellmdata)
    ;
    out=CREATE_STRUCT(out,scanname,scan)
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
