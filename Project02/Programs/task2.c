#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int comparator(const void* a, const void* b);
char* removeRepeating(char* s);
char* fullSort(char* s);

/* 
function for the qsort, this this returns the lower then higher value between the char
*/
int comparator(const void *a, const void *b){
	// char to represent a and b 
	char firstchar = *(const char*)a;
	char secondchar = *(const char*)b;
	// if the first char in the parameter is less than the second
	// it will be sorted before
	if (firstchar<secondchar){
		return -1;
	}
	if (firstchar>secondchar){
		return 1;
	}
	return 0;
}

/* 
This function takes a string remove any repeating val in the string
and return that string
*/
char* removeRepeating(char* s){
	// len is the string length and resultStr is the string without any
	// repeating value
	int len = strlen(s),pos=0;
	char* resultStr = (char*)malloc(len+1);

	// brute force method by traversing through the entire string
	for(int i = 0; i<len;i++){
		// control to see if we add a particular char to the new string
		int control=0;
		// checks every char before the current char, if any char matches 
		// change val of control and end the loop
		for(int j = i-1; j>=0; j--){
			if(s[i]==s[j]){
				control=1;
				break;
			}
		}
		// if control val did not change within the nested loop
		// then add the char to the new string
		if (control==0){
			resultStr[pos++]=s[i];
		}
	}
	// string always ends with terminate 0
	resultStr[pos]='\0';
	return resultStr;
}

/* 
Function that sort string by letter,number, then any other
the letter will be sorted by ASCII same with number and any other
*/
char* fullSort(char* s){
	//acollating char array with 3 rows, one for letters, numbers, and etc
	char** sorting = (char**)calloc(3,sizeof(char*));
	// the sorted string
	char* resultStr = (char*)calloc(strlen(s)+1,sizeof(char));
	// array of 3 int that represents the amount of letter,number,and etc
	int count[3] = {0,0,0};

	// counts the # of letter, numbers, and etc
	for (int i=0;s[i]!='\0';i++){
		if ((s[i]>= 'a' && s[i] <= 'z') || (s[i]>='A' && s[i]<='Z') ){
			count[0]++;
		}
		else if (s[i]>='0' && s[i]<='9'){
			count[1]++;
		}
		else {
			count[2]++;
		}
	}

	// allocate to heap the the 2d array based on number of letter,numbers, and etc
	for (int i = 0; i<3;i++){
		sorting[i]=(char*)calloc(count[i]+1,sizeof(char));
		sorting[i][count[i]]='\0';
	}

	// to represent the index of the 2d array
	int lstart=0,nstart=0,etcstart=0;

	// tranverse through string adding each value to respective array
	for (int i = 0; i<=strlen(s);i++){
		if ((s[i]>= 'a' && s[i] <= 'z') || (s[i]>='A' && s[i]<='Z') ){
			sorting[0][lstart++]=s[i];
		}
		else if (s[i]>='0' && s[i]<='9'){
			sorting[1][nstart++]=s[i];
		}
		else {
			sorting[2][etcstart++]=s[i];
		}
	}

	// qsort each row of the array by ASCII value
	for (int i = 0; i<3;i++){
		qsort(sorting[i],count[i],sizeof(char),comparator);
	}

	// index for the output string
	int index=0;
	// traverse through all of the 2d array adding each value to the output string
	for (int i = 0; i<3; i++){
		for (int j = 0; j<count[i]; j++){
			resultStr[index++] = sorting[i][j];
		}
	}

	// string ends with terminate 0
	resultStr[index]='\0';

	// freeing the 2d array from the heap
	for(int i = 0 ;i<3;i++){
		free(sorting[i]);
	}
	free(sorting);
	return resultStr;

}
int main(int argc, char* argv[]){

	// to hold string input
	char str[100];
	// while loop that asks for input until user input EXIT
	do {
		printf("enter a string: ");
		// to scan user input
		scanf("%[^\n]",str);
		char c = getchar();
		(void) c;
		// if string entered is not EXIT
		if (strcmp(str,"EXIT")){
			// get the new string with no repeating characters
			char* newstr = removeRepeating(str);
			// get the sorted string
			char* sortedStr = fullSort(newstr);
			// need to free from heap
			free(newstr);
			printf("\"%s\"\n",sortedStr);
			// need to free from heap
			free(sortedStr);
		}

	} while(strcmp(str,"EXIT"));

	return 0;
}