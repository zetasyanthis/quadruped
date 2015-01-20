function codeGen(argArray,swap,mm,debug)
%CODEGEN Generates code from GUI data
%   Given data from codeGenGUI, generates a .c file containing movement
%   arrays

if(nargin<3)
    mm=0;
    debug=0;
end

%Static variables for motor commands:
ping='0x01';
readData='0x02';
writeData='0x03';  %instant
writeReg='0x04'; %requires sending Action cmd
action='0x05';
reset='0x06';
syncWrite='0x83';

%Static variables for registers:
goalPos='0x1E';
movingSpeed='0x20';
presentPos='0x24';

%Static variables for command values:
moveAbsolute=1;
moveForward=2;
moveBackward=3;
setVelocity=4;
pollAbsolute=5;
pollForward=6;
pollBackward=7;

%Static variables for degrees/points
points=1;
degrees=2;

%Define "forward" for all the motors:
forwardArray=[1,-1,-1,-1,1,1,1,1,1,-1,-1,-1];


arrayProps=argArray{1};

arrayName=arrayProps{1};
%numCommands=arrayProps{2}
numCommands=length(argArray)-1;
fileName=arrayProps{3};
append=arrayProps{4};

%fileName=[img(1:end-4),'.h'];
if(append==1)
    fid=fopen(fileName,'at');
    action='appended.';
else
    fid=fopen(fileName,'wt');
    action='overwritten.';
end

% char arrayName[9][length]={{
if(~mm)
    fprintf(fid,['static char ',arrayName,'[',num2str(numCommands),'][4]={\n']);
end

%dur,mID,cID,addr,btL,btH[,tRep,nRep]}
for k=2:length(argArray)
    currentLine=argArray{k};
    
    if(length(currentLine)==1)
        dur=currentLine{1};
        fprintf(fid,['wait_ms(',num2str(dur),'0);\n\n']);
        continue;
    else
        dur=currentLine{1};
    end
    
    mID=currentLine{2};
    shoulder=0;
    
    %Determine whether motor is a shoulder
    if(mID==0 || mID==3 || mID==6 || mID==9)
        shoulder=1;
    end
    
    %Swap left and right motor IDs if necessary
    if(strcmp(swap,'LR'))
        if((0<=mID<=2) || (6<=mID<=8))
            mID=mID+3;
        else
            mID=mID-3;
        end
    end
    
    %Swap front and back motor IDs if necessary
    if(strcmp(swap,'FB'))
        if(0<=mID<=2)
            mID=mID+9;
        elseif(3<=mID<=5)
            mID=mID+3;
        elseif(6<=mID<=8)
            mID=mID-3;
        else
            mID=mID-9;
        end
    end
            
    %Get command
    cmd=currentLine{3};
    
    %Reset command variables
    poll=0;
    absolute=0;
    forward=0;

    %Check the command:
    if(cmd==moveAbsolute || cmd==pollAbsolute)
        absolute=1;
    else
        if(cmd==moveForward || cmd==pollForward)
            forward=1;
        end
        
    end
    
    %If we're swapping forward and backward, toggle forward except
    %shoulders
    if(strcmp(swap,'FB') && ~absolute)% && ~shoulder)
        if(forward)
            forward=0;
        else
            forward=1;
        end
    end
    
    %Account for forward or backward: 
    if(forward)
        %values can range from 0 to 512
        %multiply motor positions to be "forward" accordingly
        angle=str2double(currentLine{4})*forwardArray(mID)+512;     
    elseif(~absolute) %backward
        %values can range from 0 to 512
        %multiply motor positions to be "backward" accordingly
        angle=str2double(currentLine{4})*(-1)*(forwardArray(mID)+512);  
    else %absolute
        %values can range from -512 to 512
        angle=str2double(currentLine{4});
    end

    %Account for degrees or points:
    if(currentLine{7}==degrees && ~absolute)     
        %values can range from -149.5 to 149.5
        newVal=angle*(1024/300)+512;
        currentLine{4}=num2str(newVal);
    elseif(currentLine{7}==degrees) %& absolute
        newVal=angle*(1024/300);
        currentLine{4}=num2str(newVal);
    else
        
        %values can range from -512 to 512
        currentLine{4}=num2str(angle);
        
    end        
    
    %{
    %Set cID and addr:
    if(cmd==moveAbsolute || cmd==moveForward || cmd==moveBackward)
        cID=writeData;
        addr=goalPos;        
    elseif(cmd==setVelocity)
        cID=writeData;
        addr=movingSpeed;
    else %(cmd==pollMotor)
        cID=readData;
        addr=presentPos;
        poll=1;
    end
    %}
    
    %btL=currentLine{4} & FF
    
    %Calculate hex values for high and low data byte
    value=uint16(str2double(currentLine{4}));
    
    if(debug)
        disp(['motor ',num2str(mID),' value=',num2str(value),' points.']);
    end
    
    btL=dec2hex(double(bitand(value,uint16(255))));
    btH=dec2hex(double(bitshift(bitand(value,uint16(hex2dec('FF00'))),-8)));
    
    %Print array row to file
    if(mm)
        fprintf(fid,['moveMotor(',num2str(mID),',',num2str(value),');\n']);
    else
        fprintf(fid,['{',dur,',',num2str(mID),...',',cID,',',addr,
            ',0x',btL,',0x',btH]);
        
        %If we're at the end of the array, terminate it
        if(k==length(argArray))
            fprintf(fid,'}};\n');
        else
            fprintf(fid,'},\n');
        end
    end    
    %{
    %Include nRep and tRep if polling
    if(poll==1)
        tRep=currentLine{5};
        nRep=currentLine{6};
    else
        tRep='0';
        nRep='0';
    end
    
    %Print nRep and tRep to file
    fprintf(fid,[',',tRep,',',nRep]);
    %}

    
end

fclose(fid);

disp(['File ',fileName,' successfully ',action]);

end