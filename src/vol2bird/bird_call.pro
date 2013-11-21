;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Main routine for the bird_call program
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
PRO bird_call,h5infile,h5outfile,       $
definitions=definitions,                $
processed_datetime=processed_datetime,  $
cm=cm,                                  $
verbose=verbose,                        $
sigma_bird=sigma_bird,                  $
img=img,                                $
dualpol=dualpol,                        $
vad1=vad1,vad2=vad2,vvp1=vvp1,          $
vvp2=vvp2,vvp3=vvp3,                    $
tex1=tex1,tex2=tex2,                    $
vsignflip=vsignflip,                    $
txtoutfile=txtoutfile,                  $
append=append,                          $
success=success,                        $
help=help
;
success=0
;
IF KEYWORD_SET(help) THEN BEGIN
ENDIF
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;Definition of standard parameters.                                          ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
RESOLVE_ROUTINE, 'common_definition', /compile_full_file
common_definition
;
COMMON constants,$
  NSCANX,RADIUS43,RADIUS,HLAYER,NLAYER,NDATA,RANGMIN,RANGMINSTDEV,    $
  RANGMAXSTDEV,RANGMAX,AZIMMIN,AZIMMAX,VRADMIN,NGAPBIN,NGAPMIN,   $
  NDBZMIN,VDIFMAX,VMASKMAX,EMASKMAX,RHOMIN,ZDRMIN,DBZMIN,         $
  DBZMAX,DBZNOISE,DBZRAIN,DBZCELL,STDEVCELL,AREACELL,CLUTPERCCELL,$
  NEIGHBOURS,VTEXSCALE,VTEXOFFSET,STDEVSCALE,NTEXBINAZIM,         $
  NTEXBINRANG,NTEXMIN,TEXCV,TEXSTDEV,DBZCLUTTER,DBZFACTOR,        $
  SIGMABIRD,STDEVBIRD,XOFFSET,XSCALE,XMEAN
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;get the vol2bird.so library from the environment variable IDL_ENRAM_VOL2BIRDLIB
v2b_clib=GETENV('IDL_ENRAM_VOL2BIRDLIB')
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Analyse the definitions structure
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
compile_file_command=FILE_BASENAME(definitions.io_file,'.pro')
readscan_command    =compile_file_command+'_read_scan'
readcm_command      =compile_file_command+'_read_cluttermap'
writeprofile_command=compile_file_command+'_write_profile'
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;WRITE to stdout only if txtoutfile is not given
;
stdout = N_ELEMENTS(txtoutfile) eq 0 ? 1 : 0
IF N_ELEMENTS(append) ne 0 THEN BEGIN
  append=APPEND eq 0 ? 0 : 1
ENDIF ELSE append=1
;
IF not stdout THEN BEGIN
  FILE_MKDIR,file_dirname(txtoutfile)
  OPENW,un0,/GET_LUN,txtoutfile,APPEND=APPEND
ENDIF
t0=SYSTIME(1)
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;;
;;In IDL UPPERCASE and lowercase parameter names are equal, so initialisations like
;;dbzcell=DBZCELL are skipped
;
;DEFINE a "NIL" parameter to substitute the C NULL pointer
NIL=-1b
missing=-9999.0

cmflag=0L
uzflag=1L
vflag=0L
zflag=0L
dualpolflag=0L
verboseflag=0L
xflag=0

Ndx=2L
Npar=3L
Nlev=0L

rmin=RANGMIN
rmax=RANGMAX
amin=AZIMMIN
amax=AZIMMAX

rminscan=REPLICATE(0.,NSCANX)
rmaxscan=REPLICATE(0.,NSCANX)
Npntmax=0L

rcellmax=RANGMAX+5
cellarea=AREACELL
clutcell=CLUTPERCCELL
vmin=VRADMIN
vdmax=VDIFMAX

emask=EMASKMAX
dBZx=DBZMAX

textype=TEXSTDEV
texscale=1L
texoffset=0L

vsign = KEYWORD_SET(vsignflip) ? -1L : 1L ;flip the radial velocity sign [off]
imgflag = KEYWORD_SET(img ) ? 1L : 0L
IF KEYWORD_SET(sigma_bird) THEN SIGMABIRD=sigma_bird
;
;;;test flags;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
IF KEYWORD_SET(vad1) THEN BEGIN
   funcs=svd_vad1func;
   Ndx=1;
   Npar=3;
   PRINT,"#Calculation of VAD1 profile from volume data."
ENDIF
IF KEYWORD_SET(vad2) THEN BEGIN
   funcs=svd_vad2func;
   Ndx=1;
   Npar=5;
   PRINT,"#Calculation of VAD2 profile from volume data."
ENDIF
IF KEYWORD_SET(vvp1) THEN BEGIN
   funcs=svd_vvp1func;
   Ndx=2;
   Npar=3;
   PRINT,"#Calculation of VVP1 profile from volume data."
ENDIF
IF KEYWORD_SET(vvp2) THEN BEGIN
   funcs=svd_vvp2func;
   Ndx=3;
   Npar=6;
   PRINT,"#Calculation of VVP2 profile from volume data."
ENDIF
IF KEYWORD_SET(vvp3) THEN BEGIN
   funcs=svd_vvp3func;
   Ndx=4;
   Npar=9;
   PRINT,"#Calculation of VVP3 profile from volume data."
ENDIF
;
IF KEYWORD_SET(tex1) THEN BEGIN
   textype=TEXSTDEV;
   texoffset=0;
   texscale=STDEVSCALE;
ENDIF

IF KEYWORD_SET(tex2) THEN BEGIN
   textype=TEXCV;
   texoffset=VTEXOFFSET;
   texscale=VTEXSCALE;
ENDIF

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;SET the cluttermap
;cm='/usr/people/graafdem/FLYSAFE/development/cluttermaps/bsspapl_src/RAD_NL60_VOL_NA.h5'
;cm='/usr/people/graafdem/FLYSAFE/process/data/cluttermaps/RAD_NL61_CLU_NA_20080214-19.h5'
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;Opening and reading of HDF5 radar input file.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;COMPILE the io_file with read and write definitions
RESOLVE_ROUTINE,compile_file_command,/compile_full_file
;
IF N_ELEMENTS(h5infile) eq 0 or FILE_TEST(h5infile) eq 0 THEN BEGIN
  PRINT, 'Error: no valid HDF5 radar input file specified'
  IF not stdout THEN FREE_LUN,un0,/FORCE
  RETURN
ENDIF
;
openingh5_message=STRING(FORMAT= '("#Opening of HDF5 radar input file : ",a)',h5infile)
IF stdout THEN PRINT, openingh5_message ELSE PRINTF,un0,openingh5_message
;
;;Reading of radial velocity and reflectivity data of the selected scans.
CALL_PROCEDURE,readscan_command,h5infile,'V',vmeta,vscan,in,imgflag=imgflag,processed_datetime=processed_datetime,success=vflag
CALL_PROCEDURE,readscan_command,h5infile,'Z',zmeta,zscan,success=zflag
CALL_PROCEDURE,readscan_command,h5infile,'U',uzmeta,uzscan,success=uzflag
;CALL_PROCEDURE,readscan_command,h5infile,'W',wmeta,wscan   ;W is not used
;
IF vflag eq 0 or zflag eq 0 THEN BEGIN      ;IF either vscan or zscan is not available,
  IF vflag eq 0 THEN message='VRAD not found'   ;the bird profile cannot be determined
  IF zflag eq 0 THEN message='DBZH not found'   ;
  message+=' in file '+h5infile
  PRINT, message
  IF not stdout THEN FREE_LUN,un0,/FORCE
  RETURN
ENDIF
;
IF uzflag eq 0 THEN BEGIN       ;uZ (uncorrected Z is only used in classification
  uzmeta=zmeta                  ;to classify unvalid corrected Z (by signal
  uzscan=zscan                  ;processor) as clutter. This will fail when uZ=Z and
ENDIF                           ;no extra clutter will be generated. (besides uzflag<>1)
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;OPEN the cluttermap, if given
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
IF N_ELEMENTS(cm) eq 0 or FILE_TEST(cm) eq 0 THEN BEGIN
  PRINT, 'Error: no valid cluttermap file specified'
  IF not stdout THEN FREE_LUN,un0,/FORCE
  RETURN
ENDIF
;
IF N_ELEMENTS(cm) ne 0 THEN BEGIN
  ;
  openingh5_message=STRING(FORMAT= '("#Opening of HDF5 clutter map file : ",a)',cm)
  IF stdout THEN PRINT, openingh5_message ELSE PRINTF,un0,openingh5_message
  CALL_PROCEDURE,readcm_command,cm,cmmeta,cmscan
  cmflag = 1
ENDIF ELSE BEGIN
  cmmeta=zmeta
  cmscan=zscan
  cmflag=0
ENDELSE
;
;if the output file is not specified, create one from the
;time/date info
;IF N_ELEMENTS(h5outfile) eq 0 THEN BEGIN
;  YYYYMMDD=STRING(FORMAT="(i8)",zmeta[1].date)
;  HRMI=STRING(FORMAT="(i04)",zmeta[1].time/100)
;  radar_id=zmeta[1].id
;  h5outfile='RAD_'+radar_id+'_PRF_NA_'+YYYYMMDD+HRMI+'.h5'
;ENDIF ELSE  h5outfile=STRTRIM(h5outfile,2)
;;
;print, "READING HDF5 :" +STRTRIM(SYSTIME(1)-t0,2)+" s"
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;Printing metadata of selected scans to stdout.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
header=STRING("#YYYYMMDD HHMMSS Elev Nrang Nazim Rscale Ascale Voffset Vscale Zoffset Zscale")
IF stdout THEN PRINT, header ELSE PRINTF,un0,header
;
Nscan=vscan.(0)
FOR i=1,Nscan DO BEGIN
  line=STRING(FORMAT='("#",i8," ",i06,f5.1,i6,i6,f7.3,f7.3,f8.2,f7.2,f8.2,f7.2)',vmeta[i].date, $
     vmeta[i].time,vmeta[i].elev,vmeta[i].nrang,vmeta[i].nazim,vmeta[i].rscale,     $
     vmeta[i].ascale,vmeta[i].zoffset,vmeta[i].zscale,zmeta[i].zoffset,zmeta[i].zscale)
  IF stdout THEN PRINT, line ELSE PRINTF,un0, line
  ;
ENDFOR
;
;;Finalizing rmin/rmax arrays.*/
rminscan[WHERE(rminscan eq 0.)]=rmin
rmaxscan[WHERE(rmaxscan eq 0.)]=rmax
;
;;Initializing profile arrays.*/
udata=REPLICATE(!VALUES.F_NAN,NDATA,NLAYER)
hdata=REPLICATE(!VALUES.F_NAN,NDATA,NLAYER)
vdata=REPLICATE(!VALUES.F_NAN,NDATA,NLAYER)
wdata=REPLICATE(!VALUES.F_NAN,NDATA,NLAYER)
xdata=REPLICATE(!VALUES.F_NAN,NDATA,NLAYER)
zdata=REPLICATE(!VALUES.F_NAN,NDATA,NLAYER)
cdata=REPLICATE(!VALUES.F_NAN,NDATA,NLAYER)
etabird=REPLICATE(!VALUES.F_NAN,NLAYER)
rhobird=REPLICATE(!VALUES.F_NAN,NLAYER)
nzdata=REPLICATE(0L,NDATA,NLAYER)
nndata=REPLICATE(0L,NDATA,NLAYER)   ;ndata in C, but in IDL ndata==NDATA
;
fracclut=REPLICATE(!VALUES.F_NAN,NLAYER)
fracrain=REPLICATE(!VALUES.F_NAN,NLAYER)
fracbird=REPLICATE(!VALUES.F_NAN,NLAYER)
fracfringe=REPLICATE(!VALUES.F_NAN,NLAYER)
;
ff=REPLICATE(0.,NDATA,NLAYER)
dd=REPLICATE(0.,NDATA,NLAYER)
;
texmeta=vmeta
texmeta.zscale=texscale
texmeta.zoffset=texoffset
;
vtex=vscan
vtex2=vtex
;
;cellmap should be structure of LONGs of size zscan
cellmap = CREATE_STRUCT("cellmap_n",nscan)
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;Compute velocity texture field
;Compute precipitation cellmap for PPIs
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
FOR id=1L,Nscan DO BEGIN
   ;
   vtex.(id) *=0B           ;initialise
   vtex2.(id)*=0B
   ;
   ;cellmap should be structure of LONGs of size zscan
   cellmap = CREATE_STRUCT(cellmap,"cellmap_"+STRTRIM(id,2),LONG(zscan.(id))*0L)
   ;
   thevtex=vtex.(id)        ;array to pass
   ;
   c=call_external(v2b_clib,                $
      'call_texture',/PORTABLE,/UNLOAD,     $
      thevtex,vscan.(id),zscan.(id),            $
      texmeta[id],vmeta[id],zmeta[id],          $
      ntexbinrang,ntexbinazim,ntexmin,TEXSTDEV)
   ;
   vtex.(id)=thevtex        ;Store results
   ;
   IF KEYWORD_SET(dualpolflag) THEN BEGIN
   ;
   thecellmap=cellmap.(id) ;array to pass
   ;
   Ncell=call_external(v2b_clib,                    $
     'call_findcells',/PORTABLE,/UNLOAD,            $
     zscan.(id),rscan.(id),zdrscan.(id),thecellmap, $
         zmeta[id],rmeta[id],zdrmeta[id],dbzrain,rhomin,$
     zdrmin,dBZmin,rcellmax,-1)
   ;
   cellmap.(id)=thecellmap  ;Store results
   ;
   ENDIF ELSE BEGIN
     ;
     IF textype eq TEXCV THEN BEGIN
        ;
        thevtex=vtex2.(id)
    ;
    c=call_external(v2b_clib,               $
           'call_texture',/PORTABLE,/UNLOAD,    $
           thevtex,vscan.(id),zscan.(id),       $
           texmeta[id],vmeta[id],zmeta[id],     $
           ntexbinrang,ntexbinazim,ntexmin,TEXCV)
        ;
        vtex2.(id)=thevtex
        ;
    thecellmap=cellmap.(id) ;array to pass
    cv=0
    ;
    c=call_external(v2b_clib,               $
           'call_findcells',/PORTABLE,/UNLOAD,  $
           thevtex,NIL,NIL,thecellmap,      $
           texmeta[id],NIL,NIL,cv,rhomin,       $
           zdrmin,dBZmin,rcellmax,1)
        ;
        cellmap.(id)=thecellmap ;Store results
        ;
     ENDIF ELSE BEGIN
        ;
    thecellmap=cellmap.(id) ;array to pass
    ;
    Ncell=call_external(v2b_clib,       $
          'call_findcells',/PORTABLE,/UNLOAD,   $
          zscan.(id),NIL,NIL,thecellmap,        $
          zmeta[id],NIL,NIL,dbzrain,rhomin,     $
          zdrmin,dBZmin,rcellmax,-1)
        ;
        cellmap.(id)=thecellmap ;Store results
        ;
     ENDELSE
     ;
   ENDELSE
   ;
   thecellmap=cellmap.(id) ;array to pass
   ;
   NcellValid=call_external(v2b_clib,           $
     'call_analysecells',/PORTABLE,/UNLOAD,         $
     zscan.(id),vscan.(id),vtex.(id),cmscan.(id),   $
     thecellmap,zmeta[id],vmeta[id],texmeta[id],    $
     cmmeta[id],Ncell,cellarea,dbzcell,stdevcell,   $
     clutcell,vmin,dbzclutter,cmflag,dualpolflag,   $
     verboseflag);
   ;
   cellmap.(id)=thecellmap  ;Store results
   ;
ENDFOR
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;increase cellmap indices by 3 and change index -1 (nodata) to 0
;Add pixels with dBZ>dBZx to cellmap, using index NcellValid+1 0
;Now nodata=0,fringe=1,>dBZx=2,cell1=3,cell2=4, etc ...
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
FOR id=1L,Nscan DO BEGIN
   ;
   thecellmap=cellmap.(id) ;array to pass
   ;
   void=call_external(v2b_clib,             $
    'call_fringecells',/PORTABLE,/UNLOAD,       $
    thecellmap,zmeta[id],zscan.(id),            $
    emask,DBZx,id)
   ;
   cellmap.(id)=thecellmap  ;Store results
   ;
ENDFOR
;
;DETERMINE the classification of each range gate.
;
FOR l=0L,NLAYER-1 DO BEGIN
   ;
   Npnt=0L
   Npntrain=0L
   NpntrainNoFringe=0L
   Npntclut=0L
   Npntall=0L
   n=0L
   ;
   height=(l+0.5)*HLAYER;
   FOR id=1L,Nscan DO BEGIN
      ;
      void=call_external(v2b_clib,      $
      'call_classification',/PORTABLE,/UNLOAD,$
      zmeta[id],vmeta[id],uzmeta[id],       $
      cmmeta[id],cellmap.(id),              $
      zscan.(id),vscan.(id),uzscan.(id),    $
      cmscan.(id),zdata,nzdata,             $
      fracclut,fracrain,fracbird,fracfringe,$
      rminscan[id],rmaxscan[id],            $
      HLAYER,XOFFSET,XSCALE,XMEAN,height,   $
      amin,amax,vmin,dbzclutter,dBZmin,dBZx,$
      DBZNOISE,NGAPMIN,NGAPBIN,NDBZMIN,     $
      l,id,n,Npnt,Npntall,Npntclut,Npntrain,$
      NpntrainNoFringe,                     $
      cmflag,uzflag,xflag)
       ;
   ENDFOR
   ;
   nzdata[0,l]=Npntall-Npntrain-Npntclut;
   nzdata[1,l]=NpntrainNoFringe;
   nzdata[2,l]=Npntall-Npntclut;
   IF (Npntall>0) THEN BEGIN
      fracclut[l]=1.*Npntclut/Npntall
      fracrain[l]=1.*NpntrainNoFringe/Npntall
      fracbird[l]=1.*nzdata[0,l]/Npntall
      fracfringe[l]=1.*(Npntrain-NpntrainNoFringe)/Npntall;
   ENDIF
   ;
   IF (Npnt gt NDBZMIN) THEN zdata[0,l]=10*ALOG(zdata[0,l]/nzdata[0,l])/ALOG(10) $
   ELSE zdata[0,l]=!VALUES.F_NAN;
   IF (Npntrain gt NDBZMIN) THEN zdata[1,l]=10*ALOG(zdata[1,l]/nzdata[1,l])/ALOG(10) $
   ELSE zdata[1,l]=!VALUES.F_NAN;
   IF (Npnt+Npntrain gt NGAPMIN*NGAPBIN) THEN zdata[2,l]=10*ALOG(zdata[2,l]/nzdata[2,l])/ALOG(10) $
   ELSE zdata[2,l]=!VALUES.F_NAN;
   IF (n gt Npntmax) THEN Npntmax=n;
   ;
ENDFOR
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;Calculation of wind profile using VAD (Ndx=1) or using VVP (Ndx>1).
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
x=REPLICATE(0.,Ndx*Npntmax)
y=REPLICATE(0.,Npntmax)
yfit=REPLICATE(0.,Npntmax)
c=REPLICATE(0L,Npntmax)
;
IF Ndx eq 1 THEN BEGIN
  ;
  FOR l=0L,NLAYER-1 DO BEGIN
     height=(l+0.5)*HLAYER;
     FOR id=1L,Nscan DO BEGIN
     ;
     void=call_external(v2b_clib,       $
     'call_vap',/PORTABLE,/UNLOAD,          $
     vmeta[id],cellmap.(id),vscan.(id),     $
     udata,vdata,wdata,xdata,ndata,cdata,   $
     Ndx,Npnt,Npntall,Npntclut,Npntrain,    $
     NpntrainNoFringe,Npntmax,              $
     rminscan[id],rmaxscan[id],         $
     HLAYER,height,                         $
     l,id,n)

     ENDFOR
  ENDFOR
  ;
ENDIF ELSE BEGIN
  ;
  FOR k=0L,NDATA-1 DO BEGIN
    FOR l=0L,NLAYER-1 DO BEGIN
       height=(l+0.5)*HLAYER;
       n=0L
       FOR id=1,Nscan DO BEGIN
       ;
       void=call_external(v2b_clib,$
       'call_vvp',/PORTABLE,/UNLOAD,$
       vmeta[id],vscan.(id),        $
       x,y,c,cellmap.(id),          $
       Ndx,Npntmax,NGAPBIN,         $
       rminscan[id],rmaxscan[id],   $
       HLAYER,height,vmin,          $
       k,l,id,n)
       ;
       ENDFOR
       Npnt=n;
       ;
       void=call_external(v2b_clib,$
       'call_fit',/PORTABLE,/UNLOAD,$
       udata,vdata,wdata,xdata,     $
       cdata,nndata,x,y,yfit,c,     $
       Ndx,Npar,Npnt,Npntmax,       $
       ngapmin,NGAPBIN,vdmax,vsign, $
       k,l)
       ;
    ENDFOR
  ENDFOR
  ;
ENDELSE
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;Removing height layers without any data.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
n=0
FOR l=0,NLAYER-1 DO BEGIN
   IF (nndata[0,l] le Npar && nndata[1,l] le Npar && nndata[2,l] le Npar) THEN CONTINUE;
   FOR k=0,NDATA-1 DO BEGIN
      udata[k,n]=udata[k,l];
      vdata[k,n]=vdata[k,l];
      wdata[k,n]=wdata[k,l];
      xdata[k,n]=xdata[k,l];
      zdata[k,n]=zdata[k,l];
     nndata[k,n]=nndata[k,l];
      cdata[k,n]=cdata[k,l];
      hdata[k,n]=(l+0.5)*HLAYER;
      IF (nndata[k,n] eq 0) THEN BEGIN
      zdata[k,n]=!VALUES.F_NAN;
      ENDIF
   ENDFOR
   ;
   fracclut  [n]=fracclut[l];
   fracrain  [n]=fracrain[l];
   fracfringe[n]=fracfringe[l];
   fracbird  [n]=fracbird[l];
   n++;
   if (n gt Nlev) THEN Nlev=n;
ENDFOR
void=check_math()
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;;Printing of profile to stdout.*/
;
header=STRING("#Date    Time Heig Uwind  Vwind  Speed Direc Wwind  Count StdDev dBZAll dBZBird Precip")
IF stdout THEN PRINT, header ELSE PRINTF,un0,header
FOR l=0,Nlev-1 DO BEGIN
   FOR k=0,NDATA-1 DO BEGIN
      ff[k,l]=sqrt(udata[k,l]*udata[k,l]+vdata[k,l]*vdata[k,l]);
      dd[k,l]=atan2(-udata[k,l],-vdata[k,l])*!RADEG;
      IF (dd[k,l] lt 0) THEN  dd[k,l]+=360;
   ENDFOR

   line=STRING(FORMAT='(i8," ",i04," ",f4.1," ",f6.2," ",f6.2," ",f5.2," ",f5.1," ",f6.2," ",i5," ",3(f6.2," "),f4.2)',$
  vmeta[1].date,vmeta[1].time/100,hdata[2,l],udata[0,l],$
  vdata[0,l],ff[0],dd[0],wdata[0,l],nndata[2,l],        $
  xdata[2,l],zdata[2,l],zdata[0,l],cdata[2,l])
  ;
  IF stdout THEN PRINT, line ELSE PRINTF,un0,line
ENDFOR
;
;Converting reflectivity factor to bird reflectivity*/
;
FOR l=0,Nlev-1 DO BEGIN
  etabird[l]= (xdata[0,l] gt stdevbird) ? DBZFACTOR*10^(zdata[0,l]/10) : 0;
  rhobird[l]= (xdata[0,l] gt stdevbird) ? etabird[l]/SIGMABIRD : 0;
ENDFOR
;
;CREATE the HDF5 file
;
CALL_PROCEDURE, writeprofile_command,       $
h5outfile,in,                               $
success=success,                            $
vmeta,zmeta,uzmeta,                         $
vscan,zscan,uZscan,cellmap,vtex,        $
udata,vdata,wdata,xdata,zdata,nndata,nzdata,$
cdata,hdata,dd,ff,etabird,rhobird,          $
fracbird,fracclut,fracfringe,fracrain,      $
VTEXSCALE,VTEXOFFSET,NLEV,MISSING,imgflag
;
openingh5_message=STRING(FORMAT= '("#Opening of HDF5 radar output file: ",a)',h5outfile)
IF stdout THEN PRINT, openingh5_message ELSE PRINTF,un0,openingh5_message
;
void=check_math()
;
IF not stdout THEN FREE_LUN,un0
t1=SYSTIME(1)

;print, "Total :" +STRTRIM(SYSTIME(1)-t0,2)+" s"
END
