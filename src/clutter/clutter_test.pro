clut_file='/nobackup_1/users/graafdem/DATA/FLYSAFE2/cluttermaps/SK_koh/RAD_SK_koh_CLT_15082011_11092011_MODE.h5'

RESOLVE_ROUTINE, 'vol2bird_clutter_hdf5', /compile_full_file

;OPEN clutterfile
;
openingh5_message=STRING(FORMAT= '("#Opening of HDF5 radar clutter file : ",a)',clut_file)
IF stdout THEN PRINT, openingh5_message ELSE PRINTF,un0,openingh5_message

vol2bird_clutter_hdf5_read_scan,clut_file,cmmeta,cmscan


END
