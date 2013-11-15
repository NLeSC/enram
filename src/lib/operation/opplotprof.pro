PRO OPPLOTPROF, dates, heights, zvals, uvals, vvals, WINDBARB=windbarb,VECTOR=vector, _EXTRA=ex
  TitleBar=''
  ContourRange=[0,Max(zvals,/NAN)]
  TICKS=-1
  BIRDTICKS=-1
  POS=[0.1,0.2,0.8,0.8]
  BARPOS=0
  BAR=1
  HEIGHT_RANGE=[0,4]
  DATE_RANGE=[MIN(dates),MAX(dates)]
  NVECTORS=0
  YTITLE="height (km)"
  XTITLE="Time (UTC)"
;identify extra arguments
IF (N_ELEMENTS(ex) NE 0) THEN BEGIN
  IF WHERE(TAG_NAMES(ex) eq 'BARTITLE') NE -1 THEN TitleBar=ex.BARTITLE[0]
  IF WHERE(TAG_NAMES(ex) eq 'CONTOURRANGE') NE -1 THEN ContourRange=ex.CONTOURRANGE
  IF WHERE(TAG_NAMES(ex) eq 'POSITION') NE -1 THEN POS=ex.POSITION
  IF WHERE(TAG_NAMES(ex) eq 'DBZTICKS') NE -1 THEN TICKS=ex.DBZTICKS
  IF WHERE(TAG_NAMES(ex) eq 'BIRDTICKS') NE -1 THEN BIRDTICKS=ex.BIRDTICKS
  IF WHERE(TAG_NAMES(ex) eq 'BARPOS') NE -1 THEN BARPOS=ex.BARPOS
  IF WHERE(TAG_NAMES(ex) eq 'BAR') NE -1 THEN BAR=ex.BAR
  IF WHERE(TAG_NAMES(ex) eq 'YRANGE') NE -1 THEN HEIGT_RANGE=ex.YRANGE
  IF WHERE(TAG_NAMES(ex) eq 'XRANGE') NE -1 THEN DATE_RANGE=ex.XRANGE
  IF WHERE(TAG_NAMES(ex) eq 'XTITLE') NE -1 THEN XTITLE=ex.XTITLE
  IF WHERE(TAG_NAMES(ex) eq 'YTITLE') NE -1 THEN YTITLE=ex.YTITLE
  IF WHERE(TAG_NAMES(ex) eq 'NVECTORS') NE -1 THEN NVECTORS=ex.NVECTORS
ENDIF
  if n_elements(windbarb) le 0 then windbarb = 0
  if n_elements(vector) le 0 then vector = 0
  if n_elements(vectorlength) le 0 then vectorlength = 0.01
MAXHEIGHT=max(HEIGHT_RANGE)
DBZFACTOR=335.4   ;conversion factor reflectivity factor Z to reflectivity eta.
ContourRange=FLOAT(ContourRange)
heights=heights-0.1

;Determine the data grid
GridDate=(dates)[UNIQ(dates, SORT(dates))]
GridUniqDate=GridDate
GridHeight=(heights)[UNIQ(heights, SORT(heights))]
ShiftedOneDay=-GridUniqDate[lindgen(n_elements(GridUniqDate)-1)]+GridUniqDate[1+lindgen(n_elements(GridUniqDate)-1)]
ShiftHist=histogram(round(60*24*shiftedoneday),binsize=1,min=0)
GridSizeDate=where(ShiftHist eq max(ShiftHist))/double(60*24)
If (n_elements(gridsizedate) ne 1) then begin
    PRINT, FORMAT = '("WARNING: Equal probability for different time grid sizes ", 20(I1, " "))',60*24*GridSizeDate
    print, '         Using smallest grid...'
	GridSizeDate=GridSizeDate[0]
endif
GSDMin=round(60*24*GridSizeDate)
if (GSDMin mod 5) ne 0 then begin
    PRINT, FORMAT = '("WARNING: Detected time grid not a multiple of 5 minutes: ", 20(I1, " "))',GSDMin
	if (GSDMin mod 5) lt 2.5 then GridSizeDate=GridSizeDate-(GSDMin mod 5)/(60.*24) $
	else GridSizeDate=GridSizeDate+(5-(GSDMin mod 5))/(60.*24)
    PRINT, FORMAT = '("         Adjusting to nearest multiple of 5 minutes: " 20(I1, " "))',Round(60*24*GridSizeDate)
endif
GridSizeHeight=min(-GridHeight[lindgen(n_elements(GridHeight)-1)]+GridHeight[1+lindgen(n_elements(GridHeight)-1)])
GridDate=TIMEGEN(1,final=max(date_range)-GridSizeDate,start=min(date_range),units='minutes',step_size=round(GridSizeDate*60*24))
GridHeight=GridSizeHeight*lindgen(max(HEIGHT_RANGE)/GridSizeHeight)
PlotMatrix=0*findgen(size([GridDate],/dim),size([GridHeight],/dim))-2000
IF (n_elements(NVectors) eq 1 ) then NVectors=size(Plotmatrix,/dim)
UMatrix=make_array(size([GridDate],/dim),size([GridHeight],/dim),value=!VALUES.F_NAN)
VMatrix=UMatrix
if (size(PlotMatrix,/n_elements) EQ 1) then return 
n=LONG64((SIZE(dates,/dim))[0])
m=LONG64((SIZE(GridUniqDate,/dim))[0])
FOR i=LONG64(0),m-1 DO BEGIN
	dindex=round((GridUniqDate[i]-GridDate[0])/GridSizeDate)
	if dindex lt 0 or dindex ge (size(plotmatrix,/dim))[0] then CONTINUE
	PlotMatrix[dindex,*]=-1000
ENDFOR
if (not windbarb) then uscale=GridSizeDate/GridSizeHeight else uscale=1
FOR i=LONG64(0),n-1 DO BEGIN
	dindex=round((dates[i]-GridDate[0])/GridSizeDate)
	hindex=round((heights[i]-GridHeight[0])/GridSizeHeight)
	if dindex lt 0 or dindex ge (size(plotmatrix,/dim))[0] $
  	   or hindex lt 0 or hindex ge (size(plotmatrix,/dim))[1] then CONTINUE
	PlotMatrix[dindex,hindex]=zvals[i] 
	UMatrix[dindex,hindex]=uvals[i]*uscale
	VMatrix[dindex,hindex]=vvals[i]
ENDFOR
image = (!D.TABLE_SIZE)*(PlotMatrix-min(ContourRange))/(max(ContourRange)-min(ContourRange))
nodata=where(PlotMatrix le -1999)
nulldata=where(PlotMatrix le -999)
;reverse color bar, add grey for nodata, black for lines and white for zero data
   tvlct, r, g, b, /get
   r=reverse(r)
   g=reverse(g)
   b=reverse(b)
   r[255]=200
   g[255]=200
   b[255]=200
   r[0]=0
   g[0]=0
   b[0]=0
   r[1]=255
   g[1]=255
   b[1]=255
   TVLCT,r,g,b
;apply colors for nodata or zero data    
   image=round(image)
   if ~(size(nulldata,/n_elements) EQ 1) then image[nulldata]=1
   if ~(size(nodata,/n_elements) EQ 1) then image[nodata]=255
   if (size(where(~finite(plotmatrix)),/n_elements) ne 1) then image[where(~finite(plotmatrix))]=255
;shift data out of ContourRange within valid pixel values:
   epsilon=(max(contourrange)-min(contourrange))/!D.TABLE_SIZE
   index=where(PlotMatrix le min(ContourRange)+epsilon and PlotMatrix gt -1000)
   if ~(size(index,/n_elements) EQ 1) then image[index]=2
   index=where(PlotMatrix ge max(ContourRange)-epsilon)
   if ~(size(index,/n_elements) EQ 1) then image[index]=254
;plot the data
Plot,DATE_RANGE,HEIGHT_RANGE, $
  XTICKUNITS = ['Time'],XTICKFORMAT='LABEL_DATE',$
  XTITLE=XTITLE,YTITLE=YTITLE,POSITION=POS,$
  XSTYLE=1,YSTYLE=1,_EXTRA=ex,/NODATA
tv,image, !X.WINDOW(0), !Y.WINDOW(0), XSIZE = !X.WINDOW(1) - !X.WINDOW(0), YSIZE = !Y.WINDOW(1) - !Y.WINDOW(0), /NORM
Plot,DATE_RANGE,HEIGHT_RANGE, $
  XTICKUNITS = ['Time'],XTICKFORMAT='LABEL_DATE',$
  XTITLE=XTITLE,YTITLE=YTITLE,POSITION=POS,$
  XSTYLE=1,YSTYLE=1,_EXTRA=ex,/NODATA,/noerase
;plot speed vectors
VectGridDate=Congrid(GridDate,NVectors[0])
VectGridHeight=Congrid(GridHeight,NVectors[1])
Umatrix=frebin(UMatrix,N_Elements(VectGridDate),N_Elements(VectGridHeight))
Vmatrix=frebin(VMatrix,N_Elements(VectGridDate),N_Elements(VectGridHeight))
if vector then MYVELOVECT, (UMatrix),(VMatrix),VectGridDate,VectGridHeight,$
	/overplot,missing=50,vmax=25,length=vectorlength,clip=POS
if windbarb then MYVELOVECT, (UMatrix),(VMatrix),VectGridDate,VectGridHeight,$
	/overplot,missing=200,vmax=25,windbarb=windbarb,length=vectorlength,clip=POS, aspect = 1.*!D.X_Size/!D.Y_Size
;write colorbar
    IF (TICKS ge 1) THEN Colorbar_n, Position=POS+[0.77,0,0.1,0],Range=10*(ContourRange-ALOG10(DBZFACTOR)),/Vertical,$
        Format='(F6.1)',Title="Reflectivity factor [dBZ]",/RIGHT,charsize=0.7
    IF (BIRDTICKS ge 1) THEN Colorbar, Position=POS+[0.77,0,0.1,0],Range=(10^ContourRange)/10.,/Vertical,$
        Format='(I6)',Title=TexToIDL("Density [Birds / km^3]"),/RIGHT,charsize=0.7,/ylog,yticks=0
    IF (BARPOS eq 1) THEN BEGIN  ;this is for the residuals plot only
	 Colorbar_n, Position=POS+[0.77,0,0.1,0],Range=ContourRange,/Vertical,$
         Format='(F6.1)',Title=TitleBar,/RIGHT,charsize=0.7
;    ENDIF ELSE BEGIN IF (BAR) then Colorbar_n, Position=[POS[0]+(POS[2]-POS[0])/20,POS[1],POS[2]+(POS[2]-POS[0])/20,POS[3]],Range=ContourRange,/Vertical,$
    ENDIF ELSE BEGIN IF (BAR) then Colorbar_n, Position=POS+[0.77,0,0.1,0],Range=ContourRange,/Vertical,$
	        Format='(F6.1)',Title=TitleBar,charsize=0.7
    ENDELSE
TVLCT, reverse(r), reverse(g), reverse(b)
END
