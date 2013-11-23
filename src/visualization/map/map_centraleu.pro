;PLOT multiple radars' ppi's

;SET the radar ids of the radars to be processed
radar_ids = ['NL','NO','PL','SK','FR']
radar_ids = radar_names(radar_ids)

subdir = '0'
directory = GETENV('IDL_ENRAM_CLUTTER_SENSITIVITY') + subdir

; SPECIFY the time interval to process.
date='20110901'
time='1730'

find_files,files,date,time,radar_ids,directory=directory

IF KEYWORD_SET(definitions) THEN dummy=TEMPORARY(definitions)
FOR id=0,N_ELEMENTS(radar_ids)-1 DO BEGIN
  radar_definitions,radar_ids[id],radar_definition
  definitions= KEYWORD_SET(definitions) ? [definitions,radar_definition] : radar_definition
ENDFOR

psfile='map_centraleu_'+date+time+'.ps' ; FIXME

scan=1
grid=1

plot_lon=0
plot_lat=45

limit=[47,10,56,28]

map_prof,files,limit,$
date=date,time=time,$
definitions=definitions,$
coast=0,$
bg=1,$
plot_lon=plot_lon,$
plot_lat=plot_lat,$
grid=grid,$
psfile=psfile,$
radar_cross=0,$
radar_title=2

showps, psfile

END
