;+
; NAME:
;       colordivbar.pro
;
; PURPOSE:
;       Create a colorbar for a colorplot.
;       A row or column of color boxes is created and the values
;       attributed to the levels of each box are plotted below, above,
;       to the right, or to the left of the boxes.
;       The size and number of boxes is variable, the character size too
;       
; CATEGORY:
;       graphic routines, contour plots
;
; CALLING SEQUENCE:
;       colorbar,levels,colors,/col,/right,/noframe,charsize=charsize,$
;       charthick=charthick,format=format,lowleft=lowleft,xsize=xsize,$
;       ysize=ysize,unit=unit,/nofirstlast,loffset=loffset,ivestyle=ivestyle,$
;       uoffset=uoffset,levelind=levelind
;
; EXAMPLE:
;       ; create a contour plot with n=15 levels and plot
;       ; a colorbar next to it.
;       x=indgen(100) & y=indgen(100)
;       z=x#sqrt(y)
;       n=15    ; number of levels
;       levels=findgen(15)*70
;       colors=indgen(n-1)*15
;       !x.omargin=[0,10]
;       contour,z,x,y,levels=levels,c_colors=colors,/fill
;       colorbar,levels,colors,format='(i4)',/col,/right,$
;          lowleft=[0.85,0.2],ysize=0.6,/noframe,unit='[PVU]'
;
; INPUTS:
;       levels          FltArr(n): 
;                       i<=n-1: levels[i] is the data values of
;                               the lower limit of box i
;                       i=n:    levels[n] is the
;                               value of the upper limit of nox n-1 box
;       colors          FltArr(n-1): the colors of the boxes 
;                               (1 element less than levels!!)
;
; OPTIONAL INPUT PARAMETERS:
;       charsize        the size of the numbers and the unit title
;       charthick       the thickness of the numbers and the unit title
;       format          the format string for the numbers (e.g. '(f0.2)')
;       lowleft         (x,y): the lower left corner of the colorbar in normalized units
;       xsize           the horizontal length of the colorbar in normalized units
;       ysize           the vertical length of the colorbar in normalized units (0-1)
;       loffset         the (x,y)-offset of the labels from their default position in
;                       normal coos.
;       uoffset         the (x,y)-offset of the unit label from its default position in
;                       normal coos.
;       toffset         the (x,y)-offset of the title from its default position in
;                       normal coos.
;       levelind        the indices of the levels for which numbers should be plotted
;                       (if not provided, numbers for each level are shown
;       
; KEYWORD INPUT PARAMETERS:
;       col             draws a vertical instead of a horizontal colorbar
;       right           plots numbers to the right instead of left of a column colorbar
;       nofirstlast     dont plot the numbers of the first/last level
;       noframe         dont plot a frame around each color-box
;       ivestyle        draws a vertical colorbar to the right of the
;                       current plot, from the bottom to the top 
; OUTPUTS:                      
;
; COMMON BLOCKS:
;
; SIDE EFFECTS:
;
; RESTRICTIONS:
;       
; PROCEDURE:
;       
; MODIFICATION HISTORY:
;       first implementation Feb, 27 1997 by Dominik Brunner
;-
pro colordivbar,levels,colors,col=col,charsize=charsize,format=format,$
        right=right,lowleft=lowleft,xsize=xsize,ysize=ysize,unit=unit,$
        nofirstlast=nofirstlast,loffset=loffset,uoffset=uoffset,$
        charthick=charthick,noframe=noframe,levelind=levelind,	$
	ticklen=ticklen,title=title,toffset=toffset,$
	ivestyle=ivestyle,pointed=pointed,_EXTRA=extra

;------------- check for input parameters ---------------------

IF n_elements(levels) EQ 0 THEN BEGIN
   print,'parameter levels missing in call to colorbar'
   return
ENDIF

IF n_elements(levelind) EQ 0 THEN levelind=indgen(n_elements(levels))

IF n_elements(colors) EQ 0 THEN BEGIN
   print,'parameter colors missing in call to colorbar'
   return
ENDIF

IF n_elements(colors) NE (n_elements(levels)-1) THEN BEGIN
   print,'array colors must have one element less than array levels'
   return
ENDIF

IF n_elements(loffset) NE 2 THEN loffset=[0.,0.]

IF n_elements(uoffset) NE 2 THEN uoffset=[0.,0.]

IF n_elements(toffset) NE 2 THEN toffset=[0.,0.]

IF N_ELEMENTS(ticklen) NE 1 THEN TICKLEN = 1

IF keyword_set(col) THEN row=0 ELSE row=1

IF n_elements(ysize) EQ 0 THEN BEGIN
   IF row THEN ysize=0.03 ELSE ysize=0.8
ENDIF

IF n_elements(xsize) EQ 0 THEN BEGIN
   IF row THEN xsize=0.8 ELSE xsize=0.03
ENDIF

IF n_elements(charsize) EQ 0 THEN charsize=1.

; Determine position and length for an ive-style plot
; from current graphic window
IF keyword_set(ivestyle) THEN BEGIN
  IF keyword_set(col) THEN BEGIN
     right=1
     lowleft=[!x.window[1]+0.02*(!x.window[1]-!x.window[0]),!y.window[0]]
     ysize=!y.window[1]-!y.window[0]
   ENDIF ELSE BEGIN
     lowleft=[!x.window[0],!y.region[0]+0.4*ysize+2.*charsize*!d.y_ch_size/float(!d.y_size)]
     xsize=!x.window[1]-!x.window[0]
   ENDELSE
ENDIF
 
IF NOT row THEN BEGIN
   IF keyword_set(right) THEN left=0 ELSE left=1
ENDIF

IF n_elements(precision) EQ 0 THEN precision=2

IF n_elements(lowleft) EQ 0 THEN lowleft=[0.1,0.1]

IF n_elements(format) EQ 0 THEN format='(F0.0)'

;---------------------- draw the colorbar --------------------------
nboxes=n_elements(colors)       ; the number of color boxes in the colorbar
xstep=xsize/nboxes*row          ; the xoffset between two subsequent boxes
ystep=ysize/nboxes*(1-row)      ; the yoffset between two subsequent boxes
xwidth=xsize*(1-row)            ; the with of the box in a column colorbar
ywidth=ysize*row                ; the with of the box in a row colorbar

FOR i=0,nboxes DO BEGIN
        ; draw the box
        xmin=lowleft(0)+i*xstep
        xmax=lowleft(0)+(i+1)*xstep+xwidth
        ymin=lowleft(1)+i*ystep
        ymax=lowleft(1)+(i+1)*ystep+ywidth
        IF i LT nboxes THEN BEGIN
           IF keyword_set(pointed) AND (NOT keyword_set(ivestyle)) AND  $
              ((i EQ 0) OR (i EQ nboxes-1)) THEN BEGIN
              IF i EQ 0 THEN BEGIN
                 IF row THEN box=[[xmin,(ymin+ymax)/2.],[xmax,ymin],[xmax,ymax]] $
                 ELSE box=[[(xmin+xmax)/2.,ymin],[xmax,ymax],[xmin,ymax]] 
              ENDIF ELSE BEGIN
                 IF row THEN box=[[xmin,ymin],[xmax,(ymin+ymax)/2.],[xmin,ymax]] $
                 ELSE box=[[xmin,ymin],[xmax,ymin],[(xmin+xmax)/2.,ymax]]
              ENDELSE
           ENDIF ELSE BEGIN
              box=[[xmin,ymin],[xmax,ymin],[xmax,ymax],[xmin,ymax]]
           ENDELSE
           polyfill,box,color=colors[i],/norm
           IF NOT keyword_set(noframe) THEN $
              plots,[Reform(box(0,*)),box(0,0)],[Reform(box(1,*)),box(1,0)],/norm,_EXTRA=extra
        ENDIF ELSE BEGIN
           ; plot the unit
           IF n_elements(unit) NE 0 THEN BEGIN
              IF row THEN BEGIN
                 xyouts,xmin+!d.x_ch_size/float(!d.x_size)+uoffset[0],ymin+uoffset[1],unit,$
                        charsize=charsize,charthick=charthick,/norm,_EXTRA=extra
               ENDIF ELSE BEGIN
                 xyouts,xmin-xwidth+uoffset[0],ymin+0.6*!d.y_ch_size/float(!d.y_size)+uoffset[1],$
                   unit,charsize=charsize,charthick=charthick,/norm,_EXTRA=extra
              ENDELSE
           ENDIF
           ;plot the title		   
           IF n_elements(title) NE 0 THEN BEGIN
              IF row THEN BEGIN
                 xyouts,(lowleft[0]+xmin)/2+xwidth+toffset[0],ymin+2*!d.y_ch_size/float(!d.y_size)+toffset[1],$
                   title,charsize=charsize,charthick=charthick,/norm,alignment=0.5,_EXTRA=extra
               ENDIF ELSE BEGIN
              ;and a title		   
                 xyouts,xmin+xwidth+!d.x_ch_size/float(!d.x_size)+toffset[0],(lowleft[1]+ymin)/2+0.6*!d.y_ch_size/float(!d.y_size)+toffset[1],$
                   title,charsize=charsize,charthick=charthick,/norm,alignment=0.5,_EXTRA=extra,orientation=270
              ENDELSE
           ENDIF
        ENDELSE
        
        ; plot tickmarks and values
        ind=WHERE(i EQ levelind,count)
        IF count GT 0 THEN BEGIN
           IF ((i EQ 0) OR (i EQ nboxes)) AND keyword_set(nofirstlast) THEN BEGIN
                $; do nothing numstr='' $
           ENDIF ELSE BEGIN
              numstr=STRING(format=format,levels(i))
              IF row THEN BEGIN
                 plots,[xmin,xmin],[ymin,ymin-(0.2*ticklen)*ywidth],/norm,_EXTRA=extra
                 xyouts,xmin+loffset(0),ymin-(0.2+(0.2*ticklen))*ywidth-charsize*$
                   !d.y_ch_size/float(!d.y_size)+loffset(1),numstr,$
                   /norm,charsize=charsize,charthick=charthick,alignment=0.5,_EXTRA=extra
              ENDIF ELSE BEGIN
                 IF left THEN BEGIN
                    plots,[xmin,xmin-(0.2*ticklen)*xwidth],[ymin,ymin],/norm,_EXTRA=extra
                    xyouts,xmin-(0.2+(0.2*ticklen))*xwidth+loffset(0),ymin-0.5*charsize*!d.y_ch_size/$
                      float(!d.y_size)+loffset(1),numstr,/norm,charsize=charsize,$
                      charthick=charthick,alignment=1.0,_EXTRA=extra
                 ENDIF ELSE BEGIN
                    plots,[xmax,xmax+(0.2*ticklen)*xwidth],[ymin,ymin],/norm,_EXTRA=extra
                    xyouts,xmax+(0.2+(0.2*ticklen))*xwidth+loffset(0),ymin-0.5*charsize*!d.y_ch_size/$
                      float(!d.y_size)+loffset(1),numstr,/norm,charsize=charsize,$
                      charthick=charthick,_EXTRA=extra
                 ENDELSE
              ENDELSE
           ENDELSE
        ENDIF   
ENDFOR


END
