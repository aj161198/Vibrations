clear;        
clc;

v = VideoReader('*path_for_video_file');

mean=0; j1 = 0;

t=0;xc=0;m=0;q1=1;wmean=0;

frames = v.NumberOfFrames;



%find mean

for j=16000 : 16500

    img= read(v,j);

    x=0;y=0;num=0;  

    for row=68:240

            for col=264:443

                if((img(row,col,1)>=64&&img(row,col,2)<=48&&img(row,col,3)<=50||img(row,col,1)>=95&&img(row,col,2)<=70&&img(row,col,3)<=70||img(row,col,1)>=50&&img(row,col,2)<=17&&img(row,col,3)<=17))

                    x=x+row;

                    y=y+col;

                    num=num+1;

                 end

             end

    end

    x=x/num;

    mean = mean+x;

end

mean = mean/500;

mean = round(mean);





p=1;

for j = 2000 : 2100

    img = read(v,j);

    x=0; y=0 ; num=0;

    xcp = xc;

    mp = m;

    ct = j;

    a1(j,1)=j;

    % threshold all video 

    for row=68:240

            for col=264:443

                if((img(row,col,1)>=64&&img(row,col,2)<=48&&img(row,col,3)<=58||img(row,col,1)>=95&&img(row,col,2)<=70&&img(row,col,3)<=70||img(row,col,1)>=50&&img(row,col,2)<=17&&img(row,col,3)<=17))

                    img(row,col,1)=255;

                    img(row,col,2)=255;

                    img(row,col,3)=255;

                    x=x+row;

                    y=y+col;

                    num=num+1;

                 end

             end

    end

    

     % mean line

     for row=mean-1:mean+1

            for col=300:380

                 img(row,col,1)=144;

                 img(row,col,2)=238;

                 img(row,col,3)=144;  

                end

     end

            

    %centroid

    

    x=x/num; y=y/num;

    xc=x;

    for row=68:240

            for col=264:443

                if (row<x+2 && row>x-2 && col<y+2 && col>y-2)

                 img(row,col,1)=0;

                 img(row,col,2)=0;

                 img(row,col,3)=0;  

                end

            end

    end



    % Calculate time period

     if ( xc - xcp  >0 )

        m = 1;

     else

        m=-1;

     end



 

     if( mp-m ==2)

         T(q1,1) = ct - t;

         w(q1,1) = 2*3.141/ (ct-t);  

         wmean=wmean+w(q1,1);

         t = ct;

         q1=q1+1;

     end

    

 

 if ( m ~= mp) % change in the sign of velocity

  j1= j1+1 ;  

 end

 osc = round(j1/2)-1;

 

 %position store

     x_sto(p,1)=x;

     p=p+1;  

 % found values using curve fitting tool (cftool)

 img=insertText(img,[50 50],['no of oscillation:  ' num2str(osc)]);

 img=insertText(img,[50 80],['Y displacement: ' num2str(x-mean,'%0.4f')]);

 img=insertText(img,[50 110],['Damping Ratio: 4.05 x 10^(-4) ']);

 img=insertText(img,[50 140],['Damping constant: 6.800 x 10^(-2) Kg/sec']);

 img=insertText(img,[50 170],['Stifness Constant: 181.063 N/m']);

 img=insertText(img,[100,300],['Equation of Motion:  9.7365exp(-0.009534t)sin(23.556t)' ],'FontSize',18,'BoxColor','red','BoxOpacity',0.8,'TextColor','white');

 imshow(img);

    

end

%to calculate mean 

wmean=wmean/q1;



% maxima minima

plot(x_sto);

%findpeaks(x_sto);

max=findpeaks(x_sto);

min=-(findpeaks(-x_sto));



% store extremum and amplitude

p=1;

for i =1:2:2*length(min)-1

ext(i,1)=min(p,1);

ext(1+i,1)=max(p,1);

p=p+1;

fc(p,1)=p;

end

amp= abs(ext- mean);



%max amplitude

ampmax=0;

for y =1:length(amp)

    if (amp(y,1)>ampmax)

        ampmax= amp(y,1);

    end

end



%save File 

csvwrite('Damped_Amplitude.csv',amp);