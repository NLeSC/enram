clear
clear java
close all
clc


javaaddpath('../dist/netcdfAll-4.3.jar')

javaaddpath('../dist/ncradar.jar')
import nl.esciencecenter.ncradar.*

% Note: I'm using two native libraries from '/home/wbouten/enram/ncradar/dist', namely 
% 'libvol2bird.so' and 'libvol2birdjni.so', which are accessible from within MATLAB 
% because I've added '/home/wbouten/enram/ncradar/dist' to the file 
% 'libarypath.txt' that resides at '~/opt/MATLAB/R2012a/toolbox/local/');

defaultParameterValues = ParameterValues();

iScan = 5;
theDataDir = fullfile(pwd,'../../testdata/harmonized/odim/CZ_brd/20110815/');


rsDBZH = RadarScanJava(theDataDir,'T_PAGZ60_C_OKPR_20110815000447.hdf',iScan);

theScanData = rsDBZH.getScanData2D();

cLimLow = min(theScanData(:));
cLimHigh = max(theScanData(:));


subplotScreen(2,4,1)
imagesc(theScanData)
title([char(rsDBZH.getDatasetName()),' // ',char(rsDBZH.getScanType())])
colorbar

disp([min(theScanData(:)),max(theScanData(:))])


subplotScreen(2,4,2)
plotpolardata(rsDBZH.getPolarData(),'cLimLow',cLimLow,...
                                   'cLimHigh',cLimHigh,...
                                 'nAzimLines',36)


        
        