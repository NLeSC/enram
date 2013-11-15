;
;This file contains the settings for the various radar stations
;to be used for the volbird retrieval algoritm and related algorithms.
;
;USE this to set the radar characteristics for a certain site
;
PRO radar_definitions,radar_id,radar_definition
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;IO_PATH is the full path to the IDL io definitions
io_path='/usr/people/graafdem/FLYSAFE/idl/io/'
;
;RAW_DATA_PATH is the full path to the raw data, as delivered by the 
;various radar operatives. 
raw_data_path='/usr/people/graafdem/FLYSAFE/process/data/raw/'
;
;INPUT_DATA_PATH is the full path to the harmonized, ODIM format data
input_data_path='/usr/people/graafdem/FLYSAFE/process/data/odim/'
;
;BIRD_DATA_PATH is the full path to the output data directory
bird_data_path='/usr/people/graafdem/FLYSAFE/process/data/bird/'
;
;CLUTTER_DATA_PATH is the full path to the cluttermap files
clutter_data_path='/usr/people/graafdem/FLYSAFE/process/data/cluttermaps/'
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;DEFINE station dependent settings here
;

CASE STRUPCASE(radar_id) of 

'MYTST': BEGIN 

  radar_full_name='Test_my radar'
  radar_id       ='IKA_'
  path_id        ='MY_tst'
  io_file        ='vol2bird_fi_odimhdf5.pro'

END

'NLDBL': BEGIN 

  radar_full_name='The Netherlands_De Bilt'
  radar_id='NL60'
  path_id='NL_dbl'
  io_file='vol2bird_nl_odimhdf5.pro'

END

'NLDHL': BEGIN 

  radar_full_name='The Netherlands_Den Helder'
  radar_id='NL61'
  path_id='NL_dhl'
  io_file='vol2bird_nl_odimhdf5.pro'

END

'SEVIL': BEGIN 

  radar_full_name='Sweden_Vilebo'
  radar_id       ='sevil'
  path_id        ='SE_vil'
  io_file        ='vol2bird_se_odimhdf5.pro'

END

'SEVAR': BEGIN 

  radar_full_name='Sweden_Vara'
  radar_id       ='sevar'
  path_id        ='SE_var'
  io_file        ='vol2bird_se_odimhdf5.pro'

END

'SEOVI': BEGIN 

  radar_full_name='Sweden_Ornskoldsvik' ;Örnsköldsvik
  radar_id       ='seovi'
  path_id        ='SE_ovi'
  io_file        ='vol2bird_se_odimhdf5.pro'
  ascii_name     =[153,114,110,115,107,148,108,100,115,118,105,107]

END 

'SEOSU': BEGIN 

  radar_full_name='Sweden_Ostersund'  ;Östersund
  radar_id       ='seosu'
  path_id        ='SE_osu'
  io_file        ='vol2bird_se_odimhdf5.pro'
  ascii_name     =[153,115,116,101,114,115,117,110,100]

END

'SELUL': BEGIN 

  radar_full_name='Sweden_Lulea' ;Luleå
  radar_id       ='selul'
  path_id        ='SE_lul'
  io_file        ='vol2bird_se_odimhdf5.pro'
  ascii_name     =[76,117,108,101,134]

END

'SELEK': BEGIN 

  radar_full_name='Sweden_Leksand'
  radar_id       ='selek'
  path_id        ='SE_lek'
  io_file        ='vol2bird_se_odimhdf5.pro'

END

'SEKKR': BEGIN 

  radar_full_name='Sweden_Karlskrona'
  radar_id       ='sekkr'
  path_id        ='SE_kkr'
  io_file        ='vol2bird_se_odimhdf5.pro'

END

'SEKIR': BEGIN 

  radar_full_name='Sweden_Kiruna'
  radar_id       ='sekir'
  path_id        ='SE_kir'
  io_file        ='vol2bird_se_odimhdf5.pro'

END

'SEHUD': BEGIN 

  radar_full_name='Sweden_Hudiksvall'
  radar_id       ='sehud'
  path_id        ='SE_hud'
  io_file        ='vol2bird_se_odimhdf5.pro'

END

'SEASE': BEGIN 

  radar_full_name='Sweden_Ase'
  radar_id       ='sease'
  path_id        ='SE_ase'
  io_file        ='vol2bird_se_odimhdf5.pro'

END

'SEARL': BEGIN 

  radar_full_name='Sweden_Arlanda, Stockholm'
  radar_id       ='searl'
  path_id        ='SE_arl'
  io_file        ='vol2bird_se_odimhdf5.pro'

END

'SEANG': BEGIN 

  radar_full_name='Sweden_Angelholm' ;Ängelholm
  radar_id       ='seang'
  path_id        ='SE_ang'
  io_file        ='vol2bird_se_odimhdf5.pro'
  ascii_name     =[142,110,103,101,108,104,111,108,109]

END

'CZBRD': BEGIN 

  radar_full_name='Czech Republic_Praha Brdy'
  radar_id       ='60_C_OKPR'
  path_id        ='CZ_brd'
  io_file        ='vol2bird_cz_odimhdf5.pro'

END

'CZSKA': BEGIN 

  radar_full_name='Czech Republic_Skaly' ;Skály
  radar_id       ='50_C_OKPR'
  path_id        ='CZ_ska'
  io_file        ='vol2bird_cz_odimhdf5.pro'
  ascii_name     =[83,107,160,108,121]

END

'IESHA': BEGIN 

  radar_full_name='Ireland_Shannon'
  radar_id       ='40_C_EIDB'
  path_id        ='IE_sha'
  io_file        ='vol2bird_ie_odimhdf5.pro'

END

'IEDUB': BEGIN 

  radar_full_name='Ireland_Dublin'
  radar_id       ='41_C_EIDB'
  path_id        ='IE_dub'
  io_file        ='vol2bird_ie_odimhdf5.pro'

END

'HRBIL': BEGIN 

  radar_full_name='Croatia_Bilogora'
  radar_id       ='43_C_LDZM'
  path_id        ='HR_bil'
  io_file        ='vol2bird_hr_odimhdf5.pro'

END

'SILIS': BEGIN 

  radar_full_name='Slovenia_Lisca'
  radar_id       ='41_C_LJLM'
  path_id        ='SI_lis'
  io_file        ='vol2bird_si_odimhdf5.pro'

END

'SKMAJ': BEGIN 

  radar_full_name='Slovakia_Maly Javornik'
  radar_id       ='SK_MJ_'
  path_id        ='SK_maj'
  io_file        ='vol2bird_sk_odimhdf5.pro'

END

'SKKOH': BEGIN 

  radar_full_name='Slovakia_Kojsovska Hola'
  radar_id       ='SK_KH_'
  path_id        ='SK_koh'
  io_file        ='vol2bird_sk_odimhdf5.pro'

END

'FIANJ': BEGIN 

  radar_full_name='Finland_Anjalankoski'
  radar_id       ='ANJ_'
  path_id        ='FI_anj'
  io_file        ='vol2bird_fi_odimhdf5.pro'

END

'FIIKA': BEGIN 

  radar_full_name='Finland_Ikaalinen'
  radar_id       ='IKA_'
  path_id        ='FI_ika'
  io_file        ='vol2bird_fi_odimhdf5.pro'

END

'FIKOR': BEGIN 

  radar_full_name='Finland_Korpo'
  radar_id       ='KOR_'
  path_id        ='FI_kor'
  io_file        ='vol2bird_fi_odimhdf5.pro'

END

'FIKUO': BEGIN 

  radar_full_name='Finland_Kuopio'
  radar_id       ='KUO_'
  path_id        ='FI_kuo'
  io_file        ='vol2bird_fi_odimhdf5.pro'

END

'FILUO': BEGIN 

  radar_full_name='Finland_Luosto'
  radar_id       ='LUO_'
  path_id        ='FI_luo'
  io_file        ='vol2bird_fi_odimhdf5.pro'

END

'FIUTA': BEGIN 

  radar_full_name='Finland_Utajarvi' ;Utajärvi
  radar_id       ='UTA_'
  path_id        ='FI_uta'
  io_file        ='vol2bird_fi_odimhdf5.pro'
  ascii_name     =[85,116,97,106,132,114,118,105]

END

'FIVAN': BEGIN 

  radar_full_name='Finland_Vantaa' 
  radar_id       ='VAN_'
  path_id        ='FI_van'
  io_file        ='vol2bird_fi_odimhdf5.pro'

END

'FIVIM': BEGIN 

  radar_full_name='Finland_Vimpeli'
  radar_id       ='VIM_'
  path_id        ='FI_vim'
  io_file        ='vol2bird_fi_odimhdf5.pro'

END

'FRABB': BEGIN 

  radar_full_name='France_Abbeville'
  radar_id='T_PAGZ40_C_frabb'
  path_id='FR_abb'
  io_file='vol2bird_fr_odimhdf5.pro'

END

'FRALE': BEGIN 

  radar_full_name='France_Aleria';Aléria
  radar_id='T_PAGZ61_C_frale'
  path_id='FR_ale'
  io_file='vol2bird_fr_odimhdf5.pro'
  ascii_name     =[65,108,130,114,105,97]

END

'FRTRO': BEGIN 

  radar_full_name='France_Arcis';
  radar_id='T_PAGZ52_C_frtro'
  path_id='FR_tro'
  io_file='vol2bird_fr_odimhdf5.pro'

END

'FRAVE': BEGIN 

  radar_full_name='France_Avesnes'
  radar_id='T_PAGZ63_C_frave'
  path_id='FR_ave'
  io_file='vol2bird_fr_odimhdf5.pro'

END

'FRBLA': BEGIN 

  radar_full_name='France_Blaisy-Haut'
  radar_id='T_PAGZ65_C_frbla'
  path_id='FR_bla'
  io_file='vol2bird_fr_odimhdf5.pro'

END

'FRBOL': BEGIN 

  radar_full_name='France_Bollene' ;Bollène
  radar_id='T_PAGZ55_C_frbol'
  path_id='FR_bol'
  io_file='vol2bird_fr_odimhdf5.pro'
  ascii_name=[66,111,108,108,138,110,101]

END

'FRBOR': BEGIN 

  radar_full_name='France_Bordeaux' ;
  radar_id='T_PAGZ41_C_frbor'
  path_id='FR_bor'
  io_file='vol2bird_fr_odimhdf5.pro'

END

'FRBOU': BEGIN 

  radar_full_name='France_Bourges' ;
  radar_id='T_PAGZ42_C_frbou'
  path_id='FR_bou'
  io_file='vol2bird_fr_odimhdf5.pro'

END

'FRCAE': BEGIN 

  radar_full_name='France_Falaise' ;
  radar_id='T_PAGZ45_C_frcae'
  path_id='FR_cae'
  io_file='vol2bird_fr_odimhdf5.pro'

END

'FRCHE': BEGIN 

  radar_full_name='France_Cherves' ;
  radar_id='T_PAGZ64_C_frche'
  path_id='FR_che'
  io_file='vol2bird_fr_odimhdf5.pro'

END

'FRCOL': BEGIN 

  radar_full_name='France_Collobrieres' ;Collobrières
  radar_id='T_PAGZ59_C_frcol'
  path_id='FR_col'
  io_file='vol2bird_fr_odimhdf5.pro'
  ascii_name=[67,111,108,108,111,98,114,105,138,114,101,115]

END

'FRGRE': BEGIN 

  radar_full_name='France_Grezes' ;Grèzes
  radar_id='T_PAGZ44_C_frgre'
  path_id='FR_gre'
  io_file='vol2bird_fr_odimhdf5.pro'
  ascii_name=[71,114,138,122,101,115]

END

'FRMOM': BEGIN 

  radar_full_name='France_Momuy' ;
  radar_id='T_PAGZ66_C_frmom'
  path_id='FR_mom'
  io_file='vol2bird_fr_odimhdf5.pro'

END

'FRMTC': BEGIN 

  radar_full_name='France_Montancy' ;
  radar_id='T_PAGZ67_C_frmtc'
  path_id='FR_mtc'
  io_file='vol2bird_fr_odimhdf5.pro'

END

'FRMCL': BEGIN 

  radar_full_name='France_Montclar' ;
  radar_id='T_PAGZ62_C_frmcl'
  path_id='FR_mcl'
  io_file='vol2bird_fr_odimhdf5.pro'

END

'FRNAN': BEGIN 

  radar_full_name='France_Nancy' ;
  radar_id='T_PAGZ47_C_frnan'
  path_id='FR_nan'
  io_file='vol2bird_fr_odimhdf5.pro'

END

'FRNIM': BEGIN 

  radar_full_name='France_Nimes' ;Nîmes
  radar_id='T_PAGZ49_C_frnim'
  path_id='FR_nim'
  io_file='vol2bird_fr_odimhdf5.pro'
  ascii_name=[78,140,109,101,115]

END

'NOAND': BEGIN 

  radar_full_name='Norway_Andoya';Andøya
  radar_id       ='AND'
  path_id        ='NO_and'
  io_file        ='vol2bird_no_odimhdf5.pro'
  ascii_name     =[65,110,100,248,121,97]

END

'NOBML': BEGIN 

  radar_full_name='Norway_Bomlo';Bømlo
  radar_id       ='BML'
  path_id        ='NO_bml'
  io_file        ='vol2bird_no_odimhdf5.pro'
  ascii_name     =[66,248,109,108,111]

END

'NOHAS': BEGIN 

  radar_full_name='Norway_Hasvik'
  radar_id       ='HAS'
  path_id        ='NO_has'
  io_file        ='vol2bird_no_odimhdf5.pro'

END

'NOHGB': BEGIN 

  radar_full_name='Norway_Haegebostad';Hægebostad
  radar_id       ='HGB'
  path_id        ='NO_hgb'
  io_file        ='vol2bird_no_odimhdf5.pro'
  ascii_name     =[72,145,103,101,98,111,115,116,97,100]
END

'NOHUR': BEGIN 

  radar_full_name='Norway_Hurum'
  radar_id       ='HUR'
  path_id        ='NO_hur'
  io_file        ='vol2bird_no_odimhdf5.pro'

END

'NORSA': BEGIN 

  radar_full_name='Norway_Rissa'
  radar_id       ='RSA'
  path_id        ='NO_rsa'
  io_file        ='vol2bird_no_odimhdf5.pro'

END

'NORST': BEGIN 

  radar_full_name='Norway_Rost' ;Røst
  radar_id       ='RST'
  path_id        ='NO_rst'
  io_file        ='vol2bird_no_odimhdf5.pro'
  ascii_name     =[82,248,115,116]

END

'NOSTA': BEGIN 

  radar_full_name='Norway_Stadlandet'
  radar_id       ='STA'
  path_id        ='NO_sta'
  io_file        ='vol2bird_no_odimhdf5.pro'

END

'PLBRZ': BEGIN 

  radar_full_name='Poland_Brzuchania' ;
  radar_id       ='brz'
  path_id        ='PL_brz'
  io_file        ='vol2bird_pl_odimhdf5.pro'

END

'PLGDA': BEGIN 

  radar_full_name='Poland_Gdansk' ;
  radar_id       ='gda'
  path_id        ='PL_gda'
  io_file        ='vol2bird_pl_odimhdf5.pro'

END

'PLLEG': BEGIN 

  radar_full_name='Poland_Legionowo' ;
  radar_id       ='leg'
  path_id        ='PL_leg'
  io_file        ='vol2bird_pl_odimhdf5.pro'

END

'PLPAS': BEGIN 

  radar_full_name='Poland_Pastewnik' 
  radar_id       ='pas'
  path_id        ='PL_pas'
  io_file        ='vol2bird_pl_odimhdf5.pro'

END

'PLPOZ': BEGIN 

  radar_full_name='Poland_Poznan' 
  radar_id       ='poz'
  path_id        ='PL_poz'
  io_file        ='vol2bird_pl_odimhdf5.pro'

END

'PLRAM': BEGIN 

  radar_full_name='Poland_Ramza' 
  radar_id       ='ram'
  path_id        ='PL_ram'
  io_file        ='vol2bird_pl_odimhdf5.pro'

END

'PLRZE': BEGIN 

  radar_full_name='Poland_Rzeszow' 
  radar_id       ='rze'
  path_id        ='PL_rze'
  io_file        ='vol2bird_pl_odimhdf5.pro'

END

'PLSWI': BEGIN 

  radar_full_name='Poland_Swidwin' ;Swidwin
  radar_id       ='swi'
  path_id        ='PL_swi'
  io_file        ='vol2bird_pl_odimhdf5.pro'

END

ELSE : BEGIN

  MESSAGE, 'Radar id '+radar_id+' is not defined'

END
ENDCASE
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;ADD settings to radar_definition structure
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
radar_full_name   = N_ELEMENTS(radar_full_name)   eq 0 ? '' : radar_full_name
radar_id          = N_ELEMENTS(radar_id)	  eq 0 ? '' : radar_id
path_id           = N_ELEMENTS(path_id) 	  eq 0 ? '' : path_id
io_file           = N_ELEMENTS(io_file) 	  eq 0 ? '' : io_file
raw_data_path     = N_ELEMENTS(raw_data_path)	  eq 0 ? '' : raw_data_path
input_data_path   = N_ELEMENTS(input_data_path)   eq 0 ? '' : input_data_path
bird_data_path    = N_ELEMENTS(bird_data_path)	  eq 0 ? '' : bird_data_path
clutter_data_path = N_ELEMENTS(clutter_data_path) eq 0 ? '' : clutter_data_path
ascii_name        = N_ELEMENTS(ascii_name)        eq 0 ? '' : STRING(BYTE(ascii_name))
;
radar_definition = CREATE_STRUCT(     	    $
   'radar_full_name'	,radar_full_name,   $
   'radar_id'	    	,radar_id,  	    $
   'path_id'        	,path_id,	    $
   'io_file'	    	,io_path+io_file,   $
   'raw_data_path'      ,raw_data_path,	    $
   'input_data_path'    ,input_data_path,   $
   'bird_data_path'     ,bird_data_path,    $
   'clutter_data_path'	,clutter_data_path, $
   'ascii_name'     	,ascii_name 	    $
)
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
END
