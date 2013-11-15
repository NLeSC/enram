function SUNRISE, juldate,twilight=twilight
;this function gives UTC sunrise, sunset, civil twilight start, civil twilight end for given Julian Date
CALDAT, juldate, Month, Day, Year, Hour, Minute, Second 
sunwait="sunwait -z -p 52.1007N 5.1774E"
ops=" -y "+string(year)+" -m "+string(month)+" -d "+string(day)
spawn,sunwait+ops,out
if keyword_set(twilight) then begin
 risehour=strmid(out[13],29,2)
 risemin =strmid(out[13],31,2)
 sethour =strmid(out[13],44,2)
 setmin  =strmid(out[13],46,2)
endif else begin
 risehour=strmid(out[12],29,2)
 risemin =strmid(out[12],31,2)
 sethour =strmid(out[12],44,2)
 setmin  =strmid(out[12],46,2)
endelse
return,[JULDAY(Month, Day, Year, risehour, risemin, 0),JULDAY(Month, Day, Year, sethour, setmin, 0)]
end
