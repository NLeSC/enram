- test Java library with OpeNDAP over internet
- read up on BALTRAD
- either implement the full specification of the ODIM scan standard (see [here](https://www.wmo.int/pages/prog/www/OSY/Meetings/ET-SBO_Workshop_Radar_Data_Ex/SBO-WxR_Exchange_3.1.6_NGaussiat.pdf)), or see if a class already exists from which my RadarScanJava class can inherit
- install  BALTRAD software in VM, see what's what



# test

* <font color="#F00"> write unit test for svdfit using all-C code except for the top level</font>

## Java

* create BirdDensityProfileJava instance
    * read DBZ and VRAD from local files (ignore anything related to polarity for now)

* calculate local standard deviation of VRAD using .calcTexture() method
    
* identify meteo cells using .findCells() method
    
* calculate statistics of each cell using .analyzeCells() method
    * maybe print list of statistics?
    
* add a fringe to each cell using the .fringeCells() method
    
* classify cells? using the .classify() method

* calculate (elements of) bird density profile somehow
    * vvp 
    * fit 
    
* remove all stuff relating to polarity from C code base

## Matlab 

* create BirdDensityProfileMatlab instance
    * tie BirdDensityProfileJava to BirdDensityProfileMatlab as field

* visualize .texture field in Matlab using generic routines for plotting of polar data
    
* visualize .cellImage field in Matlab using generic routines for plotting of polar data
    
* visualize bird density profile


# plan

1. total number of hours available: <font color="#F00">__320__</font> (~2 months), starting September 1, 2014
1. make plan for the next 320 hours 
    1. decide on polarity issue [#41](https://github.com/NLeSC/enram/issues/41)
        * <font color="#888">_the C-code still includes lines to deal with polarity scans (these are not used in the current project, although it is foreseen that we will use polarity scan later on). Deleting these lines is necessary from a code quality and maintainability point-of-view._</font>
    1. develop unit test for svdfit
        * <font color="#888">_the fitting routine is currently untested (and very difficult to read --a recipe for undetected errors)._</font>
    1. collect the data subset that serves as input to svdfit
        1. create new class that includes a bunch of RadarScans, together making up a volume scan
            * <font color="#888">_most of the work done by vol2bird is relatively simple image processing based on individual scans. The bird density profile, however, is based on multiple scans (a so-called volume scan)._</font>
    1. call the different vol2bird methods and create a bird density profile
    1. let Adriaan verify the pipeline 
    1. the current pipeline uses data from the CZBRD station. Other stations/countries use slightly different formats, even within the ODIM-HDF5 standard. Make code to deal with this
    1. test if pipeline works for many different circumstances (many different dates and stations).
    1. make tools to interact with data and to visualize intermediate results
    1. discuss with BALTRAD people how to best go about implementing vol2bird within BALTRAD tool suite (either face to face or online, whatever best suits our needs)
    1. embed processing pipeline into BALTRAD suite
1. verify plan with Willem, maybe Adriaan, then notify third parties, notably BALTRAD, of our plans and timeline (check e-mail about Gunter Haase)
1. 