;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;FUNCTION radar_names
;
;LABELS     	INPUT names, which can be simple country abbreviations, like NL
;RADAR_IDS  	OUTPUT radar ids, for use in radar definitions file
;PRINT	    	KEYWORD to print an IDL array of radar ids, for easy copy and paste
;

;GET radar names and definitions using simple labels
;
;EXAMPLES
;
;radar_ids=radar_ids=radar_names(['no','fi'])
;help, radar_ids
;RADAR_IDS       STRING    = Array[16]
;print, radar_ids
;NOAND NOBML NOHAS NOHGB NOHUR NORSA NORST NOSTA FIANJ FIIKA FIKOR FIKUO FILUO FIUTA FIVAN FIVIM
;
;print, radar_names('netherlands',['NOAND','NOBML'])
;NOAND NOBML NLDBL NLDHL
;
;sweden_id = radar_names('SE',/print)     
;['SEVIL','SEVAR','SEOVI','SEOSU','SELUL','SELEK','SEKKR','SEKIR','SEHUD','SEASE','SEARL','SEANG']
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
FUNCTION radar_names,labels,radar_ids,$
print=print
;
;IF radar_ids is already given, extend it, otherwise create new
new_id = N_ELEMENTS(radar_ids) eq 0 ? 1 : 0
;
all_ids = [$
'CZBRD','CZSKA',$
;'HRBIL',$
'FIANJ','FIIKA','FIKOR','FIKUO','FILUO','FIUTA','FIVAN','FIVIM',$
'FRABB','FRALE','FRTRO','FRAVE','FRBLA','FRBOL','FRBOR','FRBOU','FRCAE','FRCHE','FRCOL','FRGRE','FRMOM','FRMTC','FRMCL','FRNAN','FRNIM',$
'IEDUB','IESHA',$
'NLDBL','NLDHL',$
'NOAND','NOBML','NOHAS','NOHGB','NOHUR','NORSA','NORST','NOSTA',$
'PLBRZ','PLGDA','PLLEG','PLPAS','PLPOZ','PLRAM','PLRZE','PLSWI',$
'SEVIL','SEVAR','SEOVI','SEOSU','SELUL','SELEK','SEKKR','SEKIR','SEHUD','SEASE','SEARL','SEANG',$
'SILIS',$
'SKMAJ','SKKOH'$
]
IF STRUPCASE(labels[0]) eq 'ALL' THEN labels=all_ids
;
;CHECK the given labels
label_number=N_ELEMENTS(labels)
IF label_number le 0 THEN MESSAGE, 'label is not defined. Please provide a string or string array'
;
;LOOP through multiple labels
FOR i=0,label_number-1 DO BEGIN
  ;
  ;FIND next label
  label=STRUPCASE(labels[i])
  ;
  ;CHECK for country codes
  CASE label of 

  'CZ': label='CZECH REPUBLIC'
  'FI': label='FINLAND'
  'FR': label='FRANCE'
  'HR': label='CROATIA'
  'IE': label='IRELAND'
  'NL': label='NETHERLANDS'
  'NO': label='NORWAY'
  'PL': label='POLAND'
  'SE': label='SWEDEN'
  'SI': label='SLOVENIA'
  'SK': label='SLOVAKIA'

  ELSE:

  ENDCASE
  ;
  ;CHECK for country names
  CASE label of 

  'CZECH REPUBLIC': this_radar_ids=['CZBRD','CZSKA']
  'CROATIA' : this_radar_ids=['HRBIL']
  'FINLAND' : this_radar_ids=['FIANJ','FIIKA','FIKOR','FIKUO','FILUO','FIUTA','FIVAN','FIVIM']
  'FRANCE' : this_radar_ids=['FRABB','FRALE','FRTRO','FRAVE','FRBLA','FRBOL','FRBOR','FRBOU','FRCAE','FRCHE','FRCOL','FRGRE','FRMOM','FRMTC','FRMCL','FRNAN','FRNIM']
  'IRELAND' : this_radar_ids=['IEDUB','IESHA']
  'NETHERLANDS' : this_radar_ids=['NLDBL','NLDHL']
  'THE NETHERLANDS' : this_radar_ids=['NLDBL','NLDHL']
  'POLAND' : this_radar_ids=['PLBRZ','PLGDA','PLLEG','PLPAS','PLPOZ','PLRAM','PLRZE','PLSWI']
  'NORWAY' : this_radar_ids=['NOAND','NOBML','NOHAS','NOHGB','NOHUR','NORSA','NORST','NOSTA']
  'SWEDEN' : this_radar_ids=['SEVIL','SEVAR','SEOVI','SEOSU','SELUL','SELEK','SEKKR','SEKIR','SEHUD','SEASE','SEARL','SEANG']
  'SLOVENIA' : this_radar_ids=['SILIS']
  'SLOVAKIA' : this_radar_ids=['SKMAJ','SKKOH']

  ELSE: this_radar_ids=['void']
  ENDCASE
  ;
  ;CHECK if it is already a radar_id
  IF WHERE(label eq all_ids) ge 0 THEN this_radar_ids=label
  ;
  ;IF something was found, update the radar_ids string array
  IF this_radar_ids[0] ne 'void' THEN BEGIN 
    ;
    IF new_id THEN BEGIN
      ;
      radar_ids=this_radar_ids
      new_id=0
      ;
    ENDIF ELSE radar_ids=[radar_ids,this_radar_ids]
    ;
  ENDIF
  ;
  ;EMPTY this_radar_ids for next loop
  this_radar_ids='void'
  ;
ENDFOR
;
;IF /PRINT is set, print a string array of radar_ids in IDL array format
IF KEYWORD_SET(print) THEN BEGIN

  the_radar_ids="["
  FOR i=0,N_ELEMENTS(radar_ids)-1 DO BEGIN
    the_radar_ids+=STRING(FORMAT='("''",a,"''")',radar_ids[i])
    IF i le N_ELEMENTS(radar_ids)-2 THEN the_radar_ids+=","
  ENDFOR
  the_radar_ids+="]"
  print, the_radar_ids


ENDIF

;RETURN the result. Might be empty for non-matching labels.
;
RETURN, radar_ids
;
END
