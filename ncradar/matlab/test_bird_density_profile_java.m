

clear
close all
clc


theImports = import();
nImports = numel(theImports);
libraryHasBeenImported = false;
if nImports > 0
    if iscellstr(theImports)
        for iImport = 1:nImports
            if strcmp(theImports{iImport},'nl.esciencecenter.ncradar.*')
                
                libraryHasBeenImported = true;

            end
        end
    end
end
if ~libraryHasBeenImported

    % I'm using two native libraries from '/home/wbouten/enram/ncradar/dist', 
    % namely 'libvol2bird.so' and 'libvol2birdjni.so', which are accessible 
    % from within MATLAB  because I've added '/home/wbouten/enram/ncradar/dist' 
    % to the file 'libarypath.txt' that resides at '/home/wbouten/enram/matlab'). 
    % 
    % I must execute the javaaddpaths and import (below) only once, otherwise 
    % I get errors.
    %
    % Further note that '/home/wbouten/enram/matlab' must be the directory 
    % that the matlab program is started from.
    

    javaaddpath('../dist/netcdfAll-4.3.jar')
    javaaddpath('../dist/ncradar.jar')
    import nl.esciencecenter.ncradar.*
    
end
clear iImport
clear nImports
clear theImports
clear libraryHasBeenImported



defaultParameterValues = ParameterValues();

iScan = 0;
theDataDir = fullfile(pwd,'../../testdata/harmonized/odim/CZ_brd/20110815/');


rsDBZH = RadarScanJava(theDataDir,'T_PAGZ60_C_OKPR_20110815000447.hdf',iScan);
rsVRAD = RadarScanJava(theDataDir,'T_PAHZ60_C_OKPR_20110815000447.hdf',iScan);


bdp = BirdDensityProfileJava(rsDBZH,rsVRAD)

bdp.calcTexture();


numberOfAzimuthBins = rsDBZH.getNumberOfAzimuthBins();
numberOfRangeBins = rsDBZH.getNumberOfRangeBins();
dataRaw = bdp.getTexture();
dataOffset = rsDBZH.getDataOffset();
dataScale = rsDBZH.getDataScale();
rangeOffset = rsDBZH.getRangeOffset();
rangeScale = rsDBZH.getRangeScale();
missingValueValue = rsDBZH.getMissingValueValue();
iAzimFirstRay = rsDBZH.getiAzimFirstRay();

polarDataTex = PolarData(numberOfAzimuthBins, numberOfRangeBins, dataRaw, dataOffset, dataScale,...
    rangeOffset, rangeScale, missingValueValue, iAzimFirstRay);

% % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % 
 
subplotScreen(2,3,1)
plotpolardata(rsDBZH.getPolarData());
str1 = [char(rsDBZH.getScanType),' // ',char(rsDBZH.getDatasetName),' // ',char(rsDBZH.getStartDate),' // ',char(rsDBZH.getStartTime)];
title(str1)
set(gcf,'name',str1)

subplotScreen(2,3,4)
imagesc((rsDBZH.getPolarData.getRangeScale*(0:1:519)+rsDBZH.getPolarData.getRangeOffset)/1000,0:1:359,rsDBZH.getPolarData.getData2D)
title(str1)
xlabel('range [km]')
ylabel('azimuth [degrees]')
set(gcf,'name',str1)

subplotScreen(2,3,2)
plotpolardata(rsVRAD.getPolarData());
str2 = [char(rsVRAD.getScanType),' // ',char(rsVRAD.getDatasetName),' // ',char(rsVRAD.getStartDate),' // ',char(rsVRAD.getStartTime)];
title(str2)
set(gcf,'name',str2)

subplotScreen(2,3,5)
imagesc((rsVRAD.getPolarData.getRangeScale*(0:1:519)+rsVRAD.getPolarData.getRangeOffset)/1000,0:1:359,rsVRAD.getPolarData.getData2D)
title(str2)
xlabel('range [km]')
ylabel('azimuth [degrees]')
set(gcf,'name',str2)

subplotScreen(2,3,3)
plotpolardata(polarDataTex);
str3 = ['texture // ',char(rsVRAD.getDatasetName),' // ',char(rsVRAD.getStartDate),' // ',char(rsVRAD.getStartTime)]; 
title(str3)
set(gcf,'name',str3)

subplotScreen(2,3,6)
imagesc((polarDataTex.getRangeScale*(0:1:519)+polarDataTex.getRangeOffset)/1000,0:1:359,polarDataTex.getData2D)
str3 = ['texture // ',char(rsVRAD.getDatasetName),' // ',char(rsVRAD.getStartDate),' // ',char(rsVRAD.getStartTime)]; 
title(str3)
xlabel('range [km]')
ylabel('azimuth [degrees]')
set(gcf,'name',str3)



% % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % 


nCells = bdp.findCells();

polarDataCellImage = PolarData(numberOfAzimuthBins, numberOfRangeBins, bdp.getCellImage,...
            dataOffset, dataScale,...
            rangeOffset, rangeScale,...
            missingValueValue, iAzimFirstRay);

subplotScreen(1,2,1)
plotpolardata(polarDataCellImage,'rangeResolution',5000)
str4 = ['cellImage after findCells() // ',char(rsDBZH.getDatasetName),' // ',char(rsDBZH.getStartDate),' // ',char(rsDBZH.getStartTime)]; 
title(str4)
set(gcf,'name',str4)
set(gca, 'xlim',[-1,1]*3e4,'ylim',[-1,1]*3e4)
colormap(rand(nCells+1,3))

% % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % % 



cmFlag = 0;
verbose = 1;
nCellsValid = bdp.analyzeCells(nCells, cmFlag, verbose);


polarDataCellImage = PolarData(numberOfAzimuthBins, numberOfRangeBins, bdp.getCellImage,...
            0, 1,...
            rangeOffset, rangeScale,...
            missingValueValue, iAzimFirstRay);


subplotScreen(1,2,2)
plotpolardata(polarDataCellImage,'rangeResolution',5000)
str4 = ['cellImage after analyzeCells() // ',char(rsDBZH.getDatasetName),' // ',char(rsDBZH.getStartDate),' // ',char(rsDBZH.getStartTime)]; 
title(str4)
set(gcf,'name',str4)
set(gca, 'xlim',[-1,1]*3e4,'ylim',[-1,1]*3e4)








        
        