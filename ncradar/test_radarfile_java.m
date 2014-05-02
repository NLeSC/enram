clear
close all
clc



% javaaddpath('./dist')  
% ...and import its contents
% import nl.esciencecenter.ncradar.*

javaaddpath('./dist/netcdfAll-4.3.jar')
% stil need to add whatever import you need from the jar

javaaddpath('./dist/ncradar.jar')
import nl.esciencecenter.ncradar.*

iScan = 4;
theDataDir = fullfile(pwd,'../testdata/harmonized/odim/CZ_brd/20110815/');

figure
rsDBZH = RadarScan(theDataDir,'T_PAGZ60_C_OKPR_20110815000447.hdf',iScan);
theScanData = rsDBZH.getScanData();

subplot(2,3,1)
imagesc(theScanData,[-128,127])
title([char(rsDBZH.getDatasetName()),' // ',char(rsDBZH.getScanType())])

% subplot(2,3,4)
% plotppi(rsDBZH)
% axis image
% axis off 
% drawnow
% clear rsDBZH
% 

rsVRAD = RadarScan(theDataDir,'T_PAHZ60_C_OKPR_20110815000447.hdf',iScan);
theScanData = rsVRAD.getScanData();

subplot(2,3,2)
imagesc(theScanData,[-128,127])
title([char(rsVRAD.getDatasetName()),' // ',char(rsVRAD.getScanType())])

% subplot(2,3,5)
% plotppi(rsVRAD)
% axis image
% axis off 
% drawnow
% clear rsVRAD

rsWRAD = RadarScan(theDataDir,'T_PAIZ60_C_OKPR_20110815000447.hdf',iScan);
theScanData = rsWRAD.getScanData();

subplot(2,3,3)
imagesc(theScanData,[-128,127])
title([char(rsWRAD.getDatasetName()),' // ',char(rsWRAD.getScanType())])

% subplot(2,3,6)
% plotppi(rsWRAD)
% axis image
% axis off 
% drawnow

clear WRAD