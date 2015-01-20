function codeGenGUI()

height=170;
width=600;

f = figure('Visible','off','Position',[360,500,width,height],'KeyPressFcn',{@runIfEnter});

%********%
% Panels %
%********%
dataPanel=   uipanel('Units','pixels','Position',[.025*width,height-83,.95*width,80], 'Visible','on');
arrayPanel=  uipanel('Units','pixels','Position',[.025*width,height-83,1,1], 'Visible','on');

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

cmdNum= uicontrol(dataPanel,'Style','popupmenu','BackgroundColor','white','HorizontalAlignment','right','Max',255,'Min',0,'String',cellArray, 'Position',[.7*width,8,40,20],'Callback',{@resizeCallback});

%**********%
% Checkbox %
%**********%
checkAppend= uicontrol(dataPanel,'Style','checkbox','String','Append file','Value',1,'Position',[.8*width,8,80,25],'Callback',{@toggleAppend});

%*********%
% Buttons %
%*********%
hgen=    uicontrol(f,'Style','pushbutton','String','Generate', 'Position',[width-300,5,70,25],'Callback',{@runSim,'none'},'KeyPressFcn',{@runIfEnter});
hswapLR= uicontrol(f,'Style','pushbutton','String','Swap L-R', 'Position',[width-225,5,70,25],'Callback',{@runSim,'LR'});
hswapFB= uicontrol(f,'Style','pushbutton','String','Swap F-B', 'Position',[width-150,5,70,25],'Callback',{@runSim,'FB'});
hclose=  uicontrol(f,'Style','pushbutton','String','Exit','Position',[width-75,5,70,25], 'Callback',{@closeProgram});

hfile= uicontrol(dataPanel,'Style','pushbutton','String','Select File','Position',[.95*width-78,40,70,25],'Callback',{@openFile});

%*************%
% Static text %
%*************%

numCmdsText=     uicontrol(dataPanel,'Style','text','String','Number of commands: ', 'Position',[.48*width,10,120,15]);
arrayNameText=   uicontrol(dataPanel,'Style','text','String','Array name: ',         'Position',[2,10,70,15]);

tText=       uicontrol(dataPanel,'Style','text','String','t ',      'Position',[.02*width-5,-25,20,15]);
motorIDText= uicontrol(dataPanel,'Style','text','String','motorID', 'Position',[.12*width-5,-25,50,15]);
cmdText=     uicontrol(dataPanel,'Style','text','String','command', 'Position',[.21*width,-25,50,15]);
valueText=   uicontrol(dataPanel,'Style','text','String','value',   'Position',[.4*width,-25,40,15]);
tRepText=    uicontrol(dataPanel,'Style','text','String','tRep',    'Position',[.68*width,-25,40,15]);
nRepText=    uicontrol(dataPanel,'Style','text','String','nRep',    'Position',[.78*width-10,-25,60,15]);


%*******************%
% Data array inputs %
%*******************%

tBoxes(1)=        uicontrol(arrayPanel,'Style','edit',     'BackgroundColor','white','HorizontalAlignment','right','String','',                                     'Position',[.02*width,-48,50,20]);
mIDBoxes(1)=      uicontrol(arrayPanel,'Style','edit',     'BackgroundColor','white','HorizontalAlignment','right','String','',                                     'Position',[.12*width,-48,40,20]);
cmdBoxes(1)=      uicontrol(arrayPanel,'Style','popupmenu','BackgroundColor','white','HorizontalAlignment','right','String',{'moveAbsolute','moveForward','moveBackward','setVelocity','pollAbsolute','pollForward','pollBackward'},'Position',[.21*width,-48,100,20]);
valueBoxes(1)=    uicontrol(arrayPanel,'Style','edit',     'BackgroundColor','white','HorizontalAlignment','right','String','',                                     'Position',[.4*width,-48,70,20]);
degPointBoxes(k)= uicontrol(arrayPanel,'Style','popupmenu','BackgroundColor','white','HorizontalAlignment','right','String',{'points','degrees'},                   'Position',[.52*width,-48,70,20]);
tRepBoxes(1)=     uicontrol(arrayPanel,'Style','edit',     'BackgroundColor','white','HorizontalAlignment','right','String','0',                                    'Position',[.68*width, -48,50,20]);
nRepBoxes(1)=     uicontrol(arrayPanel,'Style','edit',     'BackgroundColor','white','HorizontalAlignment','right','String','0',                                    'Position',[.78*width, -48,50,20]);

%*******************%
% Initialize values %
%*******************%

set(dataPanel,'Title','Array information');

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

currNumBoxes=1;



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

%***********************%
% Array resize function %
%***********************%
    function resizeCallback(source,eventdata)
        numBoxes=get(cmdNum,'Value');
        if(currNumBoxes>numBoxes)
            set(dataPanel,'Position',[.025*width,height-83,.95*width,80]);
            %remove boxes
        elseif(currNumBoxes<numBoxes)
            
            %add boxes
            diffBoxes=numBoxes-currNumBoxes;
            height=height+diffBoxes*25;
            
            
            set(f,'Units','Pixels');
            pos=get(f,'Position');
            pos(4)=height;
            
            set(f,'Position',pos);
            set(dataPanel, 'Position',[.025*width,height-83,.95*width,80]);
            set(arrayPanel,'Position',[.025*width,height-83,1,1]);
            movegui(f,'center');
            
            for k=currNumBoxes:numBoxes
                
                tBoxes(k)=        uicontrol(arrayPanel,'Style','edit',     'BackgroundColor','white','HorizontalAlignment','right','String','',                                     'Position',[.02*width,(-25*k)-23,50,20]);
                mIDBoxes(k)=      uicontrol(arrayPanel,'Style','edit',     'BackgroundColor','white','HorizontalAlignment','right','String','',                                     'Position',[.12*width,(-25*k)-23,40,20]);
                cmdBoxes(k)=      uicontrol(arrayPanel,'Style','popupmenu','BackgroundColor','white','HorizontalAlignment','right','String',{'moveAbsolute','moveForward','moveBackward','setVelocity','pollAbsolute','pollForward','pollBackward'},'Position',[.21*width,(-25*k)-23,100,20]);
                valueBoxes(k)=    uicontrol(arrayPanel,'Style','edit',     'BackgroundColor','white','HorizontalAlignment','right','String','',                                     'Position',[.4*width,(-25*k)-23,70,20]);
                degPointBoxes(k)= uicontrol(arrayPanel,'Style','popupmenu','BackgroundColor','white','HorizontalAlignment','right','String',{'points','degrees'},                   'Position',[.52*width,(-25*k)-23,70,20]);
                tRepBoxes(k)=     uicontrol(arrayPanel,'Style','edit',     'BackgroundColor','white','HorizontalAlignment','right','String','0',                                    'Position',[.68*width,(-25*k)-23,50,20]);
                nRepBoxes(k)=     uicontrol(arrayPanel,'Style','edit',     'BackgroundColor','white','HorizontalAlignment','right','String','0',                                    'Position',[.78*width,(-25*k)-23,50,20]);
                
            end

            set(f,'Units','normalized');
            
        end
        
        currNumBoxes=numBoxes;
      
        
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
        
        numCommands=get(cmdNum,'Value');
        arrayNameVal=get(arrayName,'String');
        
        arrayData={arrayNameVal,numCommands,fileName,append};
        
        
        %Fill data parameters
        params=cell(numCommands+1,1);
        params{1}=arrayData;
        
        currCommand=cell(6,1);
        
        for k=1:numCommands
            currCommand{1}=get(tBoxes(k),'String');
            currCommand{2}=get(mIDBoxes(k),'String');
            currCommand{3}=get(cmdBoxes(k),'Value');
            currCommand{4}=get(valueBoxes(k),'String');
            currCommand{5}=get(tRepBoxes(k),'String');
            currCommand{6}=get(nRepBoxes(k),'String');
            currCommand{7}=get(degPointBoxes(k),'Value');
            
            params{1+k}=currCommand;
        end
        
        %close(f);
        
        codeGen(params,swap);
       
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
