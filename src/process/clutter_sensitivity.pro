;
;CLUTTER_SENSITIVITY. Extract Bird density profiles from European weather radar data
;for 1 Sept 2011 using different cluttermaps
;
cluttermap=1
;SET the clutter definition, this can be 'mode', 'median' or a percentage [0-100]
clutter_ids = [STRING([0,10,25,50,75]),['mode','median']]
;
;SET the radar ids of the radars to be processed
radar_ids = ['CZ']
radar_ids = radar_names(radar_ids,/print)
;
;SET the location of the cluttermaps and the statitics files
temp_dir='/nobackup_1/users/graafdem/DATA/FLYSAFE2/sens_clutter/'
;
; SPECIFY the time interval to process.
;
day0 = 01
month0 = 09
year0 = 2011
;
day1 = 01
month1 = 09
year1 = 2011
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;Definition of standard parameters.                                          ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
RESOLVE_ROUTINE, 'common_definition', /compile_full_file
common_definition
;
COMMON constants,$
NSCANX,RADIUS43,HLAYER,NLAYER,NDATA,RANGMIN,RANGMINSTDEV,   	$
RANGMAXSTDEV,RANGMAX,AZIMMIN,AZIMMAX,VRADMIN,NGAPBIN,NGAPMIN,	$
NDBZMIN,VDIFMAX,VMASKMAX,EMASKMAX,RHOMIN,ZDRMIN,DBZMIN,     	$
DBZMAX,DBZNOISE,DBZRAIN,DBZCELL,STDEVCELL,AREACELL,CLUTPERCCELL,$
NEIGHBOURS,VTEXSCALE,VTEXOFFSET,STDEVSCALE,NTEXBINAZIM,     	$
NTEXBINRANG,NTEXMIN,TEXCV,TEXSTDEV,DBZCLUTTER,DBZFACTOR,    	$
SIGMABIRD,STDEVBIRD,XOFFSET,XSCALE,XMEAN   
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;GET the radar definitions from the definitions structure
;
IF KEYWORD_SET(definitions) THEN dumy=TEMPORARY(definitions)
FOR i=0,N_ELEMENTS(radar_ids)-1 DO BEGIN 
  radar_definitions,radar_ids[i],radar_definition
  definitions= KEYWORD_SET(definitions) ? [definitions,radar_definition] : radar_definition
ENDFOR
;
grand_total_files=0
grand_total_systems=0
;
SPAWN,"date +%A', '%e' '%B' '%Y,' '%R:%S", todate
PRINT,'============================================================================
PRINT,'                  CLUTTER SENSITIVITY
PRINT,'        '+todate
PRINT,'============================================================================

;START time logging
t0=SYSTIME(1)
;
;GET the date information
date0 = STRING(format = '(2(I02),I4)', day0, month0, year0)
date1 = STRING(format = '(2(I02),I4)', day1, month1, year1)
printdate0 = STRING(format = '(2(I02,"-"),I4)', day0, month0, year0)
printdate1 = STRING(format = '(2(I02,"-"),I4)', day1, month1, year1)
;GET total number of days
days = JULDAY(month1, day1, year1) - JULDAY(month0, day0, year0) + 1
;radar_ids = ['NL']

; Loop over the requested radar systems
;
FOR iradar=0,N_ELEMENTS(radar_ids)-1 DO BEGIN 
  ;
  this_rd=definitions[iradar]
  ;
  ;COMPILE all necessary routines
  ;
  compile_iofile_command=FILE_BASENAME(this_rd.io_file,'.pro')
  RESOLVE_ROUTINE,compile_iofile_command,/compile_full_file
  RESOLVE_ROUTINE,'bird_call',/compile_full_file
  readcm_command      =compile_iofile_command+'_read_cluttermap'
  ;
  ; FIND all files corresponding to the requested days for this system.
  ;
  files=['']
  firstday=0
  lastday =0
  FOR d = 0, days - 1 DO BEGIN
    ;
    ;LOOP through the requested days, see if files exist
    CALDAT, JULDAY(month0, day0, year0) + d, month, day, year
    date = STRING(format = '(I04,2(I02))', year,month,day)
    search_string = STRING(FORMAT = '(a,a,"/",i04,2(i02),"/*")',$
                   this_rd.input_data_path,this_rd.path_id,year,month,day)
    ;
    thisday_files = FILE_SEARCH(search_string,count=nfiles)
    ;
    ;ADD found files to files array
    IF nfiles ne 0 THEN BEGIN
      ;
      IF SIZE(firstday,/TYPE) ne 7 THEN firstday=date	;first day with files
      lastday=date  	    	    	    	    	;last day with files
      ;
      files=[files,thisday_files]   	    	    	;CONCATENATE files array
      ;
    ENDIF
    ;
  ENDFOR
  IF N_ELEMENTS(files) le 1 THEN BEGIN
    directory = STRING(FORMAT = '(a,a,"/")',$
                   this_rd.input_data_path,this_rd.path_id)
    IF files eq '' THEN BEGIN 
      MESSAGE, 'No files found in '+directory+' for '+printdate0+' to '+printdate1
    ENDIF
  ENDIF
  files=files[1:N_ELEMENTS(files)-1]
  ;
  ;PRINT some info over the found files  
  yy0 = FIX(STRMID(firstday,0,4))
  mm0 = FIX(STRMID(firstday,4,2))
  dd0 = FIX(STRMID(firstday,6,2))
  ;  
  yy1 = FIX(STRMID(lastday,0,4))
  mm1 = FIX(STRMID(lastday,4,2))
  dd1 = FIX(STRMID(lastday,6,2))
  ;
  totaldays = JULDAY(mm1, dd1, yy1) - JULDAY(mm0, dd0, yy0) + 1
  printtotaldate0 = STRING(format = '(2(I02,"-"),I4)', dd0, mm0, yy0)
  printtotaldate1 = STRING(format = '(2(I02,"-"),I4)', dd1, mm1, yy1)
  totaldate0 = STRING(format = '(2(I02),I4)', dd0, mm0, yy0)
  totaldate1 = STRING(format = '(2(I02),I4)', dd1, mm1, yy1)
  ;
  SPAWN,"date +%A', '%e' '%B' '%Y,' '%R:%S': '", todate
  PRINT
  PRINT, todate+'Computing bird profiles for '+this_rd.radar_full_name
  PRINT, FORMAT='("Looking for files for days ",a," to ",a,",")',printdate0,printdate1
  ;
  plural = N_ELEMENTS(totaldays) ne 1 ? 'ies' : 'y'
  message=STRING(FORMAT='("Found ",i0," director",a," in ",a,"/, ")',$
  totaldays,plural,this_rd.input_data_path+this_rd.path_id)
  print, message
  ;
  IF totaldays gt 1 THEN BEGIN
    message=STRING(FORMAT='("totalling ",i0," radar volume files from ",a," to ",a,".")',$
    N_ELEMENTS(files),printtotaldate0,printtotaldate1)
    stats_date=totaldate0+'_'+totaldate1
  ENDIF ELSE BEGIN
    message=STRING(FORMAT='("totalling ",i0," radar volume files on ",a,".")',$
    N_ELEMENTS(files),printtotaldate0)
    stats_date=totaldate0
  ENDELSE
  print,message
  ;
  IF files[0] eq '' THEN BEGIN
    MESSAGE, 'No '+this_rd.radar_id+' files found in '+this_rd.input_data_path+this_rd.path_id
  ENDIF
  ;
  ;CREATE filenames
  get_datetime_command=compile_iofile_command+'_get_datetime'
  CALL_PROCEDURE,get_datetime_command,files,thedates,thetimes
  ;
  h5_infiles=FILE_BASENAME(files)	    	    	    	    	;FIND file_basename
  h5_outfiles = this_rd.bird_data_path+this_rd.path_id+'/'+thedates+'/RAD_'+this_rd.path_id+'_PRF_'+thedates+thetimes+'.h5'
  ;
  t1=SYSTIME(1) ;TRACK time for this system
  ;
  IF cluttermap THEN BEGIN
    ;
    ;SET the clutterfile to be processed
    IF N_ELEMENTS(clutter_ids) lt 1 THEN MESSAGE, 'No cluttermap definition is set. Call with "clutter_id = mode|median|<percentage [0-100]>"'
    ;
    FOR iclutter =0,N_ELEMENTS(clutter_ids)-1 DO BEGIN
      ;
      clutter_id=clutter_ids[iclutter]
      ;
      ;PROCESS clutter_id in terms of percentages
      error_message="Usage: clutter_id = 'mode'|'median'|<percentage>[0-100]"
      IF SIZE(clutter_id,/TYPE) gt 0 and SIZE(clutter_id,/TYPE) lt 7 THEN BEGIN
	IF clutter_id ge 0 and clutter_id le 100 THEN   	    $
	   clutter_definition = 'PRC'+STRTRIM(clutter_id,2)     $
	   ELSE MESSAGE, error_message+ ' clutter_id is : '+STRTRIM(clutter_id,2)
      ENDIF ELSE BEGIN
	IF SIZE(clutter_id,/TYPE) eq 7 THEN BEGIN
	  CASE STRUPCASE(STRTRIM(clutter_id,2)) of 
	   '0' : clutter_definition = 'PRC0'
	   '10' : clutter_definition = 'PRC10'
	   '25' : clutter_definition = 'PRC25'
	   '50' : clutter_definition = 'PRC50'
	   '75' : clutter_definition = 'PRC75'
	   'MODE' : clutter_definition = 'MODE'
	   'MEDIAN' : clutter_definition = 'MEDIAN'
	   ELSE :  MESSAGE, error_message
	  ENDCASE
	ENDIF ELSE MESSAGE, error_message
      ENDELSE
      ;
      clutter_map_search=this_rd.clutter_data_path+this_rd.path_id+'/*'+clutter_definition+'*'
      clutter_map = FILE_SEARCH(clutter_map_search,count=nclut)
      ;
      IF nclut eq 0 THEN MESSAGE, 'Cluttermap '+clutter_map_search+' not found'
      ;
      ;TEST
      compile_file_command=FILE_BASENAME(definitions[iradar].io_file,'.pro')
      CALL_PROCEDURE,readcm_command,clutter_map,cmmeta,cmscan
      ;
      ;CREATE (new) outfile names
      h5_outfiles = temp_dir+STRTRIM(clutter_id,2)+'/'+this_rd.path_id+'/'+thedates+'/'+FILE_BASENAME(h5_outfiles)
      ;
      ;CREATE a txtout filename
      txtoutfile = temp_dir+STRTRIM(clutter_id,2)+'/'+this_rd.path_id+'/'+thedates[0]+'/'+this_rd.path_id+'_'+thedates[0]+thetimes[0]+'.DAT'
      IF FILE_TEST(txtoutfile) THEN FILE_DELETE,txtoutfile
      ;      
      ;FOR each cluttermap, forget the processed files
      dummy=TEMPORARY(datetime_names)
      ;
      FOR ifile=0,N_ELEMENTS(files)-1 DO BEGIN
;      FOR ifile=0,1 DO BEGIN
	;
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;;OPEN file and READ the uncorrected Z
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;  
	SPAWN,"date +%A', '%e' '%B' '%Y,' '%R:%S': '", todate  
	;
	;CREATE A MANUAL STOP PROCEDURE:
	;STOP here if file "STOP" exists
	IF file_test('STOP') THEN BEGIN
	  print
	  STOP,todate+" *** USER STOP *** Remove file STOP and type .c to continue                   "
	ENDIF
	;
	;FIRST check if the current datetime has not been processed before, 
	;due to other files with the same date/time but different data (happens
	;for some countries)
	continue_files=0
	IF KEYWORD_SET(datetime_names) THEN BEGIN
	  FOR idt=0,N_ELEMENTS(datetime_names)-1 DO BEGIN
	    IF STRPOS(FILE_BASENAME(files[ifile]),datetime_names[idt]) ne -1 THEN BEGIN
	      ;SKIP the file if this date/time has already been processed
	      message=STRING(FORMAT='(a,"  Skipping HDF5 radar input file [",i0,"/",i0,"] ",a)',$
        		 todate,ifile+1,N_ELEMENTS(files),FILE_BASENAME(files[ifile]))
	      PRINT, FORMAT='(a%"\r",$)', message 

	      continue_files=1    ;break out of the files loop
	      CONTINUE    	    ;this breaks out of the datetime_names loop
	    ENDIF
	  ENDFOR 
	ENDIF
	IF continue_files THEN CONTINUE
	;
	;OPEN the file
	message=STRING(FORMAT='(a,"Processing HDF5 radar input file [",i0,"/",i0,"] ",a)',$
        	   todate,ifile+1,N_ELEMENTS(files),FILE_BASENAME(files[ifile]))
	PRINT, FORMAT='("#",a%"\r",$)', message 
	;
	IF ifile eq 0 THEN PRINT  ;do not overprint the first opened file message 
	;  
	dummy=TEMPORARY(processed_datetime)
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;CALL the bird profile algorithm
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
	bird_call,files[ifile],h5_outfiles[ifile],  $
	definitions=this_rd,img=1,  	    	    $
	cm=clutter_map,     	    	    	    $
        txtoutfile=txtoutfile,	    	    	    $
	success=success,    	    	    	    $
	processed_datetime=processed_datetime
	;
	IF success eq 0 THEN CONTINUE
	
	;REPACK and zip output file 
	SPAWN, 'h5repack --filter=GZIP=9 '+h5_outfiles[ifile]+' '+h5_outfiles[ifile]+'.zip'
	SPAWN, 'mv '+h5_outfiles[ifile]+'.zip '+h5_outfiles[ifile]
	;
	;REMEMBER the processed files
	IF KEYWORD_SET(datetime_names) THEN $
	  datetime_names = [datetime_names,processed_datetime] ELSE $
	  datetime_names = processed_datetime
	;
	;FREE as much memory as possible under IDL
	H5_CLOSE
	;
      ENDFOR	;files
      ;
    ENDFOR ;clutter ids
    ;
  ENDIF ;cluttermap
  ;
  ;
  ;PRINT info 
  plural=FLOOR((SYSTIME(1)-t1) / 86400) ne 1 ? 's,' : ', '
  loop_end=STRING(FORMAT='(A,A,A, i0," h,",i02," min,",i02," s. ")',$
  'Finished bird profiles for ',this_rd.radar_full_name,' in ',$
  (SYSTIME(1)-t1)/3600,$
  (SYSTIME(1)-t1) mod 3600 / 60,$
  (SYSTIME(1)-t1) mod 3600 mod 60)
  ;
  SPAWN,"date +%A', '%e' '%B' '%Y,' '%R:%S': '", todate
  PRINT,todate+loop_end    	    ;Print to screen       
  ;
  ;STOTE some stats
  grand_total_files += N_ELEMENTS(files)
  grand_total_systems += 1
  ;
ENDFOR ;radar ids
;
;PRINT info for entire program
;
SPAWN,"date +%A', '%e' '%B' '%Y,' '%R:%S': '", todate
PRINT,FORMAT='(a,"Total number of processed files: ",i0)',todate, grand_total_files
PRINT,FORMAT='(a,"Total number of processed systems: ",i0)',todate, grand_total_systems
plural=FLOOR((SYSTIME(1)-t0) / 86400) ne 1 ? 's,' : ', '
loop_end=STRING(FORMAT='(a,"Total time:",i2," day",a2," ",i02,":",i02,":",i02)',$
todate,$
(SYSTIME(1)-t0)/86400,$
plural,$
(SYSTIME(1)-t0) mod 86400/3600,$
(SYSTIME(1)-t0) mod 3600 / 60,$
(SYSTIME(1)-t0) mod 3600 mod 60)
PRINT,loop_end    	    ;Print to screen       

SPAWN,"date +%A', '%e' '%B' '%Y,' '%R:%S': '", todate
PRINT,todate+'Finished CLUTTER SENSITIVITY'
exit
END
