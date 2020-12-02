function [ Matrice_coord, Matrice_nom ] = conversion( )

%% DEBUT 8:47                                                             CONVERSION CSV EN .TXT                     #TEAMPLANIF   #2020
%clc;
%clear all;
%close all
%Recuperation du fichier .csv
tic;
[file,path]=uigetfile('*.csv','Parcourir');
formatSpec = '%f %f %s %s';
f_recup = fopen(fullfile(path,file),'r'); % choisir le fichier a traiter
data_client = textscan(f_recup, formatSpec,...
                     'headerlines', 1,...
                     'delimiter',',',...
                     'TreatAsEmpty','NA'); % lecture du fichier
fclose(f_recup); % fermeture du fichier
if f_recup==-1
    f = errordlg('File not found','File Error');
end
Matrice_coord=[data_client{1,1} data_client{1,2}];
Matrice_nom=[cellstr(data_client{1,4})];
timer=toc;
X=sprintf('%f seconds to open and extract data from file',timer);
f=msgbox(X);

end

