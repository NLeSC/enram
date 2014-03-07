function plotppi(rf)



polygons = rf.getPolygons();
theScanData = rf.getScanData();

cLimLow = single(min(theScanData(:)));
cLimHigh = single(max(theScanData(:)));
nColors = size(colormap,1);

xColors = linspace(cLimLow,cLimHigh,nColors)';


theColors = interp1(xColors,colormap,single(theScanData(:)));

nRays = rf.getNumberOfRays;
nBins = rf.getNumberOfBins;

iColor = 0;

lonOutline = repmat(NaN,[nRays*2+1,1]);
latOutline = repmat(NaN,[nRays*2+1,1]);

for iBin = 1:nBins
    for iRay = 1:nRays
    
        lon(1:5,1) = polygons(iBin,iRay,1:5,1);
        lat(1:5,1) = polygons(iBin,iRay,1:5,2);
        iColor = iColor + 1;
        
        if abs(theScanData(iRay,iBin))>10
            patch(lon,lat,'k','facecolor',theColors(iColor,1:3),'edgecolor',theColors(iColor,1:3));
        end
        
        if iBin==nBins
            lonOutline(iRay*2+[-1,0],1) = lon(1:2,1);
            latOutline(iRay*2+[-1,0],1) = lat(1:2,1);

        end
        
        
    end
end

hold on
plot(lonOutline,latOutline,'-k');
