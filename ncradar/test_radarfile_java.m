clear
close all
clc



javaaddpath('./bin')  
% ...and import its contents
import nl.esciencecenter.ncradar.*

javaaddpath('./lib/netcdfAll-4.3.jar')

rs = RadarScan('/home/daisycutter/tmp','T_PAGZ60_C_OKPR_20110815000447.hdf');

theScanData = rs.getScanData();

nBins = rs.getNumberOfBins();
nRays = rs.getNumberOfRays();




% profile on
tic

% Maak een MATLAB figuurtje
figure
imagesc(theScanData)

figure
plotppi(rs)
axis image
axis off

% print naar CSV in een format dat CartoDB kan lezen:
rf.printAsWKTToCSV()

toc
% profile viewer