# [Projet satellite agile] #

# Compilation #
-Pour compiler :

	make ou make all
	
-Pour enlever fichiers temporaires créés lors de l'execution ou compilation :

	make clean
	
-Pour remettre le dossier dans un etat avant lancement et compilation :

	make reset
	
-Si lors du make le message "clock screw detected" apparait :

	make touch

# Run #
Sur raspi
-Executer le script launch_ARINC.sh :

	./launch_ARINC.sh

Sur un pc
-Aller dans le dossier Ground :

	./main_Ground

# Raspi #
-Pensez à vous connecter en ssh -X
