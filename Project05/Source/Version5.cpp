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
#include <fcntl.h>

using namespace std;

#define TEMP_DATALIST "datalist"
#define READ_END 0
#define WRITE_END 1
/**
 * @struct processStruct
 * Struct to store the file information
 *
 * @var processStruct::index 
 *    The index int of the file represents process num.
 * @var processStruct::linenum 
 *    Indicates the line number of the code fragment
 * @var processStruct::linval 
 *    the code fragment
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
 * function to start the distribution and process
 * @param n the number of "process" to create
 * @param filelist vector of filenames in data directory
 * @param datadirc data directory path
*/
void processfunc(int n, vector<string> filelist, const char* datadirc);

/**
 * function for the parent process of the distrbutor to do
 * which includes receiveing message from the processor and sending
 * message to the root process
 * @param mypipe[] connection to the root process
 * @param orpipe[] connection between distributor and processor
*/
void parentDistribute(int mypipe[], int orpipe[]);

/**
 * function for child to create vectors of filelist to give to the processor to process
 * @param i index for child
 * @param filelist the list of all filenames
 * @param datadirc name of the the data directory
 * @param mypipe[] connection between distributor and root process
*/

void childDistribute(int i,vector<string> filelist,const char* datadirc, int mypipe[]);

/**
 * process function where read all files stored in proc file sort them
 * and send only the code fragments back to distributor through pipe
 * @param procfile vector of the file name that will be proccessed
 * @param orpipe[] the pipe connection between processor and distributor
*/
void childProcess(vector<string> procfile,int orpipe[]);

/**
 * The root parent function to put together all the fragments
 * @param output the output file to write into
 * @param mypipe[] the pipe between root process and distributor
*/
void parentProcess(ofstream& output,int mypipe[]);


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
            parentProcess(output, mypipe[i]);
        }
    }
    output.close();
}



void parentDistribute(int mypipe[], int orpipe[]){
    // represents return value from read func
    int nread;
    char msg[BUFSIZ];
    // close the end of pipe not using
    close(orpipe[WRITE_END]);
    // string to put together all of the code fragments
    string confrags="";
    int len;

    // non blocking read until end of conversation
    while(1){
        nread=read(orpipe[READ_END],msg,BUFSIZ);
        switch(nread){
            // case -1 is when error occured either pipe is empty or
            // actually error
            case -1:
                if(errno==EAGAIN){
                    //usleep for non blocking read every 4000 micro second
                    usleep(4000);
                    break;
                }
                else{
                    cout<<"nonblock read error"<<endl;
                    exit(8);
                }
            // case 0 when end of conversation going to write the
            // completed fragment to root process
            case 0:
                close(orpipe[READ_END]);
                close(mypipe[READ_END]);
                len=confrags.length();

                // send first message to the parent
                // first message is the length of the string that is going to be send in the next write
                if (write(mypipe[WRITE_END],&len,sizeof(len))!=-1){
                    fflush(stdout);
                }
                else{
                    cout<<"write Error"<<endl;
                    exit(1);
                }

                if (write(mypipe[WRITE_END],confrags.c_str(),len+1)!=-1){
                    fflush(stdout);
                }
                else{
                    cout<<"Error"<<endl;
                    exit(2);
                }
                exit(0);
            // is when one line of fragment is being read
            default:
                string msgr(msg);
                confrags=confrags+msgr+"\n";
                fflush(stdout);
        }
    }

}



void childDistribute(int i,vector<string> filelist,const char* datadirc,int mypipe[]){
    // to hold all the file names
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

    // create a non blocking read pipe for connection between distributor and processor
    int orpipe[2];
    if (pipe(orpipe)<0){
        cout<<"Unable to pipe exiting..."<<endl;
        exit(4);
    }

    if (fcntl(orpipe[READ_END],F_SETFL,O_NONBLOCK)<0){
        cout<<"Unable to set nonblocking read exiting..."<<endl;
        exit(5);
    }

    // to create child of distributor which is the processor
    int p = fork();
    if (p<0){
        cout<<"fork failed"<<endl;
        exit(6);
    }
    // the child runs process func
    else if (p==0){
        childProcess(procfile,orpipe);
    }
    else{
        parentDistribute(mypipe,orpipe);
        }
}



void childProcess(vector<string> procfile,int orpipe[]){

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

    // close the read end of pipe
    close(orpipe[READ_END]);

    // going to write everything on the vector of structure
    for (int i=0;i<datavalue.size();i++){
        string temp = datavalue[i].lineval;
        if(write(orpipe[WRITE_END],temp.c_str(),temp.length()+1)==-1){
            cout<<"write error"<<endl;
            exit(7);
        }
        fflush(stdout);
        usleep(10000);
    }
    close(orpipe[WRITE_END]);
    exit(0);
}



void parentProcess(ofstream& output,int mypipe[]){
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