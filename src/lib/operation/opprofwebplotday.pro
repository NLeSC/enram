
;************************************************************************************
;************************************************************************************
;
; PLOT DATA BIRDCAMPAIGN 
;
; REQUIRES INITIALISATION BY: READDAT.PRO
;
;************************************************************************************
;************************************************************************************

PRO OPPROFWEBPLOTDAY, datbilt, startdate, enddate, _EXTRA=ex

;**************************************************************************
; MAIN PROGRAM 
;****************************************************************************
  print, '*************************************************************************************'
  print, '** SYNTAX: weekplot, datbilt, datdenhelder, startdate, enddate,                    **'
  print, '**            TRESH_STDEV=2, TRESH_PRECIP=0.8, SIGMABIRD=10, YRANGE=[0,200]        **'
  print, '*************************************************************************************'


   ;factor for conversion dBZ to reflectivity
   LAYERHEIGHT=0.2   ;height layer spacing in km
   DBZFACTOR=335.4   ;conversion factor reflectivity factor Z to reflectivity eta.

   ;identify extra arguments
   IF (N_ELEMENTS(ex) NE 0) THEN BEGIN
     IF WHERE(TAG_NAMES(ex) eq 'TRESH_STDEV') NE -1 THEN TRESH_STDEV=ex.TRESH_STDEV ELSE TRESH_STDEV=2
     IF WHERE(TAG_NAMES(ex) eq 'TRESH_PRECIP') NE -1 THEN TRESH_PRECIP=ex.TRESH_PRECIP ELSE TRESH_PRECIP=1
     IF WHERE(TAG_NAMES(ex) eq 'SIGMABIRD') NE -1 THEN SIGMABIRD=ex.SIGMABIRD ELSE SIGMABIRD=10
     IF WHERE(TAG_NAMES(ex) eq 'YRANGE') NE -1 THEN YRNG=ex.YRANGE ELSE YRNG=[0,100]
     IF WHERE(TAG_NAMES(ex) eq 'FILE') NE -1 THEN FILE=ex.FILE ELSE FILE='opwebplotday.ps'
   ENDIF ELSE BEGIN
     TRESH_STDEV=2
     TRESH_PRECIP=1
     SIGMABIRD=10
     YRNG=[0,100]
     FILE='opwebplotday.ps'
   ENDELSE
   ;flag out below two lines to output to x window instead of postscript
   set_plot, 'ps'
   ;Device, _Extra=PSConfig(/NoGUI, /portrait, /european, Filename='webplotday.ps')
   DEVICE, BITS_PER_PIXEL=8, COLOR=1,filename=FILE
   ;use Postscript fonts instead of Hershey fonts (no good for 3D!)
   !P.FONT=0
   ;define time axis format
   ;dummy=LABEL_DATE(DATE_FORMAT=['%D/%M'])
   dummy=LABEL_DATE(DATE_FORMAT=['%H:%I'])
   flag1=TRESH_STDEV   ;necessary, apparantly pass by reference
   flag2=TRESH_PRECIP
   datbiltint=opintprof(datbilt, TRESH_STDEV=float(TRESH_STDEV), TRESH_PRECIP=float(TRESH_PRECIP))
   datbiltint2=opintprof(datbilt, TRESH_STDEV=float(TRESH_STDEV), TRESH_PRECIP=float(TRESH_PRECIP),HEIGHT_RANGE=[0.2,4])
   
   DateRange=date2jd([startdate, enddate])

   ;************************************************************************************
   ; contourplot mtr vs height and date
   ;************************************************************************************
   loadct,5
   !P.Multi=[0,1,2] 
   RangeX=DateRange
   device,xsize=0.75*20,ysize=0.75*13*sqrt(2)*2/3
   ;convert to reflectivity, keep a 10log scale
   
   index_stdev=WHERE(datbilt.stdev GT TRESH_STDEV AND datbilt.precip LT TRESH_PRECIP, COMPLEMENT=index_stdev2)
   stdevplot=(datbilt.dbz/10+ALOG10(DBZFACTOR))
   stdevplot[index_stdev2]=-1000
   index_stdev3=WHERE(~finite(datbilt.dbz))
   if n_elements(index_stdev3) ne 1 then stdevplot[index_stdev3]=!VALUES.F_NAN
   opplotprof, datbilt.juldate,datbilt.height,stdevplot,datbilt.u,datbilt.v,/windbarb,NVectors=[30,10],vectorlength=0.02,$
      TITLE=STRJOIN([TeXtoIDL('Bird density height profile'),'']),$
      BARTITLE=TeXtoIDL('Reflectivity (log10)'),XRANGE=RangeX,$
      XPOS=0,YTICKLEN=1,XTICKLEN=1,CONTOURRANGE=[0.8,3.8],XSTYLE=1,POSITION=[0.1,0.55,0.8,0.9],XTICKUNITS='Time',XTICKS=12,BIRDTICKS=1

   ;************************************************************************************
   ; vertically integrated bird density plot
   ;************************************************************************************

   dummy=LABEL_DATE(DATE_FORMAT=['%D/%m'])
   loadct,5
   plot,datbiltint2[*,0],datbiltint2[*,1], XTICKFORMAT = 'LABEL_DATE',$
     XTICKUNITS = ['Time'], XRANGE=RangeX, YRANGE=YRNG,$
     XSTYLE=1,YSTYLE=1,XTICKLEN=1,POSITION=[0.1,0.05,0.8,0.40],LINESTYLE=1,/noerase,/nodata
   plotsunrise,datbilt.juldate,XRANGE=RangeX, POSITION=[0.1,0.05,0.8,0.40],yrange=YRNG
   plot,datbiltint2[*,0],datbiltint2[*,1], XTICKFORMAT = 'LABEL_DATE',$
     XTICKUNITS = ['Time'],XTITLE="Time (UTC)",YTITLE=TexToIdl("Density [birds / km^2]"),$
     TITLE="Height-integrated bird density", XRANGE=RangeX, YRANGE=YRNG,$; POSITION=[0.1,0.2,0.8,0.8],$
     XSTYLE=1,YSTYLE=1,XTICKLEN=1,POSITION=[0.1,0.05,0.8,0.40],LINESTYLE=1
;  oplot,datbiltint[*,0],datbiltint[*,1],LINESTYLE=0, COLOR=50
   oplot,datbiltint2[*,0],datbiltint2[*,1],LINESTYLE=0, COLOR=120
;   legend,['all', '> 200 m'],linestyle=[0,0],colors=[50,120],charsize=0.5,box=0,THICK=4,pos=[8700,3800],/device
;   cmd='mv -f idl.'+strlowcase(!d.name)+' '+FILE
;   spawn,cmd
   DEVICE,/CLOSE
   SET_PLOT,'X'
END
