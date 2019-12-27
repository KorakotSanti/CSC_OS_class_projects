#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <unistd.h>
#include <sys/wait.h>
#include <dirent.h>

/*
This is the distributor program which is used to create a file with a list of file names
for the future child processes to process those file
*/
using namespace std;

#define TEMP_DATALIST "datalist"

/*
The usually arguments in argv would be {"exename","index","list of paths to file","data diriectory path"
*/
int main(int argc, char* argv[]){
    // need to convert i to integer for later use
    int i = atoi(argv[1]);
    // the last value in argv is always going to be data directory
    string datadir = argv[argc-1];

    // file going to be writing into
    ofstream tempoutput;
    string tempfile=(string)TEMP_DATALIST+to_string(i)+".txt";

    tempoutput.open(tempfile);

    // traversing through all the file path
    // checking if the index of the file is equal to index if so
    // write the file name to the file
    for(int j =1;j<argc-1;j++){
        string rfile = (string)datadir+"/"+(string)argv[j];
        ifstream infile;
        infile.open(rfile);
        int index;
        infile >> index;
        if (index==i){
            tempoutput<<rfile+"\n";
        }
        infile.close();
    }
    tempoutput.close();

    return 0;
}