#ifndef PLAN
#define PLAN

#include <iostream>
#include <stdlib.h> 
#include <stdio.h>
#include <string.h>
#include <fstream>

#include "genericInstruction.h"

using namespace std;

const int maxInstructions = 100;

class Plan
{
	protected:
		GenericInstruction* ActivityList[maxInstructions]; 
		int nInstructions;
	public:
		Plan();
		int getnInstructions();
		void printPlan();
		GenericInstruction* getInstruction(int i);
		void loadPlan(const char* filepath);
		bool stockInstruction(GenericInstruction* newInstruction);
		int ptInstruction;
};

#endif
