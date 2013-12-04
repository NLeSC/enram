PRO load_color,triple,index
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;HANDLE colors explicitly here
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;TRIPLE = [r,g,b] color definition
;INDEX = color table index where TRIPLE will be loaded
;
IF N_ELEMENTS(TRIPLE) ne 3 THEN BEGIN
  PRINT, 'LOAD_COLOR: provided TRIPLE color definition should be a 3 element array'
  return
ENDIF
;
IF index lt 0 or index gt !D.TABLE_SIZE THEN BEGIN
  MESSAGE, 'LOAD_COLOR: Index is not within color table size'
ENDIF
;
;GET the current rgb color table arrays
TVLCT,R,G,B,/GET
;
;CHANGE the color at the index
R[index]=TRIPLE[0]
G[index]=TRIPLE[1]
B[index]=TRIPLE[2]
;
;LOAD the rgb color table arrays
TVLCT,R,G,B
;
END
