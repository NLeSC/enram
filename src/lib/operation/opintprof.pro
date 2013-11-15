FUNCTION OPINTPROF, data, _EXTRA=ex

TRESH_STDEV=2.     ;stdev treshold for identification as birds
TRESH_PRECIP=0.8   ;stdev treshold for identification as rain
TRESH_DBZ=-100.    ;dbz treshold 
TRESH_MTR=1.       ;mtr treshold for deciding on bird presence
LAYER_HEIGHT=0.2   ;height layer spacing in km
SIGMA_BIRD=10.     ;avg bird cross section in cm^2/km^3
DBZFACTOR=335.4    ;factor for conversion dBZ to reflectivity
HEIGHT_RANGE=[0,4]  ;range of heights to integrate

;***********************************************************************************
; INPUT/KEYWORD HANDLING
;***********************************************************************************

;identify extra arguments
IF (N_ELEMENTS(ex) NE 0) THEN BEGIN
    IF WHERE(TAG_NAMES(ex) eq 'TRESH_STDEV') NE -1 THEN TRESH_STDEV=ex.TRESH_STDEV
    IF WHERE(TAG_NAMES(ex) eq 'TRESH_PRECIP') NE -1 THEN TRESH_PRECIP=ex.TRESH_PRECIP
    IF WHERE(TAG_NAMES(ex) eq 'TRESH_DBZ') NE -1 THEN TRESH_DBZ=ex.TRESH_DBZ
    IF WHERE(TAG_NAMES(ex) eq 'TRESH_MTR') NE -1 THEN TRESH_MTR=ex.TRESH_MTR
    IF WHERE(TAG_NAMES(ex) eq 'SIGMA_BIRD') NE -1 THEN SIGMA_BIRD=ex.SIGMA_BIRD
    IF WHERE(TAG_NAMES(ex) eq 'LAYER_HEIGHT') NE -1 THEN LAYER_HEIGHT=ex.LAYER_HEIGHT
    IF WHERE(TAG_NAMES(ex) eq 'HEIGHT_RANGE') NE -1 THEN HEIGHT_RANGE=ex.HEIGHT_RANGE
ENDIF

;check keyword contents
IF SIZE(LAYER_HEIGHT,/TYPE) NE 4 then error, "Float expected for 'LAYER_HEIGHT'", /ioerror
IF SIZE(SIGMA_BIRD,/TYPE) NE 4 then error, "Float expected for 'SIGMA_BIRD'", /ioerror
IF SIZE(TRESH_STDEV,/TYPE) NE 4 then error, "Float expected for 'TRESH_STDEV'", /ioerror
IF SIZE(TRESH_PRECIP,/TYPE) NE 4 then error, "Float expected for 'TRESH_PRECIP'", /ioerror
IF SIZE(TRESH_DBZ,/TYPE) NE 4 then error, "Float expected for 'TRESH_DBZ'", /ioerror
IF SIZE(TRESH_MTR,/TYPE) NE 4 then error, "Float expected for 'TRESH_MTR'", /ioerror
;check data type
CASE tag_names(data,/STRUCTURE_NAME) OF
  'DATFLEDER':BEGIN
     index=WHERE(data.mtr GT TRESH_MTR and data.height GT HEIGHT_RANGE[0] and data.height LT HEIGHT_RANGE[1])
     values=LAYER_HEIGHT*(data.density)[index]
     jultimes = (data.juldate)[index]
   END
  'DATVP2':BEGIN
     index=WHERE(data.stdev GT TRESH_STDEV AND data.precip LT TRESH_PRECIP and data.dbz GT TRESH_DBZ $
	     and data.height GT HEIGHT_RANGE[0] and data.height LT HEIGHT_RANGE[1])
     values=LAYER_HEIGHT*((10^(data.dbz/10+ALOG10(DBZFACTOR)))[index])/SIGMA_BIRD
     jultimes = (data.juldate)[index]
   END
   ELSE: ERROR, "Invalid data structure '"+tag_names(data,/STRUCTURE_NAME)+"' for vertical profile integration", /ioerror 	
ENDCASE

;sort on julian date
sortindex=SORT(jultimes)
values=values[sortindex]
jultimes=jultimes[sortindex]
uniqtimes=jultimes[uniq(jultimes)]  ;NOTE: uniq only works with sorted data

notimes=(SIZE(uniqtimes,/DIM))[0]
novalues=(SIZE(values,/DIM))[0]

;************************************************************************************
; compute vertically integrated bird densities
;************************************************************************************
arr=MAKE_ARRAY(notimes,/FLOAT,VALUE=0)
i=LONG(0) & j=LONG(0)
jultimeprev=jultimes[0]
jultimetmp=jultimeprev
WHILE i LT novalues-1 DO BEGIN
  WHILE(jultimetmp eq jultimeprev and i LT novalues-1) DO BEGIN
     arr[j]=arr[j]+values[i]
     i++
     jultimetmp=jultimes[i]
;     print, arr[j],values[i]
  ENDWHILE
  j++
  jultimeprev=jultimetmp
ENDWHILE
;determine the time grid size
ShiftedOneDay=-uniqtimes[lindgen(n_elements(uniqtimes)-1)]+uniqtimes[1+lindgen(n_elements(uniqtimes)-1)]
ShiftHist=histogram(round(60*24*ShiftedOneDay),binsize=1,min=0)
tgridsize=where(ShiftHist eq max(ShiftHist))/double(60*24)
if (n_elements(tgridsize) gt 1) then tgridsize=tgridsize[0]
;tgridsize=min(uniqtimes[1:(n_elements(uniqtimes[*,0])-1),0]-uniqtimes[0:(n_elements(uniqtimes[*,0])-2),0])
;determine where there are data gaps
gapindex=WHERE(uniqtimes[1:(N_ELEMENTS(uniqtimes[*,0])-1),0]-uniqtimes[0:(N_ELEMENTS(uniqtimes[*,0])-2),0] gt tgridsize+5./(24*60))
IF (SIZE(gapindex,/n_dim) GT 0) THEN BEGIN
   FOR i=0, N_ELEMENTS(gapindex)-1 DO BEGIN
	   uniqtimes=arrinsert(uniqtimes,uniqtimes[gapindex[i]+2*i]+tgridsize,AT=gapindex[i]+2*i+1)
	   uniqtimes=arrinsert(uniqtimes,uniqtimes[gapindex[i]+2*i+2]-tgridsize,AT=gapindex[i]+2*i+2)
	   arr=arrinsert(arr,0,AT=gapindex[i]+2*i+1)
	   arr=arrinsert(arr,0,AT=gapindex[i]+2*i+2)
   ENDFOR
ENDIF
RETURN,[[uniqtimes],[arr]]
END
