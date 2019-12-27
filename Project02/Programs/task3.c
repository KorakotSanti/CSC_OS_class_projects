#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
struct to store information about a certain category
*/
typedef struct Category {
	int index;		// represent unique index for each Category struct created
	int numrange;	// represents the amount of different range in the category
	char* name;		// represents name of category
	char* low;		// lower bound of the range
	char* high;		// upper bound of the range
} Category;

/*
struct to store All of the defined categories and the number of defined category
*/
typedef struct AllCategories{
	int numCategory;	// the amount of defined category
	Category *allCate;	// pointer to a array of Category structure
}AllCategories;

int check(char c,AllCategories all);
int checkRange(char c, char from, char to);
int checkCategory(char c,Category care);
int comparator(const void* a, const void* b);
char* removeRepeating(char* s);
char* fullSort(char* s);

/*
Function which takes a char and the AllCategories struct
and then return the index of the defined category or
return the num of category which is the index for the etc category
*/
int getCategoryindex(char c, AllCategories all){
	for (int i = 0;i<all.numCategory;i++){
		if (checkCategory(c,all.allCate[i])){
			return all.allCate[i].index;
		}
	}

	return all.numCategory;

}

/*
Function which return 1/true or 0/false if char c is within 
the range of low and high
*/
int checkRange(char c, char low, char high){

	if (c>=low && c<=high){
		return 1;
	}

	return 0;
}

/* 
function for the qsort, this this returns the lower then higher value between the char
*/
int checkCategory(char c,Category cate){
	for (int i=0;i<cate.numrange;i++){
		if(checkRange(c,cate.low[i],cate.high[i])){
			return 1;
		}
	}

	return 0;
}

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
	// creating stuct variable
	AllCategories cate;
	// setting number of Category
	cate.numCategory=2;
	// allocating array based on number of category
	cate.allCate = (Category*)malloc(sizeof(Category)*cate.numCategory);

	// creating every defined category datatype
	Category letter = {0,2,"letter","aA","zZ"};
	Category number = {1,1,"number","0","9"};

	// add category stuct to the array based on its unique index
	cate.allCate[letter.index]=letter;
	cate.allCate[number.index]=number;

	// totalCategory represents total number of category
	int totalCategory = cate.numCategory+1;

	//acollating char array with 3 rows, one for letters, numbers, and etc
	char** sorting = (char**)calloc(totalCategory,sizeof(char*));
	// the sorted string
	char* resultStr = (char*)calloc(strlen(s)+1,sizeof(char));

	// create int of array of count that represents count for all the category
	int count[totalCategory];
	// intialize all the value in the count array to 0
	memset(count,0,sizeof(count));

	// traverse through string
	for (int i=0;s[i]!='\0';i++){
		// to count each number of each category
		// w represents the unique index for each category
		int w = getCategoryindex(s[i],cate);
		count[w]++;
	}

	// allocate to heap the the 2d array based on number of letter,numbers, and etc
	for (int i = 0; i<totalCategory;i++){
		sorting[i]=(char*)calloc(count[i]+1,sizeof(char));
		sorting[i][count[i]]='\0';
	}

	// to represent the index of the 2d array
	int startin[totalCategory];
	memset(startin,0,sizeof(startin));

	// tranverse through string adding each value to respective array
	for (int i=0;s[i]!='\0';i++){
		// calls getCategory to get unqiue index for each category
		int w = getCategoryindex(s[i],cate);
		sorting[w][startin[w]++]=s[i];
	}

	// qsort each row of the array by ASCII value
	for (int i = 0; i<totalCategory;i++){
		qsort(sorting[i],count[i],sizeof(char),comparator);
	}

	// free the array of struct that was allocated.
	free(cate.allCate);

	// index for the output string
	int index=0;
	// traverse through all of the 2d array adding each value to the output string
	for (int i = 0; i<totalCategory; i++){
		for (int j = 0; j<count[i]; j++){
			resultStr[index++] = sorting[i][j];
		}
	}

	// string ends with terminate 0
	resultStr[index]='\0';

	// freeing the 2d array from the heap
	for(int i = 0 ;i<totalCategory;i++){
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