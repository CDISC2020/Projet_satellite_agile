#include "plan.h"

using namespace std;


Plan::Plan()
{
	this->nInstructions = 0;
	this->ptInstruction = 0;
}

void Plan::printPlan()
{
	for (int i =0; i < this->nInstructions; i++)
	{
		this->ActivityList[i]->printInstruction();
	}
}

bool Plan::stockInstruction(GenericInstruction * newInstruction)
{
	if (nInstructions == maxInstructions)
	{
		return false;
	}
	else
	{
		ActivityList[nInstructions] = newInstruction;
		nInstructions++;
		return true;
	}
}

void Plan::loadPlan(const char* filepath)
{
	ifstream  readFile;
	readFile.open(filepath);
	string type;
	
	cout << "Starting the reading of a the plan "<< filepath<< endl;
	
	GenericInstruction * planInstruction;
	string planLine;
	int line = 0;
	
	if (readFile.is_open())	
	{
		cout << "\t File " << filepath<< " opened \n";
		while(  getline (readFile, planLine) ) 
		{
			if (planLine != "\n" && planLine.length()>6)
			{
				type = planLine.substr(6,1);
				
				if (type == "a")
				{
					planInstruction = new AttitudeInstruction(planLine); 
				} 
				
				else if (type == "p") 
				{
					planInstruction = new PhotoInstruction(planLine); 
				}
				else
				{
					//Message d'erreur!
				}
				this->stockInstruction(planInstruction);
				line ++;
			}
		}

		cout << "Lines read: "<< line << endl;
		readFile.close();
	}
	else
	{
		cout <<endl <<"File not found bitch!!!!"<< endl;	
	}
}

int Plan::getnInstructions()
{
	return this->nInstructions;
}

GenericInstruction* Plan::getInstruction(int i)
{
	if (i < this->nInstructions)
	{
		return ActivityList[i];
	}
	else
	{
		/* Error in the number of the instruction. Number out of range */
		return NULL;
	}
}


