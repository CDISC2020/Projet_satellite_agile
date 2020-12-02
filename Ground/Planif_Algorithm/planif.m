clc;
clear all;

%% Paramètres orbites/station sol
global angle_limite altitude R l alpha Trev_sat Trev_terre omegat omegas delta_t delta_t_prim repos phi0 Torientation Tprise_image
angle_limite=67.43*pi/180; %angle limite rotation pitch tourelle  67.43 est la valeur pour couvrir tous les points
altitude=700000; %satelitte
R=6371000;
l=2*altitude*tan(angle_limite)/sqrt(2); 
alpha=2*atan(l/(2*R));
Trev_sat=90*60; %période sat
Trev_terre=24*3600; %période terre
omegat=2*pi/Trev_terre;
omegas=2*pi/Trev_sat;
delta_t=alpha/omegat; %Fenêtre temporelle de couverture longitudinale du satellite
delta_t_prim=alpha/omegas; %Fenêtre temporelle de couverture lattitudinale du satellite
Torientation = 27;
Tprise_image = 1;
repos=127; %angle repos tourelle
Tsol=365.25*24*3600;
omegasol=2*pi/Tsol;
duree_ombre=alpha/(2*omegasol);
%% Prise en compte du cas jour/nuit
    %Variables
    Omega1=20.2251300*pi/180;  %Station kiruna
    phi01=67.8557200*pi/180;
        
    Omega1=pi+Omega1;
    phi01=phi01+pi/2;
    
    Omega2=pi+Omega1;
    phi02=2*pi-phi01;
    %Differents cas
if datenum('01-Jan-2020')+seconds(duree_ombre) <= datenum(now) <= datenum('30-Jun-2020')-seconds(duree_ombre)
        Omega=Omega1;
        phi0=phi01;
elseif datenum('01-Jul-2020')+seconds(duree_ombre) <= datenum(now) <= datenum('31-Dec-2020')-seconds(duree_ombre)
        Omega=Omega2;
        phi0=phi02;
        
else
    disp('Vous êtes dans une journée inplanifiable par la version courante de cet algorithme!'); 
end;


%% Algo de recherche d'intersection coordonée zone couverte:
[coordonnees,capitaux]=conversion();
[temps,coordonnees_bloquees]=instant_coordonnee(coordonnees,Omega); 



%% Planification:       
T_total = Tprise_image+Torientation;
%Trier le vecteur sortie
Plan_prise=sortrows(temps,2);
Replanif=[];
i=1;
N=(size(Plan_prise,1)-1);
Plan=[[Plan_prise(1,1),Plan_prise(1,2),Plan_prise(1,2),Plan_prise(1,4)]];

while i<=N
    if Plan_prise(i+1,2)-Plan_prise(i,2) <=T_total
        if Plan_prise(i+1,3)-Plan_prise(i,2) <=T_total
            Replanif=[Replanif;Plan_prise(i+1,:)];
            Plan_prise(i+1,:)=[];
            N=N-1;
        else
            Plan=[Plan;[Plan_prise(i+1,1),Plan_prise(i+1,2),Plan_prise(i,2)+T_total,Plan_prise(i+1,4)]];
            Plan_prise(i+1,2)=Plan_prise(i,2)+T_total;
            i=i+1;
        end;
        
    else
        Plan=[Plan;[Plan_prise(i+1,1),Plan_prise(i+1,2),Plan_prise(i+1,2),Plan_prise(i+1,4)]];
        i=i+1;        
    end;
end;
Plan_vf=[];
N=size(Plan_prise,1);
for i=1:N
    deltaO1=omegat*(Plan(i,3)-Plan(i,4))-l/(2*R);
    deltaO2=omegas*(Plan(i,2)+delta_t_prim/2-Plan(i,3));
    yaw=atan(deltaO2/deltaO1);
    yaw=yaw*180/pi+repos;
    pitch=atan(R*sqrt(deltaO1^2+deltaO2^2)/altitude);
    pitch=pitch*180/pi+repos;
    Plan_vf=[Plan_vf;[Plan(i,1),Plan(i,3),pitch,yaw]];
end;
Plan=Plan_vf;
Plan=sortrows(Plan,2);
%% Bilan

sprintf('La planification est réussie: Sur %d coordonnées %d coordonnées sont planifiées,%d sont à replanifier et %d sont bloquées',length(coordonnees), length(Plan),length(Replanif),length(coordonnees_bloquees)) 

%% Création du fichier .txt
format long
nom_plan=strcat('Plan_',date,'.txt');
fid = fopen(nom_plan,'w');
N=size(Plan,1);
for i=1:N
    yaw=round(Plan(i,4));
    if yaw<=10
        yaw=strcat('00',num2str(yaw));
    elseif yaw<=100
        yaw=strcat('0',num2str(yaw));
    else    
        yaw=num2str(yaw);
    end;
    pitch=round(Plan(i,3));
    if pitch<=10
        pitch=strcat('00',num2str(pitch));
    elseif pitch<=100
        pitch=strcat('0',num2str(pitch));
    else    
        pitch=num2str(pitch);
    end;
    roll='000';
    Index=Plan(i,1);
    p = datestr(seconds(Plan(i,2)-Torientation),'HHMMSS');
    fprintf(fid,'%sa%s%s%s %d\n',p,roll,yaw,pitch,Index);
    
    p = datestr(seconds(Plan(i,2)),'HHMMSS');
    exposure=1000;
    fprintf(fid,'%sp%dPhoto_%s %d\n',p,exposure,strrep(capitaux{Index},' ',''),Index);
    
end;

fclose(fid);

%% Création CSV replanif
if length(Replanif)>0
    Replanif=sortrows(Replanif,1);
    Replanif=[Replanif(:,1) coordonnees(Replanif(:,1),:)];
    nom_replan=strcat('Replanif_',date,'.csv');
    csvwrite(nom_replan,Replanif);
end;
%% CALCUL DE LANGLE LIMITE
val1=Trev_sat/((1/omegas)+(1/omegat));   % LE DENOM EST DELTAT PLUS DELTATPRIMEZ
ll=2*R*tan(val1/2);
lll=ll/(sqrt(2)*altitude);
angle_optimal_couverture=atand(lll);

