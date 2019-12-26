>listpid
xterm -e ./main_Control localhost &
xterm -e ./main_run localhost &
xterm -e ./main_Com_ST localhost &
echo '***** WAITING FOR INSTALL TO COMPLETE  *****'
for i in `seq 20`
do
 sleep 1
 echo -n '*-'
done
echo   
echo "Liste des pid"
ps | grep -e xterm | grep -v grep | awk '{ print $1 }' > listpid
echo '***** LIST OF PROCESSES *****'
cat listpid

xterm -e ./goK.sh
