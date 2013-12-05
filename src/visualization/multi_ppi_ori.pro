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



files=[$
'/usr/people/graafdem/FLYSAFE/process/data/raw/NL_db/2011/08/15/00/RAD_NL60_VOL_NA_201108150000.h5' ,$
'/usr/people/graafdem/FLYSAFE/process/data/raw/NL_dh/2011/08/15/00/RAD_NL61_VOL_NA_201108150000.h5' ,$
'/usr/people/graafdem/FLYSAFE/process/data/raw/SK_koh/SK_KH_20110815000000.hdf'     	    	    ,$
'/usr/people/graafdem/FLYSAFE/process/data/raw/SK_maj/SK_MJ_20110815000000.hdf'     	    	    ,$
'/usr/people/graafdem/FLYSAFE/process/data/raw/SE_vil/20110815/sevil_20110815T000000Z.h5'	   ,$
'/usr/people/graafdem/FLYSAFE/process/data/raw/SE_var/20110815/sevar_20110815T000000Z.h5'	   ,$
'/usr/people/graafdem/FLYSAFE/process/data/raw/SE_osu/20110815/seosu_20110815T000000Z.h5'	   ,$
'/usr/people/graafdem/FLYSAFE/process/data/raw/SE_lul/20110815/selul_20110815T000000Z.h5'	   ,$
'/usr/people/graafdem/FLYSAFE/process/data/raw/SE_ovi/20110815/seovi_20110815T000000Z.h5'	   ,$
'/usr/people/graafdem/FLYSAFE/process/data/raw/SE_lek/20110815/selek_20110815T000000Z.h5'	   ,$
'/usr/people/graafdem/FLYSAFE/process/data/raw/SE_kkr/20110815/sekkr_20110815T000000Z.h5'	   ,$
'/usr/people/graafdem/FLYSAFE/process/data/raw/SE_kir/20110815/sekir_20110815T000000Z.h5'	   ,$
'/usr/people/graafdem/FLYSAFE/process/data/raw/SE_hud/20110815/sehud_20110815T000000Z.h5'	   ,$
'/usr/people/graafdem/FLYSAFE/process/data/raw/SE_ase/20110815/sease_20110815T000000Z.h5'	   ,$
'/usr/people/graafdem/FLYSAFE/process/data/raw/SE_arl/20110815/searl_20110815T000000Z.h5'	   ,$
'/usr/people/graafdem/FLYSAFE/process/data/raw/SE_ang/20110815/seang_20110815T000000Z.h5'	   ,$
'/usr/people/graafdem/FLYSAFE/process/data/raw/CZ_ska/hdf5/20110815/T_PAGZ50_C_OKPR_20110815000359.hdf',$
'/usr/people/graafdem/FLYSAFE/process/data/raw/CZ_brd/hdf5/20110815/T_PAGZ60_C_OKPR_20110815000447.hdf',$
'/usr/people/graafdem/FLYSAFE/process/data/raw/IE_sha/T_PAGZ40_C_EIDB_20110818080500.h5'	   ,$
'/usr/people/graafdem/FLYSAFE/process/data/raw/IE_dub/T_PAGZ41_C_EIDB_20110818080600.h5'	   ,$
'/usr/people/graafdem/FLYSAFE/process/data/raw/HR_bil/T_PAZZ43_C_LDZM_20111105000012.h5'    	   ,$
'/nobackup/users/graafdem/DATA/RADAR/FLYSAFE2/raw/SI_lis/T_PAZZ41_C_LJLM_20110815000006.h5' 	    $
]
; FIXME


radar_ids = [$
	     'NLDBL','NLDHL',				            	$
	     'SKMAJ','SKKOH',	    	    	    	    	    	$
	     'SEVIL','SEVAR','SEOSU','SELUL','SEOVI',		        $
	     'SELEK','SEKKR','SEKIR','SEHUD','SEASE','SEARL','SEANG', 	$
    	     'CZSKA','CZBRD',					        $
	     'IESHA','IEDUB',					     	$
	     'HRBIL',     	    	    	    	    	    	$
	     'SILIS']

files_ids = INDGEN(N_ELEMENTS(radar_ids))


IF KEYWORD_SET(definitions) THEN dumy=TEMPORARY(definitions)
FOR ifile=0,N_ELEMENTS(files)-1 DO BEGIN
  radar_definitions,radar_ids[files_ids[ifile]],radar_definition
  definitions= KEYWORD_SET(definitions) ? [definitions,radar_definition] : radar_definition
ENDFOR

scan=1
grid=1

plot_lon=0
plot_lat=45
limit=[40,-15,70,40]

map_ppi,files,limit,$
definitions=definitions,$
coast=0,$
bg=1,$
plot_lon=plot_lon,$
plot_lat=plot_lat,$
scan=scan,$
grid=grid,$
radar_cross=20

END
