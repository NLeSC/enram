;
;STATISTICS.PRO
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;COMPUTE statistics of radar volume file 
;
;REDISTRIBUTE all pixel values into the hist_data array, which is an 
;multi-dimensional array of 256 x nbins x nrays x nscan 
;[256,nrang,nazim,nscan]. Each pixel value adds one to its respective 
;histogram, in the right bin [0-255], according to its image value. This 
;is repeated for all avaliable volume files, each time adding values of 
;each pixels to its respective histogram: The histograms are filled
;using multiple files, each pixel having it own histogram (in time).
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;INPUT 
;
;FILES   	    =   Names of the radar volume file [STRING ARRAY]
;STATS_FILE_NAME    =	Name of the output file
;
;KEYWORDS
;definition	    =	Radar definition structure
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
PRO statistics,     	    $
files,h5_statsfile,	    $
definition=definition,	    $
uncorrected=uncorrected,    $
refresh=refresh
;
AZIMAX=360  	;ALWAYS create and load AZIMAX azimuth bins. Skip bins in volume 
    	    	;files with more than AZIMAX bins. Fill bins with zeros for 
		;files that have less than AZIMAX bins
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;INITIALISE
uncorrected = 1 ;If present, load the uncorrected Z relf.(TH), otherwise load 
    	    	;corrected Z (DBZH) and compute statistics over that. In that 
		;case return uncorrected=0 to caller program

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; Analyse the radar definitions structure
compile_file_command=FILE_BASENAME(definition.io_file,'.pro')
readscan_command    =compile_file_command+'_read_scan'
;
;CHECK wether stats file exists
file_exist = FILE_TEST(h5_statsfile) ? 1 : 0 
file_names=FILE_BASENAME(files[0])
;
;CHECK if a stats_file needs updating or refreshing
open_stats_file = not KEYWORD_SET(refresh) and file_exist ? 1 : 0
IF open_stats_file THEN BEGIN
  ;
  ;IF stats_file_name exists and no refresh is requested, OPEN this file 
  ;and ADD results to this file
  ;
  h5_id=H5F_OPEN(h5_statsfile)    	;..OPEN the stats file
  
  PRINT, 'Reading statistics file '+ FILE_BASENAME(h5_statsfile)
  ;
  ;READ the groups 
  gid = H5G_OPEN(h5_id, '/')
  out = H5_PARSE(gid,'/' ,/READ_DATA)
  ;
ENDIF ELSE BEGIN 
  ;
  ;START a new histogram and CREATE a stats_file
  SPAWN,"date +%A', '%e' '%B' '%Y,' '%R:%S': '", todate
  PRINT,todate+'Creating statistics file '+ FILE_BASENAME(h5_statsfile)
  ;
  ;OPEN the first file and use the structure of this file to 
  ;base the histogram on
  ;
  CALL_PROCEDURE,readscan_command,files[0],'U',meta,scan,in,/img,success=uzflag
  ;
  IF uzflag eq 0 THEN BEGIN 
    PRINT,"Uncorrected Z reflectances are not present in file "+files[0]
    uncorrected=0
    ;
    ;RETRY for corrected Z
    CALL_PROCEDURE,readscan_command,files[0],'Z',meta,scan,success=zflag
    ;
    IF zflag eq 0 THEN $
      MESSAGE, "This is useless: both TH and DBZH are not present in "+files[0]
    ;
  ENDIF
  ;
  ;CREATE an output file
  out=CREATE_STRUCT(  	    	    	    $
  "_NAME"         ,h5_statsfile, 	    $
  "_ICONTYPE"     ,"hdf",     	    	    $
  "_TYPE"         ,"GROUP",   	    	    $
  "_FILE"         ,h5_statsfile, 	    $
  "_PATH"         ,"/",     	    	    $
  "_COMMENT"      ,"(IDL) Statistics file", $
  "what"          ,in.what,    	    	    $
  "where"         ,in.where)
  ;
  ;LOOP over the available vertical scans
  nscan=scan.(0)
  FOR iscan=1,nscan DO BEGIN
    ;
    ;NAME dataset according to scan number
    dataset_name=STRING(FORMAT='("dataset",i0)',iscan)
    ;
    ;ACCESS the volume data
    data=scan.(iscan)
    nrang=meta[iscan].nrang 	;GET the dimensions 
    nazim=meta[iscan].nazim 	;of the volume data
    ;
    ;CREATE a new dataset structure to hold the hist_data
    dataset = {_NAME:dataset_name,_TYPE:"GROUP"}
    ;
    ;CREATE a HISTOGRAM data group
    hist_data = BYTARR(256,nrang,AZIMAX)    	;...CREATE a new array 
    histset_name = 'data1'

    hist_name = 'hist_data'
    hist_offset = meta[iscan].ZOFFSET
    hist_gain   = meta[iscan].ZSCALE
    hist_nodata = 255.0
    hist_undetect = 0.0
    hist_basequantity = uncorrected ? 'TH' : 'DBZH'
    ;
    histogramdata={_NAME:'data',_TYPE:'Dataset',_DATA:hist_data,     	$
    	    CLASS:{_NAME:'CLASS',_TYPE:'ATTRIBUTE',_DATA:'IMAGE'},   	$
    IMAGE_VERSION:{_NAME:'IMAGE_VERSION',_TYPE:'ATTRIBUTE',_DATA:'1.2'}}
    ;
    ;CREATE a 'what' description group
    what = {_NAME:'what',_TYPE:"GROUP",$
	    quantity:{_NAME:'quantity',_TYPE:'ATTRIBUTE',_DATA:hist_name},$
	    gain:{_NAME:'gain',_TYPE:'ATTRIBUTE',_DATA:hist_gain},$
	    offset:{_NAME:'offset',_TYPE:'ATTRIBUTE',_DATA:hist_offset},$
   	    nodata:{_NAME:'nodata',_TYPE:'ATTRIBUTE',_DATA:hist_nodata},$
   	    undetect:{_NAME:'undetect',_TYPE:'ATTRIBUTE',_DATA:hist_undetect},$
   	    basequantity:{_NAME:'basequantity',_TYPE:'ATTRIBUTE',_DATA:hist_basequantity}$
	    }
    histdata_group = {_NAME:histset_name,_TYPE:"GROUP",DATA:histogramdata,WHAT:what}
    ;
    ;CREATE a 'MEAN' group
    mean_data = FLTARR(nrang,AZIMAX)    	;...CREATE a new array 
    meanset_name = 'data2'
    meandata_name = 'mean'
    ;
    Nfiles = 0L
    ;
    meandata={_NAME:'data',_TYPE:'Dataset',_DATA:mean_data,     	$
    	    CLASS:{_NAME:'CLASS',_TYPE:'ATTRIBUTE',_DATA:'FLOAT'}}
    ; H5_CREATE,h5_statsfile,out
    ;CREATE a 'what' description group
    what = {_NAME:'what',_TYPE:"GROUP",$
            Nfiles:{_NAME:'nFiles',_TYPE:'ATTRIBUTE',_DATA:Nfiles},  	$
	    quantity:{_NAME:'quantity',_TYPE:'ATTRIBUTE',_DATA:meandata_name}}
    meandata_group = {_NAME:meanset_name,_TYPE:"GROUP",DATA:meandata,WHAT:what}
    ;
    ;CREATE a 'STD' group
    stnd_data = FLTARR(nrang,AZIMAX)    	;...CREATE a new array 
    stndset_name = 'data3'
    stnddata_name = 'std'
    ;
    stnddata={_NAME:'data',_TYPE:'Dataset',_DATA:stnd_data,     	$
    	    CLASS:{_NAME:'CLASS',_TYPE:'ATTRIBUTE',_DATA:'FLOAT'}}
    ;
    ;CREATE a 'what' description group H5_CREATE,h5_statsfile,out
    what = {_NAME:'what',_TYPE:"GROUP",$
	    quantity:{_NAME:'quantity',_TYPE:'ATTRIBUTE',_DATA:stnddata_name}}
    stnddata_group = {_NAME:stndset_name,_TYPE:"GROUP",DATA:stnddata,WHAT:what}
    
    ;DUMP all in dataset structure
    dataset = CREATE_STRUCT(dataset,histset_name,histdata_group)
    dataset = CREATE_STRUCT(dataset,meanset_name,meandata_group)
    dataset = CREATE_STRUCT(dataset,stndset_name,stnddata_group)
    
    ;IF in.datasetXX.what exists THEN add the what group to datsetXX
    i_intag=WHERE(TAG_NAMES(in) eq STRUPCASE(dataset_name),ntags)
    IF ntags eq 1 THEN BEGIN
      i_settag=WHERE(TAG_NAMES(in.(i_intag)) eq "WHAT",ntags)
      IF ntags eq 1 THEN dataset = CREATE_STRUCT(dataset,"what",in.(i_intag).(i_settag))
      i_settag=WHERE(TAG_NAMES(in.(i_intag)) eq "WHERE",ntags)
      IF ntags eq 1 THEN dataset = CREATE_STRUCT(dataset,"where",in.(i_intag).(i_settag))
    ENDIF
    ;
    ;ADD dataset to out structure
    out=CREATE_STRUCT(out,dataset_name,dataset)
    ;
  ENDFOR  
  ;IMAGE
  ;FILE_DELETE,h5_statsfile,/ALLOW_NONEXISTENT	;H5_CREATE should overwrite the file, but somehow it doesn't
  ;CREATE an (empty) stats file
  ;H5_CREATE,h5_statsfile,out
  ;
ENDELSE
;
;LOOP over the volume files
FOR ifile=0,N_ELEMENTS(files)-1 DO BEGIN
  ;
  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  ;;OPEN file and READ the uncorrected Z
  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  ;  
  SPAWN,"date +%A', '%e' '%B' '%Y,' '%R:%S': '", todate  
  ;
  ;CREATE A MANUAL STOP PROCEDURE:
  ;STOP here if file "STOP" exists
  IF FILE_TEST('STOP') THEN BEGIN
    PRINT
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
  PRINT, FORMAT='(a%"\r",$)', message 
  ;
  IF ifile eq 0 THEN PRINT  ;do not overprint the first opened file message 
  ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;skip_everything=0
if keyword_set(skip_everything) then begin
 processed_datetime=STRMID(FILE_BASENAME(files[ifile]),11,12)
 ;remember the processed files
 IF KEYWORD_SET(datetime_names) THEN $
   datetime_names = [datetime_names,processed_datetime] ELSE $
   datetime_names = processed_datetime
 continue
endif
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  ;  
  dummy=TEMPORARY(processed_datetime)
  CASE uncorrected of 
    0: CALL_PROCEDURE,readscan_command,files[ifile],'Z',meta,scan,success=uzflag,processed_datetime=processed_datetime
    1: CALL_PROCEDURE,readscan_command,files[ifile],'U',meta,scan,success=uzflag,processed_datetime=processed_datetime
    ELSE: MESSAGE, 'uncorrected flag should be 0 or 1, please check'
  ENDCASE
  ;
  ;REMEMBER the processed files
  IF KEYWORD_SET(datetime_names) THEN $
    datetime_names = [datetime_names,processed_datetime] ELSE $
    datetime_names = processed_datetime
  ;
  ;IF something is wrong, quarantine the file, mention this, and move on
  IF uzflag eq 0 THEN BEGIN
    desired_product = uncorrected ? 'TH' : 'DBZH'
    thisfile=files[ifile]
    thisdir= FILE_DIRNAME(thisfile,/MARK_DIRECTORY)
    SPAWN,"date +%A', '%e' '%B' '%Y,' '%R:%S': '", todate  
    message=STRING(FORMAT='(a,a," is not available, quarantining file ",a)',$
               todate,desired_product,FILE_BASENAME(files[ifile]))
    IF FILE_TEST(thisfile) THEN BEGIN	;it can already be quarantined by 
                                        ; io_routine. In that case just move on
      PRINT
      PRINT,message
      SPAWN, 'mv '+thisfile+' '+thisdir+'quarantine_'+FILE_BASENAME(thisfile)
    ENDIF
    CONTINUE
  ENDIF  
  ;
  file_names = [file_names,FILE_BASENAME(files[ifile])]
  ;
  ;LOOP over the available vertical scans
  nscan=scan.(0)
  FOR iscan=1,nscan DO BEGIN
    ;
    IF meta[iscan].nrang eq 0 THEN CONTINUE
    ;
    ;NAME dataset according to scan number
    dataset_name=STRING(FORMAT='("dataset",i0)',iscan)
    ;
    ;FIND the field in the out structure corresponding to dataset_name
    i_outtag=WHERE(TAG_NAMES(out) eq STRUPCASE(dataset_name),ntags)
    ;
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;    
    ;IF it happens that a new scan is available, create the dataset in 'out' 
    ;to hold the statistics for this scan
    IF ntags eq 0 THEN BEGIN
      ;
      ;NAME dataset according to scan number
      dataset_name=STRING(FORMAT='("dataset",i0)',iscan)
      ;
      ;ACCESS the volume data
      data=scan.(iscan)
      nrang=meta[iscan].nrang 	;GET the dimensions 
      nazim=meta[iscan].nazim 	;of the volume data
      ;
      ;CREATE a new dataset structure to hold the hist_data
      dataset = {_NAME:dataset_name,_TYPE:"GROUP"}
      ;
      ;CREATE a HISTOGRAM data group
      hist_data = BYTARR(256,nrang,AZIMAX)    	;...CREATE a new array 
      histset_name = 'data1'

      hist_name = 'hist_data'
      hist_offset = meta[iscan].ZOFFSET
      hist_gain   = meta[iscan].ZSCALE
      hist_nodata = 255.0
      hist_undetect = 0.0
      hist_basequantity = uncorrected ? 'TH' : 'DBZH'
      ;
      histogramdata={_NAME:'data',_TYPE:'Dataset',_DATA:hist_data,     	$
    	      CLASS:{_NAME:'CLASS',_TYPE:'ATTRIBUTE',_DATA:'IMAGE'},   	$
      IMAGE_VERSION:{_NAME:'IMAGE_VERSION',_TYPE:'ATTRIBUTE',_DATA:'1.2'}}
      ;
      ;CREATE a 'what' description group
      what = {_NAME:'what',_TYPE:"GROUP",$
	      quantity:{_NAME:'quantity',_TYPE:'ATTRIBUTE',_DATA:hist_name},$
	      gain:{_NAME:'gain',_TYPE:'ATTRIBUTE',_DATA:hist_gain},$
	      offset:{_NAME:'offset',_TYPE:'ATTRIBUTE',_DATA:hist_offset},$
   	      nodata:{_NAME:'nodata',_TYPE:'ATTRIBUTE',_DATA:hist_nodata},$
   	      undetect:{_NAME:'undetect',_TYPE:'ATTRIBUTE',_DATA:hist_undetect},$
   	      basequantity:{_NAME:'basequantity',_TYPE:'ATTRIBUTE',_DATA:hist_basequantity}$
	      }
      histdata_group = {_NAME:histset_name,_TYPE:"GROUP",DATA:histogramdata,WHAT:what}
      ;
      ;CREATE a 'MEAN' group
      mean_data = FLTARR(nrang,AZIMAX)    	;...CREATE a new array 
      meanset_name = 'data2'
      meandata_name = 'mean'
      ;
      Nfiles = 0L
      ;
      meandata={_NAME:'data',_TYPE:'Dataset',_DATA:mean_data,     	$
    	      CLASS:{_NAME:'CLASS',_TYPE:'ATTRIBUTE',_DATA:'FLOAT'}}
      ; H5_CREATE,h5_statsfile,out
      ;CREATE a 'what' description group
      what = {_NAME:'what',_TYPE:"GROUP",$
              Nfiles:{_NAME:'nFiles',_TYPE:'ATTRIBUTE',_DATA:Nfiles},  	$
	      quantity:{_NAME:'quantity',_TYPE:'ATTRIBUTE',_DATA:meandata_name}}
      meandata_group = {_NAME:meanset_name,_TYPE:"GROUP",DATA:meandata,WHAT:what}
      ;
      ;CREATE a 'STD' group
      stnd_data = FLTARR(nrang,AZIMAX)    	;...CREATE a new array 
      stndset_name = 'data3'
      stnddata_name = 'std'
      ;
      stnddata={_NAME:'data',_TYPE:'Dataset',_DATA:stnd_data,     	$
    	      CLASS:{_NAME:'CLASS',_TYPE:'ATTRIBUTE',_DATA:'FLOAT'}}
      ;
      ;CREATE a 'what' description group H5_CREATE,h5_statsfile,out
      what = {_NAME:'what',_TYPE:"GROUP",$
	      quantity:{_NAME:'quantity',_TYPE:'ATTRIBUTE',_DATA:stnddata_name}}
      stnddata_group = {_NAME:stndset_name,_TYPE:"GROUP",DATA:stnddata,WHAT:what}

      ;DUMP all in dataset structure
      dataset = CREATE_STRUCT(dataset,histset_name,histdata_group)
      dataset = CREATE_STRUCT(dataset,meanset_name,meandata_group)
      dataset = CREATE_STRUCT(dataset,stndset_name,stnddata_group)

      ;IF in.datasetXX.what exists THEN add the what group to datsetXX
      i_intag=WHERE(TAG_NAMES(in) eq STRUPCASE(dataset_name),ntags)
      IF ntags eq 1 THEN BEGIN
	i_settag=WHERE(TAG_NAMES(in.(i_intag)) eq "WHAT",ntags)
	IF ntags eq 1 THEN dataset = CREATE_STRUCT(dataset,"what",in.(i_intag).(i_settag))
	i_settag=WHERE(TAG_NAMES(in.(i_intag)) eq "WHERE",ntags)
	IF ntags eq 1 THEN dataset = CREATE_STRUCT(dataset,"where",in.(i_intag).(i_settag))
      ENDIF
      ;
      ;ADD dataset to out structure
      out=CREATE_STRUCT(out,dataset_name,dataset)
      ;
      ;FIND the field again in the out structure corresponding to dataset_name
      i_outtag=WHERE(TAG_NAMES(out) eq STRUPCASE(dataset_name),ntags)
      ;
      IF ntags eq 0 THEN STOP
      ;
    ENDIF    
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;    
    ;    
    ;LOAD the statistics data for this scan
    hist_data = out.(i_outtag).data1.data._data
    mean_data = out.(i_outtag).data2.data._data
    stnd_data = out.(i_outtag).data3.data._data
    ;
    nFiles = out.(i_outtag).data2.what.nfiles._data
    stnd = (stnd_data^2 + mean_data^2) * nFiles
    mean = mean_data * nFiles

    IF not FINITE(MAX(stnd)) THEN STOP
    ;    
    ;ACCESS the volume data for this scan
    data=scan.(iscan)
    nrang=meta[iscan].nrang 	;GET the dimensions 
    nazim=meta[iscan].nazim 	;of the volume data
    ;
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ;ADD the current volume data to their respective histograms.
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ;
    ;GET the number of range bins from the stored stats file (this number is
    ;defined by the number of range bins in the first volume file, but is not 
    ;guarantueed to be stable over time/volume files)
    ;
    dimstat = SIZE(hist_data,/DIM)
    dimvol  = SIZE(data,/DIM)
    RANGMAX = dimstat[1]
    ;
    rangmin = dimstat[1] < dimvol[0]
    azimin = dimstat[2] < dimvol[1]
    ;
    ;INSTEAD of looping through the data array, 
    ;CREATE an indexing array to update hist_Data
    ;directly
    ;
    if 0 eq 1 THEN begin  
      
      hist_data_direct = hist_data
    
      i = BYTARR(RANGMAX,AZIMAX)
      i[0:rangmin-1,0:azimin-1] = data[0:rangmin-1,0:azimin-1]

      i=REFORM(i,RANGMAX*AZIMAX,/OVERWRITE)
      j=REFORM(INDGEN(RANGMAX) # (INTARR(AZIMAX)+1),RANGMAX*AZIMAX,/OVERWRITE)
      k=REFORM((INTARR(RANGMAX)+1) # INDGEN(AZIMAX),RANGMAX*AZIMAX,/OVERWRITE)

      indx = i + j * RANGMAX + k * RANGMAX*AZIMAX

      hist_data_direct[indx] += 1

    endif
    ;
    ;LOOP over ALL statistiscs dimensions
    FOR iazim = 0,AZIMAX-1 DO BEGIN 
      FOR irang = 0,RANGMAX-1 DO BEGIN
	 ;
	 IF iazim LT nazim and irang LT nrang THEN BEGIN 
           ;
           hist_data[data[irang,iazim],irang,iazim]+=1
	   mean[irang,iazim] += data[irang,iazim]
	   stnd[irang,iazim] += (data[irang,iazim]*1.0)^2
           ;
	 ENDIF ELSE BEGIN
           ;
	   ;ADD zeros for all statistics pixels that have no values in 
	   ;the current volume file (Should this be done? or just skip?)
           hist_data[0,irang,iazim]+=1
	   ;
	ENDELSE
        ;
      ENDFOR
    ENDFOR
    ;
    mean_data = mean / (nFiles+1)
    stnd_data = SQRT((stnd/(nFiles+1)) - mean_data^2)
    ;
    ;CORRECT for possible round-off errors
    IF not FINITE(MAX(stnd_data)) THEN BEGIN
      fnan = WHERE(FINITE(stnd_data) ne 1,nfnan)
      IF nfnan gt 0 THEN stnd_data[fnan]=0
    ENDIF
    ;
    ;UPDATE the output structure
    ;
    out.(i_outtag).data1.data._data = hist_data
    out.(i_outtag).data2.data._data = mean_data
    out.(i_outtag).data3.data._data = stnd_data
    ;
    out.(i_outtag).data2.what.nfiles._data = nFiles+1
    ;
  ENDFOR ;scans
  ;
ENDFOR ; files
dummy=check_math()
PRINT
;
;CREATE a 'HOW' group that contains the volume file filenames
file_names=H5T_STR_TO_VLEN(file_names)
namesdata={_NAME:'data',_TYPE:'Dataset',_DATA:file_names,     	$
      CLASS:{_NAME:'CLASS',_TYPE:'ATTRIBUTE',_DATA:'STRING'}}
how={_NAME:'how',_TYPE:"GROUP",DATA:namesdata}
;out=CREATE_STRUCT(out,'how',how)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;STORE the updated histograms in a file
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

SPAWN,"date +%A', '%e' '%B' '%Y,' '%R:%S': '", todate
print, todate+'Writing statistics file '+ h5_statsfile

FILE_DELETE,h5_statsfile,/ALLOW_NONEXISTENT	;H5_CREATE should overwrite the file, but somehow it doesn't
H5_CREATE,h5_statsfile,out
;
END
