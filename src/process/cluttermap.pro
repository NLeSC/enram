;
;CLUTTERMAP.PRO
;
;CREATE cluttermaps from statistics files for different radar systems
;
;SET the radar ids of the radars to be processed
radar_ids = ['NLDBL']
radar_ids = radar_names(radar_ids)
;
;SET the location of the statitics files
stats_dir = GETENV('IDL_ENRAM_STATISTICS')
;
;SET the location of the cluttermaps
cluts_dir = GETENV('IDL_ENRAM_CLUTTERMAPS')
;
;SET the selection criterion (mode precedes percentage, median precedes mode)
;mode=1
;median=0
;percentage=45
perc = [0,5,10,20,25,50,75,-1,-2]
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
;GET the radar definitions from the definitions structure
;
IF KEYWORD_SET(definitions) THEN dumy=TEMPORARY(definitions)
FOR i=0,N_ELEMENTS(radar_ids)-1 DO BEGIN
  radar_definitions,radar_ids[i],radar_definition
  definitions= KEYWORD_SET(definitions) ? [definitions,radar_definition] : radar_definition
ENDFOR
;
SPAWN,"date +%A', '%e' '%B' '%Y,' '%R:%S", todate
PRINT,'============================================================================
PRINT,'                       CLUTTERMAP
PRINT,'          '+todate
PRINT,'============================================================================

;START time logging
t0=SYSTIME(1)
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
  ;FIND statistics file
  stats_file_name=FILE_SEARCH(stats_dir+this_rd.PATH_ID+'/*.h5')
  ;
  IF N_ELEMENTS(stats_file_name) gt 1 THEN BEGIN
    PRINT, STRTRIM(N_ELEMENTS(stats_file_name),2)+' files were found. Select one before proceeding'
    PRINT, stats_file_name
    STOP
  ENDIF
  ;
  ;CREATE maps for various percentages, and mode and median
  ;
  refresh = 1           ;MAKE sure the input array is erased fro each new radar system
  FOR iperc = 0,N_ELEMENTS(perc)-1 DO BEGIN
    ;
    percentage = perc[iperc]
    mode=0
    median=0
    IF percentage eq -1 THEN BEGIN
     mode = 1
     median = 0
    ENDIF
    IF percentage eq -2 THEN BEGIN
     mode = 0
     median = 1
    ENDIF
    ;
    ;CREATE a cluttermap filename
    clutfile=FILE_BASENAME(stats_file_name,'.h5')
    stspos=STRPOS(clutfile,'STS')
    STRPUT,clutfile,'CLT',stspos
    ;
    extention = 'PRC'+STRTRIM(percentage,2)
    extention = KEYWORD_SET(mode) ? 'MODE' : extention
    extention = KEYWORD_SET(median) ? 'MEDIAN' : extention
    clutfile+='_'+extention+'.h5'
    ;
    clutter_file_name=cluts_dir+this_rd.PATH_ID+'/'+clutfile
    make_cluttermap,stats_file_name,clutter_file_name,in,refresh=refresh,mode=mode,median=median,percentage=percentage
    ;
    ;DO not re-read the statistic file for the same radar system
    refresh = 0
    ;
  ENDFOR

ENDFOR

END
