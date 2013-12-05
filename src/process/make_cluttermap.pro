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

;
;make_cluttermap.pro
;
;OPEN a statistics file and create a cluttermap based on the
;given or assumed percentile.
;
PRO make_cluttermap,h5_statsfile,   $
h5_clutterfile,in,  	    	    $
mode=mode,  	    	    	    $
median=median,	    	    	    $
percentage=percentage,	    	    $
refresh=refresh

IF KEYWORD_SET(refresh) THEN in=0
perc = N_ELEMENTS(percentage) eq 1 ? percentage : 0
;
;CHECK wether stats file exists
file_exist = FILE_TEST(h5_statsfile) ? 1 : 0 
IF not file_exist THEN MESSAGE, 'File '+h5_statsfile+' cannot be found.'
;
h5_id=H5F_OPEN(h5_statsfile)    	;..OPEN the stats file
;
t0=SYSTIME(1)
SPAWN,"date +%A', '%e' '%B' '%Y,' '%R:%S': '", todate
PRINT, FORMAT='(a,"Reading statistics file ",a," ",$)',todate,FILE_BASENAME(h5_statsfile)
;
;READ the groups 
gid = H5G_OPEN(h5_id, '/')
IF SIZE(in,/TYPE) ne 8 THEN in = H5_PARSE(gid,'/',/READ_DATA)
;
PRINT, FORMAT='("in ",i0," s.")',SYSTIME(1)-t0
;
h5_clutfile='clut_tst.h5' ; FIXME (variable not used)
;
;CREATE an output file
out=CREATE_STRUCT(  	    	    	$    
"_NAME"         ,h5_statsfile,    	$    
"_ICONTYPE"     ,"hdf",     	    	$    
"_TYPE"         ,"GROUP",   	    	$    
"_FILE"         ,h5_statsfile, 	$ 
"_PATH"         ,"/",     	    	$
"_COMMENT"      ,"(IDL) Cluttermap",	$
"what"          ,in.what,    	    	$
"where"         ,in.where)
;  
in_tags=WHERE(STRMID(TAG_NAMES(in),0,7) eq 'DATASET',nscan)
if nscan le 0 THEN MESSAGE, 'No datasets found in '+FILE_BASENAME(h5_statsfile)
;
FOR iscan=1,nscan DO BEGIN
  ;
  ;FIND dataset name according to scan number
  dataset_name=STRING(FORMAT='("dataset",i0)',iscan)
  ;
  ;FIND the field in the out structure corresponding to dataset_name
  i_intag=WHERE(TAG_NAMES(in) eq STRUPCASE(dataset_name),ntags)
  ;    
  ;LOAD the statistics data for this scan
  hist_data = in.(i_intag).data1.data._data
  ;
  ;FIND the dimension and create clutter data array 
  dimstat = SIZE(hist_data,/DIM)
  ;
  nrang=dimstat[1]
  nazim=dimstat[2]
  ;
;  nrang = 80
;  nazim =360

  clut_data = BYTARR(nrang,nazim)+iscan
  ;
  ;FIND the reflectivity corresponding to PERC for each pixel
  FOR i=0,nrang-1 DO BEGIN
    FOR j=0,nazim-1 DO BEGIN
      ;
      hist=hist_data[*,i,j]
      ;
      start = 0
      ;
      ;COMPUTE the mode and median of the distribution
      thismode = WHERE(hist eq MAX(hist[start,*],/NAN),nmode)
      thismedian = MAX(hist,/NAN) gt 0 ? FLOOR(MEDIAN(WHERE(hist gt 0))) : 0
      ;
      ;IF mode is set, use this
      IF KEYWORD_SET(mode) THEN BEGIN
        IF nmode gt 0 THEN clut_data[i,j] = thismode[0] ELSE MESSAGE,'No max found'
      ;
      ENDIF ELSE BEGIN
        ;
        ;ELSE IF median is set, use this	
	IF KEYWORD_SET(median) THEN BEGIN
          clut_data[i,j] = thismedian[0] 
	ENDIF ELSE BEGIN
	  ;
	  ;ELSE, if both are unset, use the given percentage
          ;
	  total=0
	  FOR k=start,255 DO BEGIN
	    total+=hist[k]  
	    IF total le TOTAL(hist)*perc/100. THEN BEGIN
	      clut_data[i,j]=k    
	    ENDIF ELSE BEGIN
	      ;IF percentage if found, break
	      BREAK
	    ENDELSE;percentage found
	  ENDFOR;k
	  ;
	ENDELSE;median
      ENDELSE;mode
      ;
    ENDFOR;i
  ENDFOR;j
  ;
  ;CREATE a new dataset structure to hold the clut_data
  dataset = {_NAME:dataset_name,_TYPE:"GROUP"}

  ;CREATE a clutter data group
  clutset_name = 'data1'
  clut_name = 'clutter_data'
  ;
  ;COPY the 'what' group from the hist_data group
  clut_what = in.(i_intag).data1.what
  ;CHANGE the name
  clut_what.quantity._data='scan_clutter_data'
  ;
  clutterdata={_NAME:'data',_TYPE:'Dataset',_DATA:clut_data,     	$
    	  CLASS:{_NAME:'CLASS',_TYPE:'ATTRIBUTE',_DATA:'IMAGE'},   	$
  IMAGE_VERSION:{_NAME:'IMAGE_VERSION',_TYPE:'ATTRIBUTE',_DATA:'1.2'}}
  clutter_group = {_NAME:clutset_name,_TYPE:"GROUP",DATA:clutterdata,WHAT:clut_what}

  ;IF in.datasetXX.what exists THEN add the what group to datsetXX
;    i_intag=WHERE(TAG_NAMES(in) eq STRUPCASE(dataset_name),ntags)
  IF ntags eq 1 THEN BEGIN
    i_settag=WHERE(TAG_NAMES(in.(i_intag)) eq "WHAT",ntags)
    IF ntags eq 1 THEN dataset = CREATE_STRUCT(dataset,"what",in.(i_intag).(i_settag))
    i_settag=WHERE(TAG_NAMES(in.(i_intag)) eq "WHERE",ntags)
    IF ntags eq 1 THEN dataset = CREATE_STRUCT(dataset,"where",in.(i_intag).(i_settag))
  ENDIF
  ;
  ;DUMP all in dataset structure
  dataset = CREATE_STRUCT(dataset,clutset_name,clutter_group)
  ;
  ;ADD dataset to out structure
  out=CREATE_STRUCT(out,dataset_name,dataset)

ENDFOR
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;STORE the updated histograms in a file
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

SPAWN,"date +%A', '%e' '%B' '%Y,' '%R:%S': '", todate
print, todate+'Writing cluttermap '+ h5_clutterfile

FILE_MKDIR,FILE_DIRNAME(h5_clutterfile)
FILE_DELETE,h5_clutterfile,/ALLOW_NONEXISTENT	;H5_CREATE should overwrite the file, but somehow it doesn't
H5_CREATE,h5_clutterfile,out
;

;
END
