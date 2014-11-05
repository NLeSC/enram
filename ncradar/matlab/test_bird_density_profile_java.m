

clear
close all
clc


javaaddpath('../dist/netcdfAll-4.3.jar')
javaaddpath('../dist/ncradar.jar')

import nl.esciencecenter.ncradar.*

% Note: I'm using two native libraries from '/home/wbouten/enram/ncradar/dist', namely 
% 'libvol2bird.so' and 'libvol2birdjni.so', which are accessible from within MATLAB 
% because I've added '/home/wbouten/enram/ncradar/dist' to the file 
% 'libarypath.txt' that resides at '~/opt/MATLAB/R2012a/toolbox/local/');
%
% Furthermore, it seems you have to start matlab in the directory that this m-file is in.
% More weirdness: it seems that the link between my JNIMethodsVol2Bird and
% the actual native library can only be resolved every other time you run
% it (without any changes to the code)
%

defaultParameterValues = ParameterValues();

iScan = 5;
theDataDir = fullfile(pwd,'../../testdata/harmonized/odim/CZ_brd/20110815/');


rsDBZH = RadarScanJava(theDataDir,'T_PAGZ60_C_OKPR_20110815000447.hdf',iScan);
rsVRAD = RadarScanJava(theDataDir,'T_PAHZ60_C_OKPR_20110815000447.hdf',iScan);



bdp = BirdDensityProfileJava(rsDBZH,rsVRAD)

nRangNeighborhood = 3
nAzimNeighborhood = 3
nCountMin = 0
texOffset = 0
texScale = 1
vradMissing = 255;

bdp.calcTexture(nRangNeighborhood, nAzimNeighborhood, nCountMin, texOffset, texScale, vradMissing)


numberOfAzimuthBins = rsDBZH.getNumberOfAzimuthBins();
numberOfRangeBins = rsDBZH.getNumberOfRangeBins();
dataRaw = bdp.getTexture();
dataOffset = 0;
dataScale = 1.0;
rangeOffset = rsDBZH.getRangeOffset();
rangeScale = rsDBZH.getRangeScale();
missingValueValue = rsDBZH.getMissingValueValue();
iAzimFirstRay = rsDBZH.getiAzimFirstRay();

polarDataTex = PolarData(numberOfAzimuthBins, numberOfRangeBins, dataRaw, dataOffset, dataScale,...
    rangeOffset, rangeScale, missingValueValue, iAzimFirstRay);


 
subplotScreen(2,3,1)
plotpolardata(rsDBZH.getPolarData(),'nAzimLines',36);
str1 = [char(rsDBZH.getScanType),' // ',char(rsDBZH.getDatasetName),' // ',char(rsDBZH.getStartDate),' // ',char(rsDBZH.getStartTime)];
title(str1)

subplotScreen(2,3,4)
imagesc((rsDBZH.getPolarData.getRangeScale*(0:1:519)+rsDBZH.getPolarData.getRangeOffset)/1000,0:1:359,rsDBZH.getPolarData.getData2D)
title(str1)
xlabel('range [km]')
ylabel('azimuth [degrees]')

subplotScreen(2,3,2)
plotpolardata(rsVRAD.getPolarData(),'nAzimLines',36);
str2 = [char(rsVRAD.getScanType),' // ',char(rsVRAD.getDatasetName),' // ',char(rsVRAD.getStartDate),' // ',char(rsVRAD.getStartTime)];
title(str2)

subplotScreen(2,3,5)
imagesc((rsVRAD.getPolarData.getRangeScale*(0:1:519)+rsVRAD.getPolarData.getRangeOffset)/1000,0:1:359,rsVRAD.getPolarData.getData2D)
title(str2)
xlabel('range [km]')
ylabel('azimuth [degrees]')

subplotScreen(2,3,3)
plotpolardata(polarDataTex,'nAzimLines',36);
str3 = ['texture // ',char(rsVRAD.getDatasetName),' // ',char(rsVRAD.getStartDate),' // ',char(rsVRAD.getStartTime)]; 
title(str3)

subplotScreen(2,3,6)
imagesc((polarDataTex.getRangeScale*(0:1:519)+polarDataTex.getRangeOffset)/1000,0:1:359,polarDataTex.getData2D)
str3 = ['texture // ',char(rsVRAD.getDatasetName),' // ',char(rsVRAD.getStartDate),' // ',char(rsVRAD.getStartTime)]; 
title(str3)
xlabel('range [km]')
ylabel('azimuth [degrees]')





% % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % 


polarDataCellImage = PolarData(numberOfAzimuthBins, numberOfRangeBins, bdp.getCellImage,...
            dataOffset, dataScale,...
            rangeOffset, rangeScale,...
            missingValueValue, iAzimFirstRay);

        
subplotScreen(2,3,1)
imagesc(polarDataCellImage.getData2D)
colorbar


dbzThresMin = -10.0;
rCellMax = 200;
theSign = 1;

bdp.findCells(dbzThresMin,rCellMax,theSign)

polarDataCellImage = PolarData(numberOfAzimuthBins, numberOfRangeBins, bdp.getCellImage,...
            dataOffset, dataScale,...
            rangeOffset, rangeScale,...
            missingValueValue, iAzimFirstRay);

        
subplotScreen(2,3,1)
imagesc(polarDataCellImage.getData2D)
colorbar




        

