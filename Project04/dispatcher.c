#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

/** Function to produce an input path to image
*	@param indir	  	The name of the path to the directory
*   @param imgname   	The name of the image 
*	@return input path to image
*/
char* buildinputPath(char* indir, char* imgname);
/** Function to check the arguements if they are valid or not
*	@param argc	  		The number of total arguments
*   @param argv   		The list of arguments
*	@return 1 if invalid arguments or 0 when it is valid
*/
int checkArguments(int argc, char** commandslist, int numcommand);

/** Function to get number of commands
*	@param argc	  		The number of total arguments
*   @param argv   		The list of arguments
*	@return number of commands
*/
int numCommands(int argc, char* argv[]);

/** Function get number of total commands from argument list
*	@param argc	  		The number of total arguments
*   @param argv   		The list of arguments
*	@param indexarray 	Array to hold the index of each command
*	@param numcommand   number of commands there are 
*	@return array of commands
*/
char** getCommandsList(int argc, char*argv[],int *indexarray,int numcommand);

/** Function to create child process and runs command in the commandslist
*	@param numcommand       The number of commands
*	@param commandslist     List of commands
*	@param indexs			List of index where command is stored in arguement list
*	@param indir            input directory path name
*   @param output           Output diretory
*   @param argv             Argument list
*   @return Number of failed program execution
*/
int dispatching(int numcommand, char** commandslist,
				int* indexs, char* indir, char* output, char* argv[]);


/** Function that depending on the command execute the command with the
*   right amount of arguments
*	@param argv       		Argument list
*	@param index            index which represent where command is in the arguement list
*	@param indir            input directory path name
*   @param output           Output diretory
*   @param command          the name of the command
*/
void runCommand(char* argv[], int index, char* indir, char* output, char* command);


char* buildinputPath(char* indir, char* imgname){
	// to allocate right amount of memory to create the the image path
	// +2 to account for "/" and NULL character
	char* inPath = malloc(strlen(indir)+strlen(imgname)+2*sizeof(char));

	// copy to the newly created char* and concatate the rest
	strcpy(inPath,indir);
	strcat(inPath,"/");
	strcat(inPath,imgname);

	return inPath;
}

int checkArguments(int argc, char** commandslist,int numcommand){
	// to count amount of rotate, split, and crop command
	int numrot=0,numsplit=0,numcrop=0;

	// loop through commandslist to count the different command
	for (int i = 0; i<numcommand;i++){
		if(!strcmp(commandslist[i],"rotate")){
			numrot++;
		}
		else if (!strcmp(commandslist[i],"crop")){
			numcrop++;
		}
		else{
			numsplit++;
		}
	}

	// to get the correct total argument
	// the +3 at the end to count the prog exe name, inputpath, and output path
	int totalarg=(numrot*3)+(numsplit*2)+(numcrop*6)+3;
	if(argc!=totalarg){
		return 1;
	}

	// if there is no problems with arguments
	return 0;
}

int numCommands(int argc, char* argv[]){
	int numcommand=0;
	// count for the three possible command in argv
	for(int i = 3; i<argc;i++){
		if(!strcmp(argv[i],"rotate") || !strcmp(argv[i],"split") || !strcmp(argv[i],"crop"))
			numcommand++;
	}

	return numcommand;
}

char** getCommandsList(int argc, char*argv[],int *indexarray,int numcommand){

	// allocate enough space for the right amount of command string to store
	char** commandslist = malloc(numcommand*sizeof(char*));

	// represent index for commands list
	int commandin=0;

	// loop through argv finding each of the three command and adding that
	// command to the commandslist and adding the index the command found at in the 
	// index list
	for(int i = 3; i<argc;i++){
		if (!strcmp(argv[i],"rotate")){
			commandslist[commandin]=malloc(7*sizeof(char));
			commandslist[commandin]=argv[i];
			indexarray[commandin++]=i;
		}
		else if(!strcmp(argv[i],"split")){
			commandslist[commandin]=malloc(6*sizeof(char));
			commandslist[commandin]=argv[i];
			indexarray[commandin++]=i;
		}
		else if(!strcmp(argv[i],"crop")){
			commandslist[commandin]=malloc(5*sizeof(char));
			commandslist[commandin]=argv[i];
			indexarray[commandin++]=i;
		}
	}
	return commandslist;
}

int dispatching(int numcommand, char** commandslist,
				int* indexs, char* indir, char* output, char* argv[]){
	// count the number of program that failed
	int numFail=0;

	// loop through commandlist creating the exact amount of child as commands
	// there are
	for (int i =0; i<numcommand;i++){

		int p = fork();

		// checks the p for child, parent, or fail creation
		if (p<0){
			printf("Fail Child Creation\n");
			exit(-1);
		}

		// if child run command function occurs
		else if (p==0){
			runCommand(argv,indexs[i],indir,output,commandslist[i]);
		}

		// if parent wait for child to terminate and check if it failed or not
		else{
			int status;
			pid_t termproc = waitpid(p,&status,0);
			// if WEXITSTATUS returns a 0 means child process successful
			if (WEXITSTATUS(status)!=0){
				numFail++;
			}
		}
	}
	return numFail;
}

void runCommand(char* argv[], int index, char* indir, char* output, char* command){

	// check for the command,
	// depending on the particular command
	// execute the program with x number of arguments 
	if (!strcmp(command,"rotate")){
		char exe[9]="./";
		strcat(exe,command);
		char* inPath = buildinputPath(indir,argv[index+2]);
		execlp(exe,argv[index],argv[index+1],inPath,output,NULL);
	}
	else if (!strcmp(command,"split")){
		char exe[8]="./";
		strcat(exe,command);
		char* inPath = buildinputPath(indir,argv[index+1]);
		execlp(exe,argv[index],inPath,output,NULL);
	}
	else{
		char exe[7]="./";
		strcat(exe,command);
		char* inPath = buildinputPath(indir,argv[index+5]);
		execlp(exe,argv[index],inPath,output,argv[index+1],argv[index+2],
				argv[index+3],argv[index+4],NULL);
	}


	return;
}

int main(int argc, char* argv[]){

	// first argument check
	// check if there are at least 3 argument
	if (argc<4){
		printf("invalid arguments\n");
		return 1;
	}

	char* inputdir=argv[1];
	char* outputdir=argv[2];

	// get number of command
	int numcommand=numCommands(argc,argv);
	// represent the index of where each command is located
	int indexs[numcommand];
	// get list of commands from argv
	char** commandslist=getCommandsList(argc,argv,indexs,numcommand);

	// check if there are enough arguments in argv
	if(checkArguments(argc,commandslist,numcommand)){
		printf("invalid arguments\n");
		return 1;
	}

	// run dispatcher which creates child process to run the commands and return
	// the number of fail operators
	int numFails = dispatching(numcommand,commandslist,indexs,inputdir,outputdir,argv);
	free(commandslist);
	return numFails;
}