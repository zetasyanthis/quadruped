function [xp,zp]=turnOneLeg(x,z,t)

l=(x.^2+z.^2)^(.5);

p=asin(x/l);
p2=acos(z/l);

if(p>0)
    p=p2;
else    
    if(p2>pi/2)
        p=-p2;
    end %else p=p;
end

xp=l*sin(t+p);
zp=l*cos(t+p);




end