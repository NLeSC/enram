;PLOT multiple radars' ppi's

;SET the radar ids of the radars to be processed
radar_ids = ['SK']
;radar_ids = []
radar_ids = radar_names(radar_ids)

directory = '/usr/people/graafdem/FLYSAFE/process/data/sens_clutter/0/'

; SPECIFY the time interval to process.
;
date='20110915'
time='2200'
;time='2100'

IF KEYWORD_SET(definitions) THEN dummy=TEMPORARY(definitions)
FOR id=0,N_ELEMENTS(radar_ids)-1 DO BEGIN
  radar_definitions,radar_ids[id],radar_definition
  definitions= KEYWORD_SET(definitions) ? [definitions,radar_definition] : radar_definition
ENDFOR

find_files,files,date,time,radar_ids;,directory=directory

;print, files
;stop

psfile='map_prof_'+date+time+'.ps'

scan=1
grid=1

plot_lon=0
plot_lat=45
;
;limit=[51,0,56,8]
limit=[41,-15,71,40]

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
radar_title=0

showps, psfile

END
