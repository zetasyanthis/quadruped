function [stepArray,prevPos] = stepGen(argArray, prevPos,debug)

lng=length(argArray);

footCommand=cell(7,1);
legCommand=cell(7,1);
shoulderCommand=cell(7,1);
stepArray=cell(1,1);

aryLng=0;

for k=1:lng
    currentLine=argArray{k};

    x=currentLine{1};
    y=currentLine{2};
    z=currentLine{3};
    legNum=currentLine{4};
    delay=currentLine{5};

    switch legNum
        case 1
            leg=0;
        case 2
            leg=3;
        case 3
            leg=6;
        case 4
            leg=9;
    end

    [legAngle,footAngle,shoulderAngle]=roboAngles(x,y,z,leg,debug);
    
    legIndex=leg+1; %can't start indexing at zero, must run 1-12
    
    %If motor angles are the same as last time, skip this command
    if(prevPos(legIndex)~=shoulderAngle)
        aryLng=aryLng+1;
        shoulderCommand{1}=delay;            %delay is same for all motors on a leg
        shoulderCommand{2}=leg;              %shoulder motor
        shoulderCommand{3}=1;                %move absolute
        shoulderCommand{4}=num2str(shoulderAngle);%leg angle
        shoulderCommand{5}=0;                %not polling, so no need for tRep
        shoulderCommand{6}=0;                %not polling, so no need for nRep
        shoulderCommand{7}=2;                %degrees
        stepArray{aryLng}=shoulderCommand;
        prevPos(legIndex)=shoulderAngle;
    end
    
    %If motor angles are the same as last time, skip this command
    if(prevPos(legIndex+1)~=legAngle)
        aryLng=aryLng+1;
        legCommand{1}=delay;            %delay is same for all motors on a leg
        legCommand{2}=leg+1;            %leg motor = shoulder motor + 1
        legCommand{3}=1;                %move absolute
        legCommand{4}=num2str(legAngle);%leg angle
        legCommand{5}=0;                %not polling, so no need for tRep
        legCommand{6}=0;                %not polling, so no need for nRep
        legCommand{7}=2;                %degrees
        stepArray{aryLng}=legCommand;
        prevPos(legIndex+1)=legAngle;
    end
        
    %If motor angles are the same as last time, skip this command
    if(prevPos(legIndex+2)~=footAngle)
        aryLng=aryLng+1;
        footCommand{1}=delay;             %delay is same for all motors on a leg
        footCommand{2}=leg+2;             %foot motor = shoulder motor + 2
        footCommand{3}=1;                 %move absolute
        footCommand{4}=num2str(footAngle);%leg angle
        footCommand{5}=0;                 %not polling, so no need for tRep
        footCommand{6}=0;                 %not polling, so no need for nRep
        footCommand{7}=2;                 %degrees
        stepArray{aryLng}=footCommand;
        prevPos(legIndex+2)=footAngle;
    end

end

end