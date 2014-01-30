Function Atan2, arr1, arr2, zero = zr, deg = deg

; FIXME this is not the function Martin used
;print,'(atan2.pro) warning: this is not the function that Martin used during development'

;+
; NAME:
;	ATAN2
; PURPOSE:
;	Similar to the FORTRAN Intrinsic Function ATAN2
; CATEGORY:
;	Mathematical Function (general).
; CALLING SEQUENCE:
;	Result = ATAN2( ARR1, ARR2 [, /ZERO, /DEG])
;       ARR1 is typically the Y-value and ARR2 the X-value
;       Result is in the range -pi ... +pi
; INPUTS:
;    ARR1, ARR2
;	Arrays, type must be floating point or double precision
;       and the number of dimensions must be zero (scalar), one
;       (vector), or two (two-dimensional array).
; OPTIONAL INPUT PARAMETERS:
;	None.
; KEYWORD PARAMETERS:
;    /ZERO
;	If set, the normally undefined result when both ARR1 and
;       ARR2 are set to zero will give a result set to zero.
;    /DEG
;	Return results in degrees instead of radians
; OUTPUTS:
;	Returns the value of the FORTRAN intrinsic function ATAN2.
;       If an error, then returns the value 0 (byte).
; OPTIONAL OUTPUT PARAMETERS:
;	None.
; COMMON BLOCKS:
;	None.
; SIDE EFFECTS:
;	None.
; RESTRICTIONS:
;	None.
; PROCEDURE:
;	Uses the SIZE function to obtain information about the arrays.  
;	Compares, data types, the dimensions, and size of the 
;	dimension.
; MODIFICATION HISTORY:
;	Created 09-OCT-1994 by Roger J. Dejus, APS, ANL.
;	Some ideas adopted from Mati Meron's function arreq.
;-

; Check arrays
    fsiz = size(arr1)
    ssiz = size(arr2)
    if fsiz(0) eq ssiz(0) then ndim = fsiz(0) else begin
      message, 'Arrays must be same dimension', /continue
      return, 0b
    endelse
    if (ndim gt 2) then begin 
      message, 'Dimension must be 2D or less', /continue
      return, 0b
    endif
    if (total(abs(fsiz(0:ndim+1)-ssiz(0:ndim+1))) ne 0) then begin 
      message, 'Incompatible data types', /continue
      return, 0b
    endif
    ftyp = fsiz(ndim +1) ; styp is same as ftyp
    if (ftyp le 3 or ftyp ge 6) then begin
	message, 'Data type must be floating or double precision', $
		  /continue
	return, 0b
    endif

;  Determine # of elements for each dimension
    if (ndim le 1) then begin
      n1 = fsiz(ndim +2) ; # of elements (for scalar or vector)
      n2 = 0
    endif else begin ; two-dimensional array
      n1 = fsiz(1)   ; first dimension
      n2 = fsiz(2)   ; second dimension
    endelse
    if (ftyp eq 4) then begin ; floating point
      zero = 0.0
      pi   = !pi
      pi2  = !pi/2.0
      radeg= 180.0/pi
      cr   = fltarr(n1)
      if (n2 ge 1) then res = fltarr(n1,n2)
    endif else begin ; double precision
      zero = 0.0d0
      pi   = !dpi
      pi2  = !dpi/2.0d0
      radeg= 180.0d0/pi
      cr   = dblarr(n1)
      if (n2 ge 1) then res = dblarr(n1,n2)
    endelse

;  Two-dimensional arrays
    if (n2 ge 1) then begin
      for j=0,n2-1 do begin
        for i=0,n1-1 do begin
          x = arr2(i,j)
          y = arr1(i,j)

          if (x eq zero and y eq zero) then $
	    if (keyword_set(zr)) then begin
	      r = zero 
	    endif else begin
              message, 'Undefined result: zero divide by zero', /continue
	      return, 0b 
	    endelse $ 
          else if (x eq zero) then $
	    if (y gt zero) then r = pi2 else r = -pi2 $
          else if (y eq zero) then $
	    if (x gt zero) then r = zero else r = pi $
          else begin
	    r = atan(y/x)
            if (y gt zero) then begin
	      if (r lt zero) then r = r +pi 
	    endif else begin ; y < 0.0
	      if (r gt zero) then r = r -pi
	    endelse
          endelse
          cr(i) = r
        endfor ; i
        res(0,j) = cr
      endfor ; j

    endif else begin ; scalar or one-dimensional vector
      for i=0,n1-1 do begin
        x = arr2(i)
        y = arr1(i)

        if (x eq zero and y eq zero) then $
	  if (keyword_set(zr)) then begin
	    r = zero 
	  endif else begin
            message, 'Undefined result: zero divide by zero', /continue
	    return, 0b 
	  endelse $ 
        else if (x eq zero) then $
	  if (y gt zero) then r = pi2 else r = -pi2 $
        else if (y eq zero) then $
	  if (x gt zero) then r = zero else r = pi $
        else begin
	  r = atan(y/x)
          if (y gt zero) then begin
	    if (r lt zero) then r = r +pi 
	  endif else begin ; y < 0.0
	    if (r gt zero) then r = r -pi
	  endelse
        endelse
        cr(i) = r
      endfor ; i
      if (ndim eq 1) then res = cr else res = cr(0)
    endelse

    if (keyword_set(deg)) then res = res*radeg
return, res
end
