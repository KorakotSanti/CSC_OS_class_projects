#include <stdio.h>
#include <stdlib.h>
#include <string.h>
char* removeRepeating(char* s);
char* partialSort(char* s);

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
function takes a string and sort it by letter, numbers, and then any other char
*/
char* partialSort(char* s)
{
	// new sorted string
	char *resultStr = (char*)malloc(strlen(s)+1);
	// num to store number of letters, numbers, and etc respectively
	int acount=0,ncount=0,scount=0;

	// loop through string counting letter,number, and etc
	for (int i=0;s[i]!='\0';i++){
		if ((s[i]>= 'a' && s[i] <= 'z') || (s[i]>='A' && s[i]<='Z') ){
			acount++;
		}
		else if (s[i]>='0' && s[i]<='9'){
			ncount++;
		}
		else {
			scount++;
		}
	}

	// astart represent where the letter starts in the string
	int astart=0;
	// represent where number starts in the string
	int nstart=acount;
	// where etc starts in the string
	int sstart=acount+ncount;

	// traverse through string sorting the string
	for (int i = 0; i<=strlen(s);i++){
		if ((s[i]>= 'a' && s[i] <= 'z') || (s[i]>='A' && s[i]<='Z') ){
			resultStr[astart++]=s[i];
		}
		else if (s[i]>='0' && s[i]<='9'){
			resultStr[nstart++]=s[i];
		}
		else {
			resultStr[sstart++]=s[i];
		}
	}
	//string ends with terminate 0
	resultStr[sstart]='\0';
	
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
			char* sortedStr = partialSort(newstr);
			// need to free from heap
			free(newstr);
			printf("\"%s\"\n",sortedStr);
			// need to free from heap
			free(sortedStr);
		}

	} while(strcmp(str,"EXIT"));

	return 0;
}


