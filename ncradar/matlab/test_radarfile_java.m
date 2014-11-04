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


rsDBZH = RadarScanMatlab(theDataDir,'T_PAGZ60_C_OKPR_20110815000447.hdf',iScan);

theScanData = rsDBZH.getScanData();

cLimLow = min(theScanData(:));
cLimHigh = max(theScanData(:));


subplotScreen(2,4,1)
imagesc(theScanData)
title([rsDBZH.getDatasetName(),' // ',rsDBZH.getScanType()])
colorbar

disp([min(theScanData(:)),max(theScanData(:))])


subplotScreen(2,4,2)
plotppi(rsDBZH,'cLimLow',cLimLow,...
              'cLimHigh',cLimHigh,...
            'nAzimLines',36)


        
        
rsVRAD = RadarScanMatlab(theDataDir,'T_PAHZ60_C_OKPR_20110815000447.hdf',iScan);

theScanData = rsVRAD.getScanData();

cLimLow = min(theScanData(:));
cLimHigh = max(theScanData(:));


subplotScreen(2,4,3)
imagesc(theScanData)
title([rsVRAD.getDatasetName(),' // ',rsVRAD.getScanType()])
colorbar

disp([min(theScanData(:)),max(theScanData(:))])


subplotScreen(2,4,4)
plotppi(rsVRAD,'cLimLow',cLimLow,...
               'cLimHigh',cLimHigh,...
               'nAzimLines',36)

           
% % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % 
% calcTexture:
           

rsDBZHJava = RadarScanJava(theDataDir,'T_PAGZ60_C_OKPR_20110815000447.hdf',iScan);
rsVRADJava = RadarScanJava(theDataDir,'T_PAHZ60_C_OKPR_20110815000447.hdf',iScan);
bdp = BirdDensityProfileJava(rsDBZHJava,rsVRADJava);
bdp.calcTexture(3,3,4,0,1,255);

numberOfAzimuthBins = rsDBZH.getNumberOfAzimuthBins;
numberOfRangeBins = rsDBZH.getNumberOfRangeBins;
dataRaw = double(bdp.getTexture');
dataOffset = 0;
dataScale = 1;
rangeOffset = rsDBZH.getRangeOffset;
rangeScale = rsDBZH.getRangeScale;
missingValueValue = rsDBZH.getMissingValueValue; 
iAzimFirstRay = rsDBZH.getiAzimFirstRay;

polarPlotTexture = PolarData(numberOfAzimuthBins, numberOfRangeBins, dataRaw,...
            dataOffset, dataScale,...
            rangeOffset, rangeScale,...
            missingValueValue, iAzimFirstRay);

subplotScreen(2,4,5)
plotppi(polarPlotTexture,'nAzimLines',36)
title('texture')

% % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % 
% findCells:

polarPlotCellImage = PolarData(numberOfAzimuthBins, numberOfRangeBins, bdp.getCellImage,...
            dataOffset, dataScale,...
            rangeOffset, rangeScale,...
            missingValueValue, iAzimFirstRay);

subplotScreen(2,4,6)
plotppi(polarPlotCellImage,'nAzimLines',36)
title('cellImage')


dbzImage = bdp.getReflectivityRaw;
cellImage = bdp.getCellImage;
dbzMissing = 255;
dbznAzim = rsDBZH.getNumberOfAzimuthBins;
dbznRang = rsDBZH.getNumberOfRangeBins;
dbzOffset = single(rsDBZH.getDataOffset);
dbzScale = single(rsDBZH.getDataScale);
dbzRangeScale = single(rsDBZH.getRangeScale);
dbzThresMin = single(0);

rhoMissing = dbzMissing;
rhonAzim = dbznAzim;
rhonRang = dbznRang;
rhoOffset = single(dbzOffset);
rhoScale = single(dbzScale);
rhoThresMin = single(dbzThresMin);
    
zdrMissing = dbzMissing;
zdrnAzim = dbznAzim; 
zdrnRang = dbznRang; 
zdrOffset = single(dbzOffset); 
zdrScale = single(dbzScale); 
zdrThresMin = single(dbzThresMin);
rCellMax = single(defaultParameterValues.getRANGMAX+5);
sign = -1;

% 
% bdp.findCells(dbzImage, [], [], cellImage,...
%                 dbzMissing, dbznAzim, dbznRang, dbzOffset, dbzScale, dbzRangeScale, dbzThresMin,...
%                 rhoMissing, rhonAzim, rhonRang, rhoOffset, rhoScale, rhoThresMin,...
%                 zdrMissing, zdrnAzim, zdrnRang, zdrOffset, zdrScale, zdrThresMin,...
%                 rCellMax, sign);

% bdp.analyzeCells();
% bdp.fringeCells();
% bdp.classify();




subplotScreen(1,2,1)


plotppi(rsVRAD,'cLimLow',cLimLow,...
               'cLimHigh',cLimHigh,...
               'nAzimLines',36,...
               'showRangeLabels',false,...
               'showColorbar',false,...
               'showAzimLabels',false,...
               'showTitle',false)


set(gcf,'PaperPositionMode','auto','InvertHardCopy','off')

print('radar-precipitation.png','-dpng','-r600')
   
        