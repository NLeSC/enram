PRO PLOTSUNRISE, dates, _EXTRA=ex
;identify extra arguments
IF (N_ELEMENTS(ex) NE 0) THEN BEGIN
  IF WHERE(TAG_NAMES(ex) eq 'POSITION') NE -1 THEN POS=ex.POSITION ELSE POS=[0.1,0.2,0.8,0.8]
  IF WHERE(TAG_NAMES(ex) eq 'XRANGE') NE -1 THEN DATE_RANGE=ex.XRANGE ELSE DATE_RANGE=[min(dates),max(dates)]
  IF WHERE(TAG_NAMES(ex) eq 'YRANGE') NE -1 THEN YRANGE=ex.YRANGE ELSE YRANGE=[0,1]
ENDIF ELSE BEGIN
  POS=[0.1,0.2,0.8,0.8]
  YRANGE=[0,1]
  DATE_RANGE=[MIN(dates),MAX(dates)]
ENDELSE

;nodays=1+floor(max(date_range)-min(date_range))
;nodays=n_elements(uniq(round(dates)))
nodays=n_elements(TIMEGEN(1,final=max(date_range),start=min(date_range),units='days',step_size=1))
CALDAT, date_range, month, day, year
if (nodays eq 1 and day[0] ne day[1]) then nodays+=1 ;dirty hack
pcolor=255
for j=0,1 do begin
   juldates=min(date_range)+dindgen(nodays)
   times=dindgen(nodays*2)
   if (j eq 1) then tvlct,[[210],[210],[210]],pcolor
   if (j eq 0) then tvlct,[[230],[230],[230]],pcolor
   for i=0,n_elements(juldates)-1 do begin
	if (j eq 1) then tmp=sunrise((juldates[i]),/twilight)
	if (j eq 0) then tmp=sunrise((juldates[i]))
	times[2*i]=tmp[0]
	times[2*i+1]=tmp[1]
   endfor
   if min(date_range) lt times[0] then begin
	   grid=[min(date_range),times]
   endif
   if min(date_range) gt times[0] and min(date_range) lt times[1] then begin
	   grid=times[1+lindgen(n_elements(times)-1)]
   endif
   if min(date_range) gt times[1] then begin
      grid=times[1+lindgen(n_elements(times)-1)]
      grid[0]=min(date_range)
   endif
   if max(date_range) lt times[n_elements(times)-2] then begin
      grid2=grid[lindgen(n_elements(grid)-1)]
      grid2[n_elements(grid2)-1]=max(date_range)
   endif
   if ((max(date_range) gt times[n_elements(times)-2]) and (max(date_range) lt times[n_elements(times)-1])) then begin
	if n_elements(grid) le 1 then return ;daytime only, plot no shades
   	grid2=grid[lindgen(n_elements(grid)-1)]
   endif
   if max(date_range) gt times[n_elements(times)-1] then begin 
	grid2=[grid,max(date_range)]
   endif
   grid=grid2
   for i=0,n_elements(grid)/2-1 do begin
      polyfill,[grid[2*i],grid[2*i],grid[2*i+1],grid[2*i+1]],[yrange[0],yrange[1],yrange[1],yrange[0]],color=pcolor
   endfor
endfor
END
