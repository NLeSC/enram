%----------------------------------------------
%  reslist.m = List contents of a res file.
%  R. Sterner, 2004 Sep 29
%    hdr = reslist(file,[mach])
%      file = res file name.  in
%      mach = endian: 'l' little (def), 'b' big.
%      hdr = res file header. out
%----------------------------------------------

	function h = reslist(file, mach0)

    %----  Deal with endian  --------
    if (nargin == 2)
        mach1 = mach0;
    else
        mach1 = 'l';
    end
    
    %--- Open res file and read 3 long ints  ----
	fid = fopen(file,'r',mach1);
	if (fid == -1)
	  [' Error in resget: Could not open file ',file]
	  return
	end
	a = fread(fid,3,'int32');
    if (max(abs(a(2:end))) > 10000)
        disp(' Could not read res file.')
        disp(' Try using a different endian: ''l'' or ''b''')
        disp(' For example: reslist(file,''b'')')
        disp([' Current endian is ',mach1])
        return
    end
    
    %---  First value is pointer to header  -----
	status = fseek(fid, a(1), 'bof');

    %---  Read header  -----
	b = fread(fid,[a(2),a(3)],'uchar');
	status = fclose(fid);
	h = char(b');

%______________________________________________
