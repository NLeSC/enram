;
;HISTROGRAM.PRO
;
;COMPUTE statistics of radar volume files for the creation of cluttermaps.
;
;SET the radar ids of the radars to be processed
radar_ids = ['SEKKR','SEKIR','SEHUD','SEASE','SEARL','SEANG','SKMAJ','SKKOH','SILIS']
radar_ids = radar_names(radar_ids,/print)
;
;SET the location of the cluttermaps and the statitics files
stats_dir='/nobackup_1/users/graafdem/DATA/FLYSAFE2/statistics/'
;
; SPECIFY the time interval to process.
;
day0 = 15
month0 = 08
year0 = 2011
;
day1 = 15
month1 = 09
year1 = 2011
;
;SET refresh if a new statistics file is to be cretaed. Otherwise the results 
;can be added to an existing statistics file
refresh=1
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
PRINT,'                       HISTOGRAM
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
;
; Loop over the requested radar systems
;
FOR iradar=0,N_ELEMENTS(radar_ids)-1 DO BEGIN 
  ;
  this_rd=definitions[iradar]
  ;
  ;COMPILE all necessary routines for this system
  ;
  compile_iofile_command=FILE_BASENAME(this_rd.io_file,'.pro')
  RESOLVE_ROUTINE,compile_iofile_command,/compile_full_file
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
  PRINT, todate+'Computing statistics for '+this_rd.radar_full_name
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
  ;DEFINE a name for the statistics file to be created.  
  stats_file_name=stats_dir+this_rd.PATH_ID+'/'
  FILE_MKDIR, stats_file_name
  stats_file_name+='RAD_'+this_rd.PATH_ID+'_STS_'+stats_date+'.h5'
  ;
  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  ; OPEN all files and ADD all values to the histogram
  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  ;
  t1=SYSTIME(1) ;TRACK time for this system
  ;
  ;CALL statistics
  statistics,files,stats_file_name,definition=this_rd,refresh=refresh
  ;
  ;FREE as much memory as possible under IDL
  H5_CLOSE
  ;
  ;PRINT info over the statitics loop
  plural=FLOOR((SYSTIME(1)-t1) / 86400) ne 1 ? 's,' : ', '
  loop_end=STRING(FORMAT='(A,A,A, i0," h,",i02," min,",i02," s. ")',$
  'Finished STATISTICS for ',this_rd.radar_full_name,' in ',$
  (SYSTIME(1)-t1)/3600,$
  (SYSTIME(1)-t1) mod 3600 / 60,$
  (SYSTIME(1)-t1) mod 3600 mod 60)
  ;
  SPAWN,"date +%A', '%e' '%B' '%Y,' '%R:%S': '", todate
  PRINT,todate+loop_end    	    ;Print to screen       
  ;
  ;STATE some stats
  grand_total_files += N_ELEMENTS(files)
  grand_total_systems += 1
  ;
ENDFOR
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
PRINT,todate+'Finished HISTOGRAM'

END
