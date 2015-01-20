function [t1,t2,t3]=roboAngles(x,y,z,leg,debug)
%ROBOANGLES(x,y,leg) calculates angles of given leg motors for a given
%position (x,y).

if(debug)
    disp(['Leg #',num2str(leg),' x:',num2str(x),' y:',num2str(y),' z:',num2str(z)]);
end

%Swap x directions for appropriate legs
if((leg==0) || (leg==3))
    xSign=-1;
elseif((leg==6) || (leg==9))
    xSign=1;
end

%If x=0, division by zero occurs, so avoid it.
if(x==0)
    x=.000000001;
end

% 8.64=(3.5^2+7.9^2)^.5

%Calculate appropriate angles
[t3_rel,yp]=shoulderAngles(4.6,2.8,y,z,'deg',debug);  %approximate a,b
%[t1_rel,t2_rel]=angles(6.8,10,xSign*x,yp,1.1198,'deg',debug);
[t1_rel,t2_rel]=angles(6.7,8.64,xSign*x,yp,1.1537,'deg',debug);
%Swap angle directions for appropriate legs
if((leg==0) || (leg==6))
    legSign=1;
elseif((leg==3) || (leg==9))
    legSign=-1;
end

%Output in absolute degrees
t1=(legSign*t1_rel+150);
t2=(legSign*t2_rel+150);
t3=(legSign*t3_rel+150);

if(debug)
    disp(['(',num2str(t1),',',num2str(t2),',',num2str(t3),')'])
end

end