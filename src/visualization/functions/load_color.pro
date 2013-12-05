;
; Copyright 2013 Netherlands eScience Center
;
; Licensed under the Apache License, Version 2.0 (the "License");
; you may not use this file except in compliance with the License.
; You may obtain a copy of the License at
;
; http://www.apache.org/licenses/LICENSE-2.0
;
; Unless required by applicable law or agreed to in writing, software
; distributed under the License is distributed on an "AS IS" BASIS,
; WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
; See the License for the specific language governing permissions and
; limitations under the License.
;

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
