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

function READVP2, filename, VERBOSE=verbose

  templateFileStr = GETENV('IDL_ENRAM_TEMPLATES')
  restore,templateFileStr
  
  data=read_ascii(filename, template=weather_template)
  juldates=julday(strmid(data.date,4,2),strmid(data.date,6,2),$
         strmid(data.date,0,4),strmid(data.time,0,2),strmid(data.time,2,2))
  caldat, juldates, months,days,years,hours,minutes

  ; FIXME we don't have ~/IDL/flysafe/templates.sav

  ;select only the hh:mm = hh:02 datapoints
  ;note that the minutes are dropped in the generation of index, i.e. effectively  put to zero
;  i=where(minutes EQ 2)
;  i=where(minutes EQ 2 OR minutes EQ 32)
  i=DINDGEN(SIZE(juldates,/dim))
  record={datvp2,juldate:juldates[0],strdate:data.date[0],month:months[0],day:days[0],year:years[0],$
      hour:hours[0],min:minutes[0],height:data.height[0],$
      speed:data.speed[0],direc:data.direc[0],u:data.u[0],v:data.v[0],w:data.w[0],$
      count:data.count[0],stdev:data.stdev[0],dbzraw:data.dbzraw[0],dbz:data.dbz[0],precip:data.precip[0],$
      ninsect:0,density:0.,mdg:!values.f_nan,rmdg:!values.f_nan,vg:!values.f_nan,sdvg:!values.f_nan,$
      sigma:0.,mtr:-1.,nbird:0.,index:long64(0)}
  data2=replicate(record,size(i,/n_elements))
  data2.juldate=juldates[i]
  data2.strdate=data.date[i]
  data2.month=months[i]
  data2.day=days[i]
  data2.year=years[i]
  data2.hour=hours[i]
  data2.min=minutes[i]
  data2.height=data.height[i]
  data2.speed=data.speed[i]
  data2.direc=data.direc[i]
  data2.u=data.u[i]
  data2.v=data.v[i]
  data2.w=data.w[i]
  data2.count=data.count[i]
  data2.stdev=data.stdev[i]
  data2.dbzraw=data.dbzraw[i]
  data2.dbz=data.dbz[i]
  data2.precip=data.precip[i]
  ;no need to sort density, mtr, nbird and ninsect, since these are still empty
  data2.index=10000000000*long64(years[i])+100000000*long64(months[i])+1000000*long64(days[i])+10000*long64(hours[i])+$
      long64(1000*data.height[i])     ;         +long64(minutes[i]))}

  ;sort the data by year-day-hour-height-minutes (i.e. by index)
  sortindex=SORT(data2.index)
  data2.juldate=(data2.juldate)[sortindex]
  data2.strdate=(data2.strdate)[sortindex]
  data2.month=(data2.month)[sortindex]
  data2.day=(data2.day)[sortindex]
  data2.year=(data2.year)[sortindex]
  data2.hour=(data2.hour)[sortindex]
  data2.min=(data2.min)[sortindex]
  data2.height=(data2.height)[sortindex]
  data2.speed=(data2.speed)[sortindex]
  data2.direc=(data2.direc)[sortindex]
  data2.u=(data2.u)[sortindex]
  data2.v=(data2.v)[sortindex]
  data2.w=(data2.w)[sortindex]
  data2.count=(data2.count)[sortindex]
  data2.stdev=(data2.stdev)[sortindex]
  data2.dbzraw=(data2.dbzraw)[sortindex]
  data2.dbz=(data2.dbz)[sortindex]
  data2.precip=(data2.precip)[sortindex]
  ;no need to sort density, mtr, nbird and ninsect, since these are still empty
  data2.index=(data2.index)[sortindex]

  ;print structure to stdout
  IF KEYWORD_SET(VERBOSE) THEN print,'read DATVP2 structure',n_elements(data2),' records...'

  return,data2
end
