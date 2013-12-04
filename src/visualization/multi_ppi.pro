;PLOT multiple radars' ppi's

;SET the radar ids of the radars to be processed
radar_ids = ['FR']
radar_ids = radar_names(radar_ids)

; SPECIFY the time interval to process.
date='20110915'
time='2200'

find_volume_files,files,date,time,radar_ids

IF KEYWORD_SET(definitions) THEN dummy=TEMPORARY(definitions)
FOR id=0,N_ELEMENTS(radar_ids)-1 DO BEGIN
  radar_definitions,radar_ids[id],radar_definition
  definitions= KEYWORD_SET(definitions) ? [definitions,radar_definition] : radar_definition
ENDFOR

psfile='map_ppi_'+date+time+'.ps'  ; FIXME

scan=1
grid=1

plot_lon=0
plot_lat=45
limit=[40,-15,70,40]

map_ppi,files,limit,$
definitions=definitions,$
coast=0,$
bg=1,$
plot_lon=plot_lon,$
plot_lat=plot_lat,$
scan=scan,$
grid=grid,$
psfile=psfile,$
radar_cross=20

showps, psfile

END
