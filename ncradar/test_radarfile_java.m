clear
close all
clc



javaaddpath('./bin')  
% ...and import its contents
import nl.esciencecenter.ncradar.*

javaaddpath('./lib/netcdfAll-4.3.jar')

rf = RadarFile('/home/daisycutter/tmp','T_PAGZ60_C_OKPR_20110815000447.hdf');
