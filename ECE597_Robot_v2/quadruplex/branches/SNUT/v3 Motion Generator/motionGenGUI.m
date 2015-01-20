function motionGenGUI()

height=400;
width=600;

currNumSteps=1;
currentStep=1;
stepArray=cell(1,1);
visitedArray=ones(1,1);

defaultDelay='10';
defaultX='0';
defaultY='14';
defaultZ='8.6';

defaultArray={defaultDelay,{defaultX,defaultX,defaultX,defaultX},{defaultY,defaultY,defaultY,defaultY},{defaultZ,defaultZ,defaultZ,defaultZ}};
stepArray{1}=defaultArray;

f = figure('Visible','off','Position',[360,500,width,height],'KeyPressFcn',{@runIfEnter});


%********%
% Panels %
%********%
dataPanel=   uipanel('Units','pixels','Position',[.025*width,height-83,.95*width,80], 'Visible','on');
motionPanel=  uipanel('Units','pixels','Position',[.025*width,35,.95*width,height-130], 'Visible','on');

%********%
% Fields %
%********%
fileSelector= uicontrol(dataPanel,'Style','edit','String','Select a file','Position',[5,40,.9*width-70,22],'Enable','inactive');
arrayName=    uicontrol(dataPanel,'Style','edit','BackgroundColor','white','HorizontalAlignment','right','String','','Position',[.13*width,8,200,20]);
gray=get(fileSelector,'BackgroundColor');

%***************%
% Dropdown Menu %
%***************%
cellArray=cell(255,1);
for k=1:255
    cellArray{k}=k;
end

numSteps= uicontrol(dataPanel,  'Style','popupmenu','BackgroundColor','white','HorizontalAlignment','right','Max',255,'Min',1,'String',cellArray, 'Position',[.7*width,8,40,20],'Callback',{@resizeCallback});
stepNum=  uicontrol(motionPanel,'Style','popupmenu','BackgroundColor','white','HorizontalAlignment','right','Max',currNumSteps,'Min',1,'String',cellArray{1:currNumSteps}, 'Position',[.5*width-40,8,40,20],'Callback',{@gotoStep,'dd'});


%**********%
% Checkbox %
%**********%
checkAppend=    uicontrol(dataPanel,'Style','checkbox','String','Append file','Value',1,'Position',[.8*width,8,80,25],'Callback',{@toggleAppend});
checkMoveMotor= uicontrol(f,'Style','checkbox','String','Generate as moveMotor','Value',0,'Position',[.03*width,8,150,25]);
checkDebug=     uicontrol(f,'Style','checkbox','String','Debug mode','Value',0,'Position',[.3*width,8,150,25]);
checkRetain=    uicontrol(f,'Style','checkbox','String','Retain values','Value',0,'Position',[.68*width,40,90,25]);

%*********%
% Buttons %
%*********%
hgen=    uicontrol(f,'Style','pushbutton','String','Generate', 'Position',[width-300,5,70,25],'Callback',{@runSim,'none'},'KeyPressFcn',{@runIfEnter});
hswapLR= uicontrol(f,'Style','pushbutton','String','Swap L-R', 'Position',[width-225,5,70,25],'Callback',{@runSim,'LR'});
hswapFB= uicontrol(f,'Style','pushbutton','String','Swap F-B', 'Position',[width-150,5,70,25],'Callback',{@runSim,'FB'});
hclose=  uicontrol(f,'Style','pushbutton','String','Exit','Position',[width-75,5,70,25], 'Callback',{@closeProgram});

hfile= uicontrol(dataPanel,'Style','pushbutton','String','Select File','Position',[.95*width-78,40,70,25],'Callback',{@openFile});

hNext= uicontrol(motionPanel,'Style','pushbutton','String','Next Step >', 'Position',[width-110,5,70,25],'Callback',{@gotoStep,'next'});
hPrev= uicontrol(motionPanel,'Style','pushbutton','String','< Prev Step', 'Position',[5,5,70,25],'Callback',{@gotoStep,'prev'});

%*******%
% Image %
%*******%

imInfo=imfinfo('robo.jpg');
im=imread('robo.jpg');
hImg=imshow(im);
imHt=imInfo.Height;
imWidth=imInfo.Width;
imAxis=get(hImg,'Parent');
set(imAxis,'Parent',motionPanel,'Units','Pixels','Position',[(width+imWidth)/4,(height+imHt)/4-40,imWidth,imHt]);

%*************%
% Static text %
%*************%

numStepsText=    uicontrol(dataPanel,'Style','text','String','Number of steps: ', 'Position',[.48*width,10,120,15]);
arrayNameText=   uicontrol(dataPanel,'Style','text','String','Array name: ',      'Position',[2,10,70,15]);

currStepText=uicontrol(motionPanel,'Style','text','String','Step: ', 'Position',[.5*width-80,3,40,20]);

stepDurationText=uicontrol(motionPanel,'Style','text','String','Step duration (x10ms): ', 'Position',[.5*width-110,220,120,15]);

legX=[30,30,425,425];
legY=[100,230,230,100];

for k=1:4
    xText(k)=   uicontrol(motionPanel,'Style','text','String','x:',               'Position',[legX(k)-15,legY(k)-25,20,15]);
    yText(k)=   uicontrol(motionPanel,'Style','text','String','y:',               'Position',[legX(k)+55,legY(k)-25,20,15]);
    zText(k)=   uicontrol(motionPanel,'Style','text','String','z:',               'Position',[legX(k)-15,legY(k)-50,20,15]);
    legText(k)= uicontrol(motionPanel,'Style','text','String',['Leg ',num2str(k)],'Position',[legX(k),   legY(k),   30,15]);
end
    

%*******************%
% Data array inputs %
%*******************%

for k=1:4
    xBoxes(k)= uicontrol(motionPanel,'Style','edit','BackgroundColor','white','HorizontalAlignment','right','String',defaultX, 'Position',[legX(k)+5, legY(k)-25,40,20]);
    yBoxes(k)= uicontrol(motionPanel,'Style','edit','BackgroundColor','white','HorizontalAlignment','right','String',defaultY,'Position',[legX(k)+70,legY(k)-25,40,20]);
    zBoxes(k)= uicontrol(motionPanel,'Style','edit','BackgroundColor','white','HorizontalAlignment','right','String',defaultZ, 'Position',[legX(k)+5, legY(k)-55,40,20]);
end

stepDuration=uicontrol(motionPanel,'Style','edit','BackgroundColor','white','HorizontalAlignment','right','String',defaultDelay, 'Position',[.5*width+10,218,40,20]);

%*******************%
% Initialize values %
%*******************%

set(dataPanel,'Title','Array information');
set(motionPanel,'Title','Step information');

%components=[f,hsim,hclose,hfile,... %buttons
%    positionPanel,selectPanel,infoPanel... % panels
%    distanceBoxText,authorBoxText,companyBoxText,dateBoxText,mfrBoxText,numHorizText,numVertText,... %text
%    lampBoxText,tiltBoxText,numLampsBoxText,lumensBoxText,multiplierBoxText,ballastFactorBoxText,...
%    widthBoxText,lengthBoxText,heightBoxText,inputHeightBoxText,...
%    fileSelector,distanceBox,authorBox,companyBox,dateBox,mfrBox,lampBox,tiltBox,numLampsBox,...%inputs
%    lumensBox,multiplierBox,ballastFactorBox,widthBox,lengthBox,heightBox,inputHeightBox,...
%    numVertBox,numHorizBox,radioButtons,unitsRadioMeters,unitsRadioFeet];

%set(components,'Units','normalized');
%
set(f,'Units','normalized','Name','Motion Array Generator');%,'Position',[0,0,.3,.3]);
movegui(f,'center')

%Show functions and refresh plot
set(f,'Visible','on');

%**************************%
% Checkbox toggle function %
%**************************%
    function toggleAppend(source,eventdata)
        append=get(checkAppend,'Value');
        
        if(append==0)
            set(fileSelector,'String','Enter file name','BackgroundColor','white','Enable','on');
        else
            set(fileSelector,'String','Select a file','BackgroundColor',gray,'Enable','inactive');
        end
    end

%********************%
% Goto step function %
%********************%
    function gotoStep(source,eventdata,step)
        
        if(strcmp(step,'prev'))
            step=currentStep-1;
            retain=0;
        elseif(strcmp(step,'next'))
            step=currentStep+1;
            retain=get(checkRetain,'Value');
        else
            step=get(stepNum,'Value');
            retain=0;
        end
        
        if(step<=0)
            disp(['step=',num2str(step),' and currentStep=',num2str(currentStep),' and currNumSteps=',num2str(currNumSteps)]);
            currentStep=1;
            
            disp('Step out of range (too low)');
            return;
        elseif(step>currNumSteps)
            disp(['step=',num2str(step),' and currentStep=',num2str(currentStep),' and currNumSteps=',num2str(currNumSteps)]);
            currentStep=currNumSteps;
            disp('Step out of range (too high)');
            return;
        end
        
        if(get(stepNum,'Value')~=step)
            set(stepNum,'Value',step);
        end
        
        newStepData=stepArray{step};
        
        newStepDuration=newStepData{1};
        newXData=newStepData{2};
        newYData=newStepData{3};
        newZData=newStepData{4};
        
        saveStep();

        

        if(~retain || visitedArray(step))
            set(stepDuration,'String',newStepDuration);

            for k=1:4
                set(xBoxes(k),'String',newXData{k});
                set(yBoxes(k),'String',newYData{k});
                set(zBoxes(k),'String',newZData{k});
            end

        end
        
        visitedArray(step)=1;
        currentStep=step;
    end

%********************%
% Save step function %
%********************%
    function saveStep()
        saveStepDuration=get(stepDuration,'String');

        saveXData=cell(4,1);
        saveYData=cell(4,1);
        saveZData=cell(4,1);
        
         for k=1:4
            saveXData{k}=get(xBoxes(k),'String');
            saveYData{k}=get(yBoxes(k),'String');
            saveZData{k}=get(zBoxes(k),'String');       
         end
         
         saveStepData={saveStepDuration,saveXData,saveYData,saveZData};
         stepArray{currentStep}=saveStepData;
    end
         

%***********************%
% Array resize function %
%***********************%
    function resizeCallback(source,eventdata)
        
        newNumSteps=get(numSteps,'Value');
        
        for k=currNumSteps:newNumSteps
            stepArray{k}=defaultArray;
        end

        newArray=cell(newNumSteps,1);
        newVisitedArray=zeros(newNumSteps,1);
        
        for k=1:newNumSteps
            newArray{k}=k;
            if(k<=currNumSteps)
                newVisitedArray(k)=visitedArray(k);
            end
        end
        
        currNumSteps=newNumSteps;
        visitedArray=newVisitedArray;
        
        val=get(stepNum,'Value');
        
        set(stepNum,'Max',currNumSteps,'String',newArray,'Value',val);
        
    end

%*************************%
% Run simulation function %
%*************************%
    function runSim(source,eventdata,swap)
        
        %Fill array data parameter
        fileName=get(fileSelector,'String');
        
        if(strcmp(fileName,'Select a file') || strcmp(fileName,'Enter file name'))
            fileName='array.c';
        end
        
        if(get(checkAppend,'Value')==1)
            append=1;
        else
            append=0;
        end
        
        numCommands=get(stepNum,'Value');
        arrayNameVal=get(arrayName,'String');
        
        saveStep();
        
        arrayData={arrayNameVal,numCommands,fileName,append};
        paramArray={arrayData,stepArray};
        
        %close(f);
        
        mm=get(checkMoveMotor,'Value');
        debug=get(checkDebug,'Value');
        motionGen(paramArray,swap,mm,debug);
       
    end



%************************************%
% Run simulation function (Keyboard) %
%************************************%
    function runIfEnter(source,eventdata)
        
        k=eventdata.Key;
        
        if strcmp(k,'escape')
            pause(0.01)
            
            closeProgram(source,eventdata);
            
        elseif strcmp(k,'return') %if enter was pressed
            pause(0.01) %allows time to update
            
            %call the pushbutton callback.
            runSim(source,eventdata);
        end
        
    end

%********************%
% Select file button %
%********************%
    function openFile(source,eventdata)
        
        [img,PathName,FilterIndex] = uigetfile('*.*','Select a file:');
        set(fileSelector,'String',[PathName,img]);
        
    end

%*************%
% Exit button %
%*************%
    function closeProgram(source,eventdata)
        
        close(f);
        
    end

end
