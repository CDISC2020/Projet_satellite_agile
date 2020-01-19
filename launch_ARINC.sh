>listpid
>pid_FDIR
>pid_COM
>pid_PM
xterm -e ./main_FDIR &
xterm -e ./main_Com_ST &
xterm -e ./main_PM &
echo '***** WAITING FOR INSTALL TO COMPLETE  *****'
for i in `seq 5`
do
 sleep 1
 echo -n '*-'
done
cat pid_FDIR pid_COM pid_PM > listpid
echo   
echo '***** LIST OF PROCESSES *****'
cat listpid

xterm -e ./src/ARINC/goK.sh
