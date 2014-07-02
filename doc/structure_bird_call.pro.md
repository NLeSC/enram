### bird_call structure  

    for iScan=1:nScans
        initialize cellmap
        
        call_texture
        
        IF KEYWORD_SET(dualpolflag) THEN BEGIN
        
            call_findcells
        
        ENDIF ELSE BEGIN
        
            IF textype eq TEXCV THEN BEGIN

            call_texture
            call_findcells
        
            ENDIF ELSE BEGIN
                
                call_findcells
                
            ENDELSE
            
        ENDELSE
        
        call_analysecells
        
    ENDFOR
    
    
    
simplest case (no polarity, texture=STDEV): 

    1. texture
    1. find_cells
    1. analysecells
    