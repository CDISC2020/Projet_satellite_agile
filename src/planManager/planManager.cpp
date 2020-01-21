#include "planManager.h"

using namespace std;

PlanManager::PlanManager()
{
	plan=NULL;
	responseController=0;
}

void PlanManager::executePlan(Controller* control,QueuingPort* ChannelCom, bool mode)
{
	if(plan!=NULL)
	{
		GenericInstruction* currentInst;

		bool jumpInstruction;

		currentInst = plan->getInstruction(plan->ptInstruction);
		jumpInstruction = false;

		time_t t = time(0);
		struct tm * now = localtime(&t);
		cout << "Inst " << plan->ptInstruction+1 << "/" << plan->getnInstructions() << endl;
		cout << "now         = " << now->tm_hour << ":" << now->tm_min << ":" << now->tm_sec << endl;
		cout << "Instruction = " << currentInst->getHour() << ":" << currentInst->getMin() << ":" << currentInst->getSec() << endl;

		int group = currentInst->getIndex();
		if ((bannedInstructions[group]) == true) jumpInstruction = true;

		if ((jumpInstruction == false))
		{
			if ((now->tm_hour == currentInst->getHour()) & (now->tm_min == currentInst->getMin()) & (now->tm_sec == currentInst->getSec()))
			{
			/*
			/ Codes des messages a envoyer :
			/ 0 = Attitude
			/ 1 = Photo
			/ 2 = RetourController
			/ 3 = PlanFilePath
			/ 4 = Status
			/ 5 = data
			*/
				cout<<"C'est l'heure "<<endl;
				bool start_timeout = false;
				/*--------------Call Controller to execute instruction-------------*/
				if(currentInst->getType() == 'p' || currentInst->getType() == 'a')
				{
					cout << "control EXECUTE!" << endl;
					if(mode)
						control->executeInstruction(currentInst, &responseController);
					else
						responseController=1;
					start_timeout = true;
				}
				else
				{
					cout << "hum probleme" << endl;
					Status S;
					S.code = 4;
					S.errorID = 2; // A changer.
					sprintf(S.description, "Wrong instruction type, instruction #%d : group #%d ", plan->ptInstruction+1,  group);

					ChannelCom->SendQueuingMsg((char*)&S, sizeof(Status));
				}
				/*---------------------------------------------------------*/

				/*-------------Wait for Time Out of the controller response-----------*/
				int time_out = 1000000;
				if (start_timeout == true)
				{
					while (time_out > 0 && responseController == 0)
					{
						usleep(1);
						time_out--;
					}
				}
				/*-------------------------------------------------------------------*/

				/*--------------------   Management of the errors    ----------------*/
				if (responseController != 1)
				{
					bannedInstructions[group] = true;
					if (responseController == -1)
					{
						cout<<"Instruction "<<group<<" banned : error from controller"<<endl;
						Status S_controller;
						S_controller.code = 4;

						if (currentInst->getType() == 'a')
						{
							// The attitude was not reached
							S_controller.errorID = 0; 	// Look the error list
							sprintf(S_controller.description, "Attitude not reached. instruction #%d : group #%d ", plan->ptInstruction+1,  group);
						}
						if (currentInst->getType() == 'p')
						{
							// The photo was not taken
							S_controller.errorID = 1; // Look the error list
							sprintf(S_controller.description, "Photo not talken. instruction #%d : group #%d ", plan->ptInstruction+1,  group);
						}

						cout << S_controller.description << endl;

						ChannelCom->SendQueuingMsg((char*)&S_controller, sizeof(Status));
					}
					if (time_out == 0)
					{
						cout<<"Instruction "<<group<<" banned : timeout."<<endl;
						// Time out in the response of the attitude controller
					}
				}
				/*-------------------------------------------------------------------*/

				/*-----------------   Photo sending to the ComGrdMnger  -------------*/
				if ( responseController == 1  && currentInst->getType() == 'p')
				{
					PlanFilePath PhotoPath;
					PhotoPath.code = 3;
					string aux =  ((PhotoInstruction*)currentInst)->getPhotoName();
					strcpy (PhotoPath.filepath , aux.c_str()) ;
					printf("Sending the photo %s \n", PhotoPath.filepath);
					ChannelCom->SendQueuingMsg((char*)&PhotoPath, sizeof(PlanFilePath));
				}
				/*-------------------------------------------------------------------*/

				responseController = 0;
				plan->ptInstruction++;


			}
			// Cas où le temps de l'instruction est déjà passé... on l'aura jamais !
			else if ( (now->tm_hour > currentInst->getHour())
				|| ((now->tm_hour == currentInst->getHour()) && (now->tm_min > currentInst->getMin()))
				|| ((now->tm_min  == currentInst->getMin())  && (now->tm_sec > currentInst->getSec())))
			{
				cout<<"L'heure est deja passé "<<plan->ptInstruction<<endl;
				Status S_heure;
				S_heure.code = 4;
				S_heure.errorID = 3;
				ChannelCom->SendQueuingMsg((char*)&S_heure, sizeof(Status));

				plan->ptInstruction++;
			}
		}
		else if (jumpInstruction)
		{
			plan->ptInstruction++;
			cout<<"Instruction jumped"<<endl;
		}


		if (plan->ptInstruction >= plan->getnInstructions()) // Fin d'un plan d'instruction, on efface le plan en cours
		{
			cout << "Le plan est terminé" << endl;
			this->destructPlan();
		}
	}

	else
		cout << "No plan to execute !" << endl;
}

void PlanManager::generatePlan(const char* filepath)
{
	cout << "GENERATE PLAN" << endl;

	string s = filepath;
	plan = new Plan();
	plan->loadPlan(filepath);

	for (int k=0; k< 100; k++) bannedInstructions[k] = false;

	cout<<"Plan ajouté"<<endl;
}

void PlanManager::printPlan()
{
	if(plan!=NULL)
		plan->printPlan();
	else
		cout << "No plan!" << endl;
}

void PlanManager::pushBan(int index)
{
	bannedInstructions[index]= true;
}

void PlanManager::destructPlan()
{
	if(plan!=NULL)
	{
		delete plan;
		plan=NULL;
	}
	for (int k=0; k< 100; k++) bannedInstructions[k] = false;
}

Plan* PlanManager::backup()
{
	return plan;
}

int PlanManager::getNInstru()
{
	if(plan!=NULL)
		return plan->getnInstructions();
	return -1;
}

void PlanManager::recover(Plan* planBack)
{
	cout << "recover" << endl;
	this->destructPlan();
	plan=planBack;
}
