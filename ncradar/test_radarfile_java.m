clear
close all
clc


javaaddpath('./dist/netcdfAll-4.3.jar')
% stil need to add whatever import you need from the jar

javaaddpath('./dist/ncradar.jar')
import nl.esciencecenter.ncradar.*

iScan = 1;
theDataDir = fullfile(pwd,'../testdata/harmonized/odim/CZ_brd/20110815/');


rsDBZH = RadarScanMatlab(theDataDir,'T_PAGZ60_C_OKPR_20110815000447.hdf',iScan);

theScanData = rsDBZH.getScanData();

cLimLow = min(theScanData(:));
cLimHigh = max(theScanData(:));


subplotScreen(1,2,1)
imagesc(theScanData)
title([rsDBZH.getDatasetName(),' // ',rsDBZH.getScanType()])
colorbar

disp([min(theScanData(:)),max(theScanData(:))])


subplotScreen(1,2,2)
plotppi(rsDBZH,'cLimLow',cLimLow,'cLimHigh',cLimHigh,...
    'nAzimLines',36)


