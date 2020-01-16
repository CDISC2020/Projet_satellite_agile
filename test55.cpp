#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <vector>
#include <string>
#include <iostream>

using namespace std;

/*function... might want it in some class?*/
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

int main()
{
    string dir = string("./TestSatelliteSaturne");
    vector<string> files = vector<string>();

    getdir(dir,files);

    files.erase(files.begin());
    files.erase(files.end());
    for (unsigned int i = 0;i < files.size();i++) {
        cout << i << files[i] << endl;
    }
    return 0;
}
