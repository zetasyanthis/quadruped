function [xp,zp]=turn(m,t)

t=t*pi/180;

xp=zeros(1,4);
zp=zeros(1,4);

for k=1:4
    x=m(k,1);
    z=m(k,2)+4.5;
    
    if((k==2)||(k==3))
        z=(-z);
    end
    
    if((k==1)||(k==2)) %back legs
        x=x-10.5;
    else
        x=x+10.5;
    end

    x
    z
    
    [xpTemp,zpTemp]=turnOneLeg(x,z,t);
    
    xpTemp
    zpTemp
    
    zpTemp=abs(zpTemp)-4.5;
    
    

    
    %added k=4 and k=2 to wrong parts here, but it seems to work?
    %{
if((k==1)||(k==4))%||(k==2)) %back legs
        xpTemp=xpTemp-10.5;
    elseif((k==3)||(k==2))
        xpTemp=xpTemp+10.5;
    end
    
    if((k==1)||(k==3))
        xpTemp=-xpTemp;
        %zpTemp=zpTemp+4.5+defaultZ;
%    elseif(k==2)
 %       xpTemp=xpTemp+10.5
        %zpTemp=zpTemp-4.5+defaultZ;
    end
%}
    
    if((k==3) || (k==4))
        xpTemp=xpTemp-10.5;
    elseif((k==1) || (k==2))
        xpTemp=xpTemp+10.5;
    end
    
    
    
    xp(k)=xpTemp;
    zp(k)=zpTemp;
end
    
end