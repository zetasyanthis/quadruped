function motionGen(argArray,swap,mm,debug)
%MOTIONGEN takes args argArray (cell array of cell arrays of
%{x,y,leg,delay}) and swap ('LR' or 'FB' or 'none')


%store previous position in order not to send duplicates
prevPos=zeros(12,1);

motionArray=cell(1,1);
mLng=1;

infoArray=argArray{1};
motionArray{1}=infoArray;

stepArray=argArray{2};

lng=length(stepArray);


for k=1:lng
    
    currentStep=stepArray{k};
    %currentStep:  cell of {delay, {4 x}, {4 y}, {4 z}};
    
    delay=currentStep{1};%string
    xPos=currentStep{2}; 
    yPos=currentStep{3};
    zPos=currentStep{4};
 
    currentArray=cell(5,1);
    positionArray=cell(4,1);
    
    for m=1:4;
        currentArray{1}=str2double(xPos{m});
        currentArray{2}=str2double(yPos{m});
        currentArray{3}=str2double(zPos{m});
        currentArray{4}=m; %leg
        currentArray{5}='0';
        positionArray{m}=currentArray;
    end
    
    
    %need positonArray=n*{x,y,leg,delay}
    
    
    
    
    [currentStepArray,prevPos]=stepGen(positionArray,prevPos,debug);
    
    currentLng=length(currentStepArray);
    
    if(mm)
        currentStepArray{currentLng+1}={delay};
        currentLng=currentLng+1;
    else
        currentStepArray{1}{1}=delay;
    end

    
    for n=1:currentLng
        motionArray{mLng+n}=currentStepArray{n};
    end
    
    mLng=mLng+currentLng;    
end



codeGen(motionArray,swap,mm,debug);

end