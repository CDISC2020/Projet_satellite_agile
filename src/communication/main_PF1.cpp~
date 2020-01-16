#include <iostream>
#include <string>
#include <unistd.h>
using namespace std;

#include "../ARINC/ARINC_Com.h"
#include "statusManager.h"
#include "../planManager/planManager.h"
#include <dirent.h>
#include <vector>

PlanFilePath pfp;

int getdir (string dir, vector<string> &files)
{
    DIR *dp;
    struct dirent *dirp;
    if((dp  = opendir(dir.c_str())) == NULL) {
        cout << "Error(" << errno << ") opening " << dir << endl;
        return errno;
    }

    while ((dirp = readdir(dp)) != NULL) {
        files.push_back(string(dirp->d_name));
    }
    closedir(dp);
    return 0;
}


int main (int argc,char* argv[]) 
{
cout<<"Je suis le PF1, respo com"<<endl;

if (argc!=2) 
{
	printf("T'as oublié l'argument banane!le hostname... \n");
	exit (-1);
}

// Suite si hostname en argument

cout << "Host name " << argv[1] << endl; 

QueuingPort channelOutPM(0, 18001, argv[1]);

channelOutPM.Display();

while (1) {

	//channel.SendQueuingMsg(str, sizeof(str));	
	
	sleep(5);
			string dir = string("./TestSatelliteSaturne");
    			vector<string> files = vector<string>();

    			getdir(dir,files);

    			files.erase(files.begin());
    			files.erase(files.end());
			char p[64];
			std::vector<char*> cstrings; 

			cstrings.reserve(files.size());

    			for(size_t i = 0; i < files.size(); ++i){
				cout << files[i] << files[i].size() << " <---------File\n";
				files[i].copy(pfp.filepath, files[i].size()+1);
				//cout << p << " <---------P\n";
				channelOutPM.SendQueuingMsg((char*)&pfp, sizeof(PlanFilePath));
				//pfp.filepath = p;
        			//cstrings.push_back(const_cast<char*>(files[i].c_str()));
				//cout << cstrings[i] << " <---------\n";				
				//pfp.filepath[i] = reinterpret_cast<char*> (&cstrings[i]);
			}
		    	/*for (unsigned int i = 0;i < files.size();i++) {
				char c[];			
				for (int g=0; g < files[i].size(); g++)		
					c[g] = files[i][g];
				pfp.filepath[i] = c;
		    	}*/

			//cout << pfp.filepath << " buffer pfp \n";

			//channelOutPM.SendQueuingMsg((char*)&cstrings, sizeof(cstrings));

}

}
