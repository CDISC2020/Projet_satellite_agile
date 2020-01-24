function [temps,coordonnees_bloquees]=instant_coordonnee(coordonnees,Omega)
global angle_limite altitude R l alpha Trev_sat Trev_terre omegat omegas delta_t delta_t_prim repos phi0 Torientation Tprise_image
%Omega = vitesse de rotation de la Terre ?
taille=size(coordonnees,1);
coordonnees_bloquees=[];
temps=[];
test=[];
k=0;
for c=1:taille
    L=coordonnees(c,2)*pi/180+pi;
    lambda=coordonnees(c,1)*pi/180+pi/2;
    frontL=mod(Omega-(alpha/2),2*pi);
    frontl=mod(lambda-(alpha/2),pi);
    %Calcul temps rencontre coordonnee zone couverte:
    if (frontL-L)>0  
        angleL_parcouru=frontL-L;
    else
        angleL_parcouru=frontl-L+2*pi;
    end;
    if (frontl-phi0)>0
        anglel_parcouru=(frontl-phi0);
    else
        anglel_parcouru=(frontl-phi0)+2*pi;
    end;
    t_s =angleL_parcouru/omegat;
    t_s_prim = anglel_parcouru/omegas;
    binf=(t_s-delta_t_prim-t_s_prim)/Trev_sat;
    bsup=(t_s+delta_t-t_s_prim)/Trev_sat;
    if (bsup<=0)
       k=1;
    elseif (binf<=0)
       n=0;
       t_coordonnee1=(t_s_prim+n*Trev_sat);
       t_coordonnee2=t_coordonnee1+delta_t_prim;
      
       if t_coordonnee1<t_s
           t_coordonnee1=t_s;
       end;
       if t_coordonnee2>t_s+delta_t;
           t_coordonnee2=t_s+delta_t;
       end;
       if (t_s >= Trev_terre-delta_t)
           t_s=t_s-Trev_terre;
           t_coordonnee1=t_coordonnee1-Trev_terre;
           t_coordonnee2=t_coordonnee2-Trev_terre;
           if t_coordonnee1<=0 & t_coordonnee2>=0
               if t_coordonnee2-t_coordonnee1>=Torientation +Tprise_image
                  t_coordonnee1=Torientation;
               else
                   k=1;
               end;
               
           elseif t_coordonnee1<=0 & t_coordonnee2<=0
               t_coordonnee1=t_coordonnee1+Trev_terre;
               t_coordonnee2=t_coordonnee2+Trev_terre;
           end;
       end;
       if t_coordonnee2-t_coordonnee1<Tprise_image
           k=1;
       end;
       
    else
        %% Intervalle entièrement inclus
       if bsup>=ceil(binf)
           n=ceil(binf);
           t_coordonnee1=(t_s_prim+n*Trev_sat);
           t_coordonnee2=t_coordonnee1+delta_t_prim;
           if t_coordonnee1<t_s
               t_coordonnee1=t_s;
           end;
           if t_coordonnee2>t_s+delta_t;
               t_coordonnee2=t_s+delta_t;
           end;
           
            if (t_s >= Trev_terre-delta_t)
               t_s=t_s-Trev_terre;
               t_coordonnee1/3600;
               t_coordonnee2/3600;
               t_coordonnee1=t_coordonnee1-Trev_terre;
               t_coordonnee2=t_coordonnee2-Trev_terre;
               if t_coordonnee1<=0 & t_coordonnee2>=0
                   if t_coordonnee2-t_coordonnee1>=Torientation +Tprise_image
                      t_coordonnee1=Torientation;
                   else
                      k=1;
                   end;

               elseif t_coordonnee1<=0 & t_coordonnee2<=0
                   t_coordonnee1=t_coordonnee1+Trev_terre;
                   t_coordonnee2=t_coordonnee2+Trev_terre;
               end;
           end;
           
           
           if t_coordonnee2-t_coordonnee1<=Tprise_image
            k=1;
           end;
       else
           k=1;
       end;
    end
    %% Blockage ou pas
    if k==1
        coordonnees_bloquees=[coordonnees_bloquees;[c,coordonnees(c,:)]];
        k=0;    
    else
        temps=[temps;[c,t_coordonnee1,t_coordonnee2,t_s]];
    end;

end


end

