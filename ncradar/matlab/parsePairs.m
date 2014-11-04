

for k = 1:2:length(varargin(:))
    if ~strcmp(varargin{k}, authorizedOptions)
        s=dbstack;
        error(['Unauthorized parameter name ' 39 varargin{k} 39 ' in ' 10,...
            'parameter/value passed to ' 39 s(end-1).name 39 '.']);
    end
    eval([varargin{k},'=varargin{',num2str(k+1),'};'])
end

if ~isempty(varargin)
    clear k
end
