function tc = twoscomphex( val, n ,output)
%TWOSCOMP two's complement
%      twoscomp( a, b ) computes the two's complement of a in b bits as
%
%      see also DEC2BIN, MOD.
if(nargin<3)
    output='bin';
end

if(nargin<2)
    n=8;
end

bin=dec2bin( mod( (val), 2^n ),n);

if(strcmp(output,'bin'))
    tc=bin;
else
    dec=bin2dec(bin);
    if(strcmp(output,'dec'))
        tc=dec;
    else
        tc=dec2hex(dec);
    end
end
    
end
