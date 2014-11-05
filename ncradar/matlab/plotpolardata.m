function varargout = plotppi(rs,varargin)


rs.calcVerticesAndFaces()
    

faces = rs.getFaces();
vertices = rs.getVertices();
facesValues = rs.getFacesValues();


colorbarVerticalAlign = 'bottom';
titlePrepend = '';
titleAppend = '';
colorbarTitle = '';
showColorbar = true;
colorbarRelativeHeight = 0.5;
cLimLow = min(facesValues);
cLimHigh = max(facesValues);
axisMarginFactor = 1.1;
showRangeLines = true;
showAzimLines = true;
nAzimLines = 8;
nRangeLines = 5;
showAzimLabels = true;
showRangeLabels = true;
showTitle = true;

authorizedOptions ={'cLimLow','cLimHigh','colorbarVerticalAlign',...
    'titleAppend','titlePrepend','colorbarTitle','showColorbar',...
    'colorbarRelativeHeight','axisMarginFactor','showRangeLines',...
    'showAzimLines','nAzimLines','nRangeLines','showAzimLabels',...
    'showAzimLabels','showRangeLabels','showTitle'};

parsePairs




radius = (max(vertices(:,1))-min(vertices(:,1)))/2;


nColormapColors = size(colormap,1);

frac = (facesValues-cLimLow)/(cLimHigh-cLimLow);

faceColor = 1+floor(frac(:)*(nColormapColors-1));

if strcmp(rs.getClass,'class nl.esciencecenter.ncradar.PolarData')
    % matlab uses 1-based indexing
    faces = faces + 1;
end

p = patch('Faces',faces,'Vertices',vertices,'EdgeColor','none',...
                                            'FaceColor','flat',...
                                      'FaceVertexCData',faceColor);
hold on

axis image
hAxes = gca;

set(hAxes,'XLim',get(hAxes,'XLim')*axisMarginFactor)
set(hAxes,'YLim',get(hAxes,'YLim')*axisMarginFactor)
set(hAxes,'color','none')
axis off

if showTitle
    
    if strcmp(rs.getClass,'class nl.esciencecenter.ncradar.RadarScanMatlab')
        title([rs.getDatasetName(),' // ',rs.getScanType()])
    end
    
end

if showColorbar
    
    hColorbar = colorbar;
    
    % workaround shifted colorbar bug (?)
    posAxes = get(hAxes,'Position');
    posColorbar = get(hColorbar,'Position');

    posColorbarNew = posColorbar;
    posColorbarNew(4) = posAxes(4)*colorbarRelativeHeight;
    switch colorbarVerticalAlign
        case 'top'
            posColorbarNew(2) = posAxes(2)+posAxes(4)-posColorbarNew(4);
        case 'middle'
            posColorbarNew(2) = posAxes(2)+posAxes(4)/2-posColorbarNew(4)/2;
        case 'bottom'
            posColorbarNew(2) = posAxes(2);
    end
    
    set(hColorbar,'Position',posColorbarNew)
    set(hAxes,'Position',posAxes)
    
    setColorbarTitle(hColorbar,colorbarTitle);
    
    yticklabels = (((get(hColorbar,'YTick')-1)/nColormapColors) * (cLimHigh-cLimLow)) + cLimLow;
    set(hColorbar,'YTickLabel',yticklabels)
    
end

if showAzimLines
    
    for iAzimLines = 1:nAzimLines
        
        xv = [(1/nRangeLines),1.0] * radius * sin((iAzimLines-1)/nAzimLines*2*pi);
        yv = [(1/nRangeLines),1.0] * radius * cos((iAzimLines-1)/nAzimLines*2*pi);
        
        plot(xv,yv,'-k')
        
    end
    
end

if showAzimLabels
    
    azimLabelRadius = radius * (1.0 + (axisMarginFactor - 1.0)/2)
    for iAzimLines = 1:nAzimLines
        
        x = azimLabelRadius*sin((iAzimLines-1)/nAzimLines*2*pi);
        y = azimLabelRadius*cos((iAzimLines-1)/nAzimLines*2*pi);
        
        text(x,y,num2str(360*(iAzimLines-1)/nAzimLines),...
            'horizontalalign','center',...
            'verticalalign','middle')

    end
    
end


if showRangeLines
    
    radVec = linspace(0,2*pi,1000);
    for iRangeLine = 1:nRangeLines
        
        
        xv = (iRangeLine/nRangeLines) * radius * sin(radVec);
        yv = (iRangeLine/nRangeLines) * radius * cos(radVec);
        
        plot(xv,yv,'-k')
        
    end
    
end


if showRangeLabels
    
    for iRangeLine = 1:nRangeLines
        
        
        x = (iRangeLine/nRangeLines) * radius * sin(pi);
        y = (iRangeLine/nRangeLines) * radius * cos(pi);
        
        text(x,y,num2str(radius*(iRangeLine/nRangeLines)),...
            'horizontalalign','center',...
            'verticalalign','middle','backgroundColor','k',...
            'Color','w')
        
    end
    
end


if nargout == 1
    varargout{1} = p;
else
    varargout = {};
end