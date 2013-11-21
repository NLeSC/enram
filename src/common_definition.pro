PRO common_definition
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;Definition of standard parameters.                                          ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
COMMON constants,$

NSCANX,RADIUS43,RADIUS,HLAYER,NLAYER,NDATA,RANGMIN,RANGMINSTDEV,       $
RANGMAXSTDEV,RANGMAX,AZIMMIN,AZIMMAX,VRADMIN,NGAPBIN,NGAPMIN,   $
NDBZMIN,VDIFMAX,VMASKMAX,EMASKMAX,RHOMIN,ZDRMIN,DBZMIN,         $
DBZMAX,DBZNOISE,DBZRAIN,DBZCELL,STDEVCELL,AREACELL,CLUTPERCCELL,$
NEIGHBOURS,VTEXSCALE,VTEXOFFSET,STDEVSCALE,NTEXBINAZIM,         $
NTEXBINRANG,NTEXMIN,TEXCV,TEXSTDEV,DBZCLUTTER,DBZFACTOR,        $
SIGMABIRD,STDEVBIRD,XOFFSET,XSCALE,XMEAN

NSCANX=64L          ;Maximum number of elevation scans.
RADIUS43=8495.0     ;Earth radius used for height calculations.
RADIUS=6378.137     ;A second, different radius for the Earth (FIXME?)

;******************************************************************************
;*Defined (default) parameters.                                               *
;******************************************************************************
HLAYER      =0.200  ;Thickness of height layer in km.
NLAYER      =30L     ;Number of stacked height layers.
NDATA       =3L      ;Data dimension height layers.
RANGMIN     =5.0    ;Minimum range for WRWP analysis in km.
RANGMINSTDEV=5.0    ;Minimum range for WRWP determination for the
                    ;radial velocity st.dev. in km.
RANGMAXSTDEV=15.0   ;Maximum range for WRWP determination for the  FIXME?
                    ;radial velocity st.dev. in km.
RANGMAX     =25.0   ;Maximum range for WRWP analysis in km.
AZIMMIN     =0.0    ;Minimum azimuth for WRWP analysis in degrees.
AZIMMAX     =360.0  ;Maximum azimuth for WRWP analysis in degrees.
VRADMIN     =1.0    ;Minimum magnitude of velocity in m/s.
NGAPBIN     =8L     ;Number of bins where availability is analyzed.
NGAPMIN     =5L     ;Minimum number of points per analyses bin.
NDBZMIN     =25L    ;Minimum number of points necessary for valid
                    ;height layer reflectivity estimate
VDIFMAX     =10.0   ;Maximum deviation of radial velocity in m/s.
VMASKMAX    =0.0    ;A radial velocity difference with the
                ;neighbouring azimuth. Pixel lower than VMASKMAX
            ;leads to inclusion in rainmask (0=no mask).
EMASKMAX    =5.0    ;Border size in km to enlarge rainmask
RHOMIN      =0.9    ;Minimum correlation for precipitation classific
ZDRMIN      =3.0    ;Minimum differential reflectivity insects/chaff
DBZMIN      =-100.0 ;Minimum reflectivity used in calc of avg dBZ.
DBZMAX      =20.0   ;Maximum reflectivity used in calc of avg dBZ.
DBZNOISE    =-40.0  ;Noise level used for missing reflectivity data
DBZRAIN     =0.0    ;Minimum reflectivity for qualification as rain
DBZCELL     =15.0   ;Raincell reflectivity threshold
STDEVCELL   =5.0    ;Raincell local standard deviation threshold
AREACELL    =4L      ;Minimum area for a rain cell in pixels
CLUTPERCCELL=0.5    ;Maximum percentage of clutter in a cell
NEIGHBOURS  =5L      ;Minimum number of directly neighouring pixels
                ;with dBZ>DBZRAIN [1-8]
VTEXSCALE   =0.2    ;Increament of bitresolution for texture field
VTEXOFFSET  =-100.  ;
STDEVSCALE  =0.1    ;
NTEXBINAZIM =3L      ;Azimuth bins on which to base texture field
NTEXBINRANG =3L      ;Range bins on which to base texture field
NTEXMIN     =4L      ;Min necessary number of gates for texture
DBZCLUTTER  =-10.0  ;Minimum reflectivity for clutter
TEXCV       =1
TEXSTDEV    =2

DBZFACTOR   =335.4  ;Conversion factor Z to cm^2/km^3 at C-band  // FIXME?
SIGMABIRD   =10.0   ;Bird radar cross section in cm^2   FIXME?
STDEVBIRD   =2.   ;Min radial standard deviation in m/s for birds
XOFFSET     =5.0    ;Cross-section offset,sigma=XOFFSET+XSCALE/range
XSCALE      =134.0  ;Cross-section prefactor
XMEAN       =15.0   ;Reflectivities are scaled by XMEAN/sigma

;******************************************************************************

END
