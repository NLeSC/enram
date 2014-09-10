
# plan

total number of hours available: <font color="#F00">__320__</font> (~2 months), starting September 1, 2014

* make plan for the next 320 hours 
    1. decide on polarity issue [#41](https://github.com/NLeSC/enram/issues/41)
        * <font color="#888">_the C-code still includes lines to deal with polarity scans (these are not used in the current project, although it is foreseen that we will use polarity scans later on). Deleting these lines is necessary from a code quality and maintainability point-of-view._</font>
    1. <font color="#F00"> write unit test for svdfit using all-C code except for the top level</font>
        * <font color="#888">_by using all-C code except for the top level, one can avoid having JNI deal with C function pointers._</font>
        * <font color="#888">_note that the fitting routine is currently untested (and very difficult to read --a recipe for undetected errors)._</font>
    1. collect the data subset that serves as input to svdfit
        1. create new class that includes a bunch of RadarScans, together making up a volume scan
            * <font color="#888">_most of the work done by vol2bird is relatively simple image processing based on individual scans. The bird density profile, however, is based on multiple scans (a so-called volume scan)._</font>
    1. make Java infrastructure that lets you retrieve results (calculated in Java) so that you can visualize and check them from Matlab.
    1. call the different vol2bird methods and create a bird density profile
    1. let Adriaan verify the pipeline 
    1. read up on BALTRAD
    1. the current pipeline uses data from the CZBRD station. Other stations/countries use slightly different formats, even within the ODIM-HDF5 standard. Make code to deal with this
        1. see if a class already exists in the BALTRAD tool suite from which my RadarScanJava class can inherit, or implement the full specification of the ODIM scan standard (see [here](https://www.wmo.int/pages/prog/www/OSY/Meetings/ET-SBO_Workshop_Radar_Data_Ex/SBO-WxR_Exchange_3.1.6_NGaussiat.pdf))
    1. test if pipeline works for many different circumstances (many different dates and stations).
    1. make tools to interact with data and to visualize intermediate results
    1. test Java NetCDF library with OpeNDAP over internet
    1. install BALTRAD software in VM, see what's what
    1. discuss with BALTRAD people how to best go about implementing vol2bird within BALTRAD tool suite (either face to face or online, whatever best suits our needs)
    1. embed processing pipeline into BALTRAD suite
* verify plan with Willem, maybe Adriaan, then notify third parties, notably BALTRAD, of our plans and timeline (check e-mail about Gunter Haase)
