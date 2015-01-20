function [t3_1,yp_1,t3_2,yp_2]=shoulderAngles(a,b,y,z,degrad,debug)

limit=(pi/180)*90;

if(nargin<6 || strcmp(degrad,'deg'))
    degrad=180/pi;
    limit=90;
end

if(nargin<7)
    debug=0;
end

firstInvalid=0;
secondInvalid=0;

%{
if((a+b)^2<(x^2+y^2))
    error(['Position (x,y)=(',num2str(x),',',num2str(y),') invalid for mechanical parameters (a,b)=(',num2str(a),',',num2str(b),').'])
    return;
end
%}

t3_1=atan2(-1.*(y*a-1.*(z^4-1.*z^2*a^2+y^2*z^2)^(1/2))/(z^2+y^2),(-1.*y*(y*a-1.*(z^4-1.*z^2*a^2+y^2*z^2)^(1/2))/(z^2+y^2)+a)/z);
t3_2=atan2(-1.*(y*a+(z^4-1.*z^2*a^2+y^2*z^2)^(1/2))/(z^2+y^2),(-1.*y*(y*a+(z^4-1.*z^2*a^2+y^2*z^2)^(1/2))/(z^2+y^2)+a)/z);

yp_1=(y+a*sin(t3_1))/cos(t3_1)-b;
yp_2=(y+a*sin(t3_2))/cos(t3_2)-b;

t3_1=t3_1*degrad;
t3_2=t3_2*degrad;

if nargout<4
    if(abs(t3_1)>limit || yp_1<0)
        if(debug)
            disp(['First solution (',num2str(t3_1),',',num2str(yp_1),') out of bounds.']);
        end
        firstInvalid=1;
    end

    if(abs(t3_1)>limit || yp_2<0)
        if(debug)
            disp(['Second solution (',num2str(t3_2),',',num2str(yp_2),') out of bounds.']);
        end
        secondInvalid=1;
    end
    
    if(firstInvalid && secondInvalid)
        error('No valid solutions found.');
    elseif(firstInvalid)
        t3_1=t3_2;
        yp_1=yp_2;
    %else return t3_1 and yp_1 as they are
    end
end