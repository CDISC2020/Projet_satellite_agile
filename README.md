# [Projet satellite agile] #

# Mise a jour logiciel embarqué (sur les raspis) #
- Executer sendFolder.sh depuis le pc servant de station sol

	./sendFolder.sh

	(Copie le dossier Projet_satellite_agile vers les 2 raspis)
	(Il est conseillé de supprimer le dossier deja present sur les raspis)

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
	-Pour lancer la totalité du system executer le script launch_ARINC.sh :
		./launch_ARINC.sh

	Il est possible de lancer individuellement les partitions (sans ordonnancement) en executant :
		./main_PM	=> Plan manager
		./main_Com_ST	=> Communication sol-satellite
		./main_FDIR	=> FDIR

Sur un pc
-Aller dans le dossier Ground :

	./main_Ground

# Raspi #
-Pensez à vous connecter en ssh -X

