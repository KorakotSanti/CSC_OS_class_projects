#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <unistd.h>
#include <sys/wait.h>
#include <dirent.h>

/*
The processor porgram does the processing of the data files that the distributor has
distributed to the different child processes to process the data file
*/
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


bool sortline(processStruct x, processStruct y){
    return x.linenum<y.linenum;
}

/*
The only argument other that the executable nae this program takes is the index
to ensure that the processor is working with the right files
*/
int main(int argc, char* argv[]){
    int i=atoi(argv[1]);
    // reading the path name stored in text file
    ifstream infile;
    string infilename=(string)TEMP_DATALIST+to_string(i)+".txt";
    infile.open(infilename);

    vector<processStruct> datavalue;
    // read line by line using a struct to capture all the values in the data file
    // and storing that struct in a vector
    string line;
    while(getline(infile,line)){
        if(line!=""){
            ifstream readfile;
            readfile.open(line);
            processStruct x;
            readfile>>x.index>>x.linenum;
            getline(readfile,x.lineval);
            datavalue.push_back(x);
            readfile.close();
        }
    }
    infile.close();

    // sort the data value using its line number
    sort(datavalue.begin(),datavalue.end(),sortline);

    // using the same file name that was used to store the file names
    // will overwrite that file
    ofstream outfile;
    outfile.open(infilename);

    // write all of the code fragment in the order of the vector onto file
    for(int i=0;i<datavalue.size();i++){
        outfile<<datavalue[i].lineval+"\n";
    }
    outfile.close();

}