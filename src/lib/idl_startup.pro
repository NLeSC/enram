; set for gif file
 device, true=24, decomposed=0, retain=2
; device, decomposed=0, pseudo=8, retain=2

; Run the essential routines 
;.run ~/IDL/prog/print.pro
;.run ~/IDL/prog/colbar.pro
;.run ~/IDL/prog/loadcol.pro

; Commands to fix charsize problem on SGI machines (DFANNING,LGUMLEY)
; window, /free, /pixmap, XSize=10, YSize=10
; wdelete, !d.window
; device,Set_Character_Size=[8,12]
; Some Color Information
;print,'IN IDL MAIN DIRECTORY! Your number of colors is: ', $
;		!D.n_colors,!d.table_size
		
compile_opt DEFINT32, STRICTARR, STRICTARRSUBS

; Automatic Path creation
sep = ':'  ; path directory separator

!PATH = !PATH + path_sep(/search_path) + EXPAND_PATH('+' + '~/idl')

;increase saved lines in buffer
;!EDIT_INPUT=200

;some custom keys
define_key, 'HOME', /start_of_line
define_key, 'END', /end_of_line

;print,'Result of Expand Path: ',!Path
;print,' '
;printf,-1 , 'startup completed'




