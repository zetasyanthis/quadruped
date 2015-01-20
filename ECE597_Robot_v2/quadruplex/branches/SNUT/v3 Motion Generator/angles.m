function [t1_1,t2_1,t1_2,t2_2] = angles(a,b,x,y,t3,degrad,debug)

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

if((a+b)^2<(x^2+y^2))
    error(['Position (x,y)=(',num2str(x),',',num2str(y),') invalid for mechanical parameters (a,b)=(',num2str(a),',',num2str(b),').'])
    return;
end

t1_1=atan2(-1/2*(b^2-x^2-a^2-y^2+y/(4*y^2+4*x^2)*(4*y*a^2-4*b^2*y+4*y^3+4*x^2*y+4*(-y^4*x^2-2*x^4*y^2+2*x^4*b^2-x^2*b^4-x^2*a^4+2*x^2*b^2*a^2+2*x^4*a^2-x^6+2*y^2*a^2*x^2+2*b^2*y^2*x^2)^(1/2)))/x/a,1/2/(4*y^2+4*x^2)*(4*y*a^2-4*b^2*y+4*y^3+4*x^2*y+4*(-y^4*x^2-2*x^4*y^2+2*x^4*b^2-x^2*b^4-x^2*a^4+2*x^2*b^2*a^2+2*x^4*a^2-x^6+2*y^2*a^2*x^2+2*b^2*y^2*x^2)^(1/2))/a);
t1_2=atan2(-1/2*(b^2-x^2-a^2-y^2+y/(4*y^2+4*x^2)*(4*y*a^2-4*b^2*y+4*y^3+4*x^2*y-4*(-y^4*x^2-2*x^4*y^2+2*x^4*b^2-x^2*b^4-x^2*a^4+2*x^2*b^2*a^2+2*x^4*a^2-x^6+2*y^2*a^2*x^2+2*b^2*y^2*x^2)^(1/2)))/x/a,1/2/(4*y^2+4*x^2)*(4*y*a^2-4*b^2*y+4*y^3+4*x^2*y-4*(-y^4*x^2-2*x^4*y^2+2*x^4*b^2-x^2*b^4-x^2*a^4+2*x^2*b^2*a^2+2*x^4*a^2-x^6+2*y^2*a^2*x^2+2*b^2*y^2*x^2)^(1/2))/a);

t2_1=(-t1_1-t3-asin((a*sin(t1_1)-x)/b))*degrad;
t2_2=(-t1_2-t3-asin((a*sin(t1_2)-x)/b))*degrad;

t1_1=t1_1*degrad;
t1_2=t1_2*degrad;

if nargout<4
    if(abs(t1_1)>limit || abs(t2_1)>limit)
        if(debug)
            disp(['First solution (',num2str(t1_1),',',num2str(t2_1),') out of bounds.']);
        end
        firstInvalid=1;
    end

    if(abs(t1_2)>limit || abs(t2_2)>limit)
        if(debug)
            disp(['Second solution (',num2str(t1_2),',',num2str(t2_2),') out of bounds.']);
        end
        secondInvalid=1;
    end
    
    if(firstInvalid && secondInvalid)
        error('No valid solutions found.');
    elseif(firstInvalid)
        t1_1=t1_2;
        t2_1=t2_2;
    %else return t1_1 and t2_1 as they are
    end
end

%{


n=(-(b.^2)+x.^2+a.^2+y.^2)/(2*a);

tone=-pi:.01:pi;

plot(tone,x*sin(tone)+y*cos(tone)-n);
grid on;

mirrorAngle=-atan(x/y);


t11=fzero(@(t1)x*sin(t1)+y*cos(t1)-n,[mirrorAngle pi]);

t21=fzero(@(t2)b*sin(t11+t2+t3)+a*sin(t11)-x,0)*degrad;
t11=t11*degrad;

if nargout>2
    t12=fzero(@(t1)x*sin(t1)+y*cos(t1)-n,[-pi mirrorAngle]);

    t22=fzero(@(t2)b*sin(t12+t2+t3)+a*sin(t12)+x,0)*degrad;
    t12=t12*degrad;
end
%}

end