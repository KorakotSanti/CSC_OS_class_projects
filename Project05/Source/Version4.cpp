#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <unistd.h>
#include <sys/wait.h>
#include <dirent.h>
#include <string.h>

using namespace std;

#define TEMP_DATALIST "datalist"
#define READ_END 0
#define WRITE_END 1
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
 * @param mypipe[] the pipeline for child message to parent
*/
void childDistribute(int i,vector<string> filelist,const char* datadirc, int mypipe[]);

/**
 * function for child process, which process and writes the part of its data files
 * in a single text file for future use
 * @param temp name of the temporary output textfile
 * @param procfile the vector of filenames for child to process
 *
*/
void childProcess(string temp, vector<string> procfile);

/**
 * function to construct the output file
 * @param output the file output
 * @param mypipe[] the pipeline for message
 * @param p the pid of child
*/
void parentProcess(ofstream& output,int mypipe[],int p);

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
    int mypipe[n][2];
    for(int i=0;i<n;i++){
        if(pipe(mypipe[i])==-1){
            cout<<"error"<<endl;
        }
    }
    ofstream output;
    output.open(outfile);
    // loop to create n child process to construct n text files with
    // file names
    for(int i=0;i<n;i++){
        int p=fork();
        //check for failure,parent or child proc
        if(p<0){
            cout<<"Child Creation has failed..."<<endl;
            exit(-1);
        }
        // child process will create a file with the path name of
        // file genration 2 children will work with, each indvidual child
        // will have its own text file to write into
        else if (p==0){
            childDistribute(i,filelist,datadirc,mypipe[i]);
            exit(0);
        }
        else{
            int status;
            waitpid(p,&status,0);
            parentProcess(output, mypipe[i],p);
        }
    }
    output.close();
}



void childDistribute(int i,vector<string> filelist,const char* datadirc,int mypipe[]){
    string temp=(string)TEMP_DATALIST+to_string(i)+".txt";
    vector<string> procfile;

    for(int j=0;j<filelist.size();j++){
        string rfile=(string)datadirc+"/"+filelist[j];
        ifstream infile;
        infile.open(rfile);
        int index;
        infile >> index;
        if(i==index){
            procfile.push_back(rfile);
        }
        infile.close();
    }

    int p = fork();
    if (p<0){
        cout<<"fork failed"<<endl;
        exit(3);
    }
    else if (p==0){
        childProcess(temp,procfile);
    }
    else{
        int status;
        waitpid(p,&status,0);
        ifstream frag;
        frag.open(temp);
        string line;
        string confrags="";
        while(getline(frag,line)){
            confrags=confrags+line+"\n";
        }
        frag.close();
        close(mypipe[READ_END]);
        int len = confrags.length();

        // send first message to the parent
        // first message is the length of the string that is going to be send in the next write
        if (write(mypipe[WRITE_END],&len,sizeof(len))!=-1){
            fflush(stdout);
        }
        else{
            cout<<"Error"<<endl;
            exit(1);
        }

        if (write(mypipe[WRITE_END],confrags.c_str(),len+1)!=-1){
            fflush(stdout);
        }
        else{
            cout<<"Error"<<endl;
            exit(2);
        }
    }

}



void childProcess(string temp, vector<string> procfile){

    vector<processStruct> datavalue;
    // read line by line using a struct to capture all the values in the data file
    // and storing that struct in a vector
    for(int i=0;i<procfile.size();i++){
        ifstream readfile;
        readfile.open(procfile[i]);
        processStruct x;
        readfile>>x.index>>x.linenum;
        getline(readfile,x.lineval);
        datavalue.push_back(x);
        readfile.close();

    }

    // sort the data value using its line number
    sort(datavalue.begin(),datavalue.end(),sortline);

    // using the same file name that was used to store the file names
    // will overwrite that file
    ofstream outfile;
    outfile.open(temp);

    // write all of the code fragment in the order of the vector onto file
    for(int i=0;i<datavalue.size();i++){
        outfile<<datavalue[i].lineval+"\n";
    }
    outfile.close();
}



void parentProcess(ofstream& output,int mypipe[],int p){
    close(mypipe[WRITE_END]);
    int len;
    // first read the length of the next string to come
    if(read(mypipe[READ_END],&len,sizeof(len))!=-1){
        fflush(stdout);
    }
    else {
        cout<<"READ ERROR"<<endl;
        exit(4);
    }

    // create char array with the right amount of size for the incoming string
    char remsg[len+1];
    if(read(mypipe[READ_END],remsg,len+1)!=-1){
        output<<remsg;
        fflush(stdout);
    }
    else{
        cout<<"READ ERROR"<<endl;
        exit(5);
    }
    remsg[len]='\0';
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