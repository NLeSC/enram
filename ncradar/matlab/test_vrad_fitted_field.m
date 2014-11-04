function test_vrad_fitted_field()



u0 = 0;
v0 = 1;
w0 = 0;


azimVec = 0:45:360;
elevVec = [30];

nAzim = numel(azimVec);
nElev = numel(elevVec);
vrad = repmat(NaN,[nAzim,nElev]);

for iAzim = 1:nAzim
    for iElev = 1:nElev
        
        azimAngle = azimVec(iAzim);
        elevAngle = elevVec(iElev);
        vrad(iAzim,iElev) = calcvrad(azimAngle,elevAngle, u0,v0,w0);

        fprintf('%6.2f,%6.2f,%6.2f\n',azimAngle,elevAngle,vrad(iAzim,iElev))
        
    end
end

fprintf('\n');

% printing for Java unit test data
for iAzim = 1:nAzim
    for iElev = 1:nElev
        
        azimAngle = azimVec(iAzim);
        elevAngle = elevVec(iElev);
        fprintf('%6.2ff,%6.2ff,\n',azimAngle,elevAngle)
    end
end

fprintf('\n');

% printing for Java unit test data
for iAzim = 1:nAzim
    for iElev = 1:nElev
        fprintf('%6.2ff,\n',vrad(iAzim,iElev))
    end
end


figure
imagesc(elevVec,azimVec,vrad)
set(gca,'xtick',elevVec)
colorbar
xlabel('elevation angle [degrees]')
ylabel('azimuth angle [degrees]')
title(sprintf('vrad simulated [u_0,v_0,w_0] = [%.2f,%.2f,%.2f]',u0,v0,w0))





function vrad = calcvrad(azimAngleDeg,elevAngleDeg, u0,v0,w0)

azimAngleRad = (azimAngleDeg/360) * 2 * pi;
elevAngleRad = (elevAngleDeg/360) * 2 * pi;


vrad = sin(azimAngleRad) * cos(elevAngleRad) * u0 +...
       cos(azimAngleRad) * cos(elevAngleRad) * v0 +...
       sin(elevAngleRad) * w0;
       
    