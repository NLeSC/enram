function varargout = plotpolardata(polarData,varargin)

polarDataClass = class(polarData);
if ~strncmp(polarDataClass,'nl.esciencecenter.ncradar.PolarData',35)
    disp('This function only takes ''nl.esciencecenter.ncradar.PolarData'' input arguments. Aborting.')
    return
end


polarData.calcVerticesAndFaces()
    

faces = polarData.getFaces();
vertices = polarData.getVertices();
facesValues = polarData.getFacesValues();


colorbarVerticalAlign = 'bottom';
colorbarTitle = '';
showColorbar = true;
colorbarRelativeHeight = 0.5;
cLimLow = min(facesValues);
cLimHigh = max(facesValues);
axisMarginFactor = 1.1;
showRangeLines = true;
showAzimLines = true;
nAzimLines = 8;
%nRangeLines = 5;
rangeResolution = 50000;
showAzimLabels = true;
showRangeLabels = true;
azimLineStyle = {'color',0.5*[1,1,1]};
rangeLineStyle = {'color',0.5*[1,1,1]};
azimLabelAngle = (3/4) * 2 * pi;

authorizedOptions ={'cLimLow','cLimHigh','colorbarVerticalAlign',...
    'colorbarTitle','showColorbar',...
    'colorbarRelativeHeight','axisMarginFactor','showRangeLines',...
    'showAzimLines','nAzimLines','showAzimLabels',...
    'showAzimLabels','showRangeLabels','rangeResolution',...
    'azimLineStyle','rangeLineStyle'};

parsePairs




radius = (max(vertices(:,1))-min(vertices(:,1)))/2;

rangeLinesVec = 0:rangeResolution:radius;

nColormapColors = size(colormap,1);

frac = (facesValues-cLimLow)/(cLimHigh-cLimLow);

faceColor = 1+floor(frac(:)*(nColormapColors-1));

if strcmp(polarData.getClass,'class nl.esciencecenter.ncradar.PolarData')
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
        
        xv = [(rangeResolution/radius),1.0] * radius * sin((iAzimLines-1)/nAzimLines*2*pi);
        yv = [(rangeResolution/radius),1.0] * radius * cos((iAzimLines-1)/nAzimLines*2*pi);
        
        
        if iscell(azimLineStyle)
            plot(xv,yv,azimLineStyle{:})
        else
            plot(xv,yv,azimLineStyle)
        end
    end
    
end

if showAzimLabels
    
    azimLabelRadius = radius * (1.0 + (axisMarginFactor - 1.0)/2);
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
    
    for iRangeLine = 1:numel(rangeLinesVec);
        
        xv = rangeLinesVec(iRangeLine) * sin(radVec);
        yv = rangeLinesVec(iRangeLine) * cos(radVec);
        
        if iscell(rangeLineStyle)
            plot(xv,yv,rangeLineStyle{:})
        else
            plot(xv,yv,rangeLineStyle)
        end
        
    end
    
end


if showRangeLabels
    
    for iRangeLine = 1:numel(rangeLinesVec)
        
        x = rangeLinesVec(iRangeLine) * sin(azimLabelAngle);
        y = rangeLinesVec(iRangeLine) * cos(azimLabelAngle);
        
        text(x,y,num2str(rangeLinesVec(iRangeLine)),...
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