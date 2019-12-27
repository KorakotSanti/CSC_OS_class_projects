#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <unistd.h>
#include <sys/wait.h>
#include <dirent.h>


using namespace std;

#define TEMP_DATALIST "datalist"

/**
 * stores information of the file content
*/
using processStruct = struct
{
    // index indicates process num
    int index;
    // indicates line number
    int linenum;
    // the string value of the text
    string lineval;
};

/**
 * Comparator for the sort function
 * @param x one of the processStruct object
 * @param y the other object of processStruct
 * @return true or false if x.linum is smaller return true else false
*/
bool sortline(processStruct x, processStruct y);

/**
 * Function that checks the output file path to make sure that the file has .c extension
 *  @param outfile the outfile string to be checked
 *  @return either the original parameter or outfile with the .c ext added
*/
string checkoutfile(string outfile);

/**
 * Function to get vector of filenames from the input directory
 * @param datadirc input data directory
 * @return vector of filenames
*/
vector<string> getFilename(const char* datadirc);

/**
 * function to get a 2d vector of process and its respective data it process from the file
 * @param n the number of "process" to create
 * @param filelist vector of filenames in data directory
 * @praram datadirc data directory path
 * @return 2d vector of process and its respective data
*/
void processfunc(int n, vector<string> filelist, const char* datadirc);

/**
 * function for child to write into a textfile the list of file names
 * which furthur child will work with
 * @param i index for child
 * @param filelist the list of all filenames
 * @param datadirc name of the the data directory
*/
void childDistribute(int i,vector<string> filelist,const char* datadirc);

/**
 * function for child process, which process and writes the part of its data files
 * in a single text file for future use
 * @param represents the index of the child process
 *
*/
void childProcess(int i);

/**
 * function to construct the output file
 * @param i index for the child process
 * @param output the output file to write into
*/
void parentProcess(int i, ofstream& output);

bool sortline(processStruct x, processStruct y){
    return x.linenum<y.linenum;
}


string checkoutfile(string outfile){

    //check last two char in string to see if it has .c ext
    if(outfile.substr(outfile.length()-2)==".c"){
        return outfile;
    }

    // to keep track of which index the following char searching for
    int index=0;

    // loop to find the last / which will indicate at what index the filename begins
    // in the outfile path
    for(int i=outfile.length()-1;i>0;i--){
        if(outfile[i]=='/'){
            index=i;
            break;
        }
    }

    // boolean to see if outfile has an ext or not
    bool hasext=false;

    // traverse through the filename part of outfile path to look for '.'
    // which indicate if file has ext or not
    for(int i=index;i<outfile.length();i++){
        if(outfile[i]=='.'){
            hasext=true;
            index=i;
            break;
        }
    }

    // check if has ext or not
    if(hasext){
        return outfile.substr(0,index)+".c";
    }
    else{
        return outfile+".c";
    }
}



vector<string> getFilename(const char* datadirc){

    // getting the filenames from directory
    // is copied from the code provided on working with directory
    DIR* directory = opendir(datadirc);
    if (directory==NULL){
        cout<< "data folder " << datadirc << "not found" << endl;
        exit(1);
    }

    struct dirent* entry;
    vector<string> fileName;
    while((entry=readdir(directory)) != NULL){
        const char* name = entry->d_name;
        if ((name[0] != '.') && (entry->d_type == DT_REG))
        {
            fileName.push_back(string(entry->d_name));
        }
    }
    closedir(directory);
    return fileName;
}



void processfunc(int n, vector<string> filelist, const char* datadirc,string outfile){

    int pids[n];
    // loop to create n child process to construct n text files with
    // file names
    for(int i=0;i<n;i++){
        pids[i]=fork();
        //check for failure,parent or child proc
        if(pids[i]<0){
            cout<<"Child Creation has failed..."<<endl;
            exit(-1);
        }
        // child process will create a file with the path name of
        // file genration 2 children will work with, each indvidual child
        // will have its own text file to write into
        else if (pids[i]==0){
            childDistribute(i,filelist,datadirc);
            exit(0);
        }
    }
    // the only process that comes out of the loop is the root parent function
    // the root parent function will wait for its children process to terminate
    for(int i=0;i<n;i++){
        int status;
        waitpid(pids[i],&status,0);
    }

    // generation 2 childrent that does the actually data process using
    // list of path in the the files
    ofstream output;
    output.open(outfile);
    for(int i=0;i<n;i++){
        pids[i]=fork();
        if(pids[i]<0){
            cout<<"Child Creation has failed..."<<endl;
            exit(-1);
        }
        else if (pids[i]==0){
            childProcess(i);
            exit(0);
        }
    }

    // wait for gen 2 children proc to terminate and run parent process
    for(int i=0;i<n;i++){
        int status;
        waitpid(pids[i],&status,0);
        parentProcess(i,output);
    }
    output.close();
}



void childDistribute(int i,vector<string> filelist,const char* datadirc){
    stringstream convert;
    convert << i;
    string index=convert.str();

    char* argslist[filelist.size()+4];
    int count=0;
    argslist[count++]=(char*)"./distributor";
    argslist[count++]=(char*)index.c_str();

    for(int k=0;k<filelist.size();k++){
        argslist[count++]=(char*)filelist[k].c_str();
    }
    argslist[count++]=(char*)datadirc;
    argslist[count++]=NULL;
    execvp(argslist[0],argslist);


}



void childProcess(int i){
    stringstream convert;
    convert << i;
    string index=convert.str();
    char* argslist[] = {(char*)"./processor",(char*)index.c_str(),NULL};
    execvp(argslist[0],argslist);
}



void parentProcess(int i, ofstream& output){
    // parent will read the text file with sorted fragments
    // that child had created
    ifstream infile;
    string indexfile=(string)TEMP_DATALIST+to_string(i)+".txt";
    infile.open(indexfile);

    // adds all the fragments to the output file
    string line;
    while(getline(infile,line)){
        output<<line+"\n";
    }
    infile.close();
}



int main(int argc, char*argv[]){

    //check for correct amount of args
    if(argc!=4){
        printf("invalid arguments\n");
        exit(-1);
    }

    // n is number of child process going to create
    int n = atoi(argv[1]);
    const char* datadirc = argv[2];
    string outfile = argv[3];

    // check output file to make sure that it ends with .c
    outfile=checkoutfile(outfile);

    // list of filenames in input directory
    vector<string> filelist;
    filelist=getFilename(datadirc);
    processfunc(n,filelist,datadirc,outfile);

    return 0;
}