;
;IDL routine to read hdf5 radar files in ODIM format from Sweden
;
;Author: M. de Graaf (KNMI)
;Date: April 2012
;

PRO vol2bird_se_odimhdf5
  ;
  ;Call  
  ;
  ; RESOLVE_ROUTINE, 'vol2bird_se_odimhdf5', /compile_full_file
  ;
  ; in the main file to compile all the hdf5 routines found in this file
  ; 
  ; All callable routines in this file are named the same as the 
  ; main routine with added names describing the function
  ; (e.g. vol2bird_se_odimhdf5_read_scan for the scan read procedure)
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
PRO vol2bird_se_odimhdf5_get_datetime,file,this_date,this_time
;
IF SIZE(file[0],/TYPE) ne 7 THEN BEGIN
  PRINT, 'please, provide a string (array) of filename(s)'
  stop
ENDIF
this_date=STRMID(FILE_BASENAME(file),6,8)
this_time=STRMID(FILE_BASENAME(file),15,6)
;
END
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
PRO vol2bird_se_odimhdf5_read_scan, 	$
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
IF FILE_TEST(h5infile) eq 0 THEN RETURN
h5_id=H5F_OPEN(h5infile)	    	    	    ;;OPEN the HDF5 file
;
vol2bird_se_odimhdf5_get_datetime,h5infile,this_date,this_time
thisdatetime=this_date+this_time
;
;COPY file structure to 'in' structure for output 
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
  H5G_CLOSE,gid
  ;
ENDIF
;
;Reading of metadata from dataset (scan) group in file.
nscan=0     ;FIND number of datasets
for i=0, H5G_GET_NUM_OBJS(h5_id)-1 DO $
  IF 'dataset' eq STRMID(STRLOWCASE(H5G_GET_MEMBER_NAME(h5_id,'/',i)),0,7) THEN nscan+=1
;
IF Nscan le 0 OR Nscan ge NSCANX THEN $ 	    ;;CHECK this number
   MESSAGE,"Number of scan groups is out of range!"
;
IF KEYWORD_SET(scanid) THEN BEGIN   	    	    ;IF scanid is passed then only read this scan
  ;
  IF scanid le 0 OR scanid gt Nscan THEN $ 	    ;;CHECK given scanid
     MESSAGE,'Selected scan group ('+STRTRIM(scanid)+') is out of range!'
  ;
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
  scan_id = H5G_OPEN(h5_id,STRING(FORMAT='("/dataset",i0)',iscan))
  ;
  where=H5_PARSE(h5_id,'where')
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
  ;
  numdata=0     ;FIND number of data groups in this scan/dataset
  for i=0, H5G_GET_NUM_OBJS(scan_id)-1 DO $
    IF 'data' eq STRMID(STRLOWCASE(H5G_GET_MEMBER_NAME(scan_id,'/dataset'+STRTRIM(iscan,2),i)),0,4) THEN numdata+=1
  ;
  ;OPEN numbered dataset?? groups and find correct /data field
  read=0
  variable=0
  ;
  ;LOOP through the data groups and find the requested one
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
    ;OPEN and READ the data group if it matches the requested data field.
    IF read THEN BEGIN
      ;
      object=STRING(FORMAT='("/dataset",i0,"/data",i0)',iscan,variable)
      scan_data=H5_PARSE(scan_id,object,/READ_DATA)
      ;
      ;CREATE an IDL structure that contains the data 
      ;from each scan, which may contain different array
      ;lengths. Each scan can be accessed by index, usidata_n=CREATE_STRUCT(data_n,"data"+idata,scan_data)ng
      ;the indexed structure format: scan2_data = scandata.(2)
      ;The number of scans is contained in the first field:
      ;number_of_scans = scandata.(0)
      scandata=CREATE_STRUCT(scandata,"scan"+STRTRIM(iscan,2),scan_data.DATA._DATA)	;;UPDATE the IDL structure
      success=1L
      ;
    ENDIF  
    ;
    ;IF imgflag is set read all the relevant variables
    IF KEYWORD_SET(imgflag) THEN BEGIN
      ;
      find_quantity=WHERE(STRUPCASE(quantity) eq ['TH','DBZH','VRAD'],bird_quantity)
      IF bird_quantity ne -1 THEN BEGIN
	;
	;IF this is not the quantity to read anyway, do it now for IN structure
	IF not read THEN BEGIN	    
	  object=STRING(FORMAT='("/dataset",i0,"/data",i0)',iscan,variable)
	  scan_data=H5_PARSE(scan_id,object,/READ_DATA)
	ENDIF 
	;
	;CREATE the data_n structure
	IF not KEYWORD_SET(data_n) THEN BEGIN
	  data_n=CREATE_STRUCT("data1",scan_data) 
	;OR update it  
	ENDIF ELSE BEGIN
	  idata=STRTRIM(N_ELEMENTS(TAG_NAMES(data_n))+1,2)  	;NUMBER consecutively 
	  data_n=CREATE_STRUCT(data_n,"data"+idata,scan_data)	;ADD structure
	ENDELSE  
	;
	;PREVENT breaking after reading the requested variable when /img is set
	read=0
      ENDIF	
      ;
    ENDIF      
    ;
    ;BREAK if no more data groups are present
    IF variable eq numdata THEN read=1
    ;
  ENDWHILE
  ;
  H5G_CLOSE,scan_id    	    ;;CLOSE the current scan
  ;
  IF KEYWORD_SET(imgflag) THEN BEGIN 

    ;ADD 'what' and 'where' groups for dataset
    data_n=CREATE_STRUCT(data_n,"what",what)
    data_n=CREATE_STRUCT(data_n,"where",where)
    ;
    in=CREATE_STRUCT(in,"dataset"+STRTRIM(iscan,2),data_n)
    dummy=TEMPORARY(data_n) 	;destroy data_n for next dataset
    ;
  ENDIF

ENDFOR
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
;IF this file was READ, return the datetime string, to avoid multiple passes
IF success THEN processed_datetime=thisdatetime
;
END
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
PRO vol2bird_se_odimhdf5_read_cluttermap,h5cluttermap,cmmeta,cmscan
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
PRO vol2bird_se_odimhdf5_write_profile,     $
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
"_COMMENT"      ,"IDL se vol2bird", $
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
    "_COMMENT"     ,"IDL se vol2bird")
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
