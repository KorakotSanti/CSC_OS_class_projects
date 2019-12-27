#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <sys/stat.h>
#include <dirent.h>
#include <string>
#include <algorithm>

using namespace std;

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
vector<vector<processStruct>> processfunc(int n, vector<string> filelist, const char* datadirc);


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

    DIR* directory = opendir(datadirc);
    if (directory==NULL){
        cout<< "data folder" << datadirc << "not found" << endl;
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



vector<vector<processStruct>> processfunc(int n, vector<string> filelist, const char* datadirc){
    vector<vector<processStruct>> datainfo;
    datainfo.resize(n);

    int fperproc = filelist.size()/n;
    int extra = filelist.size()%n;

    int index=0;
    for(int i=0;i<n;i++){
        int numfile=fperproc;
        if(i<extra){
            numfile++;
        }
        for(int j=0;j<numfile;j++){
            ifstream infile;
            string rfile=(string)datadirc+"/"+filelist[j+index];
            infile.open(rfile);
            processStruct x;
            infile>>x.index>>x.linenum;
            getline(infile,x.lineval);
            datainfo[i].push_back(x);
        }
        index+=numfile;
    }

    return datainfo;
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

    // a 2d vector where each process contains processStruct vectors of the file data
    vector<vector<processStruct>> datainfo;
    datainfo=processfunc(n,filelist,datadirc);

    // 2d vector to 1d vector
    vector<processStruct> totalinfo;
    for(int i=0;i<n;i++){
        for(int j=0;j<datainfo[i].size();j++){
            totalinfo.push_back(datainfo[i][j]);
        }
    }

    // sort vector using numline
    sort(totalinfo.begin(),totalinfo.end(),sortline);

    // write into the outputfile
    ofstream output;
    output.open(outfile);
    for(int i=0;i<totalinfo.size();i++){
        output<<totalinfo[i].lineval+"\n";
    }

    output.close();
    return 0;
}