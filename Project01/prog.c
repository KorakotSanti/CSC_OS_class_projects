/* CSC 412 Prog_01 Assignment
   Korakot Santiudommongkol */
#include<stdio.h>
#include<stdlib.h>

int isPrime(int num);
void firstNPrime(int n);
void primeNumBetween(int a, int b);

/* This functions takes the parameter num and 
check if the integer is a prime number or not
*/
int isPrime(int num){

	// check for cases if num is 1 since 1 is not a prime number
	// and my solution does not account for num==1 in the for loop
	if (num==1) {
		return 0;
	}

	// for loop to check every single number starting from 2 until int num
	// the loop checks for any possible factors if there are a factor before reaching
	// num then num is not a prime number
	for(int i = 2; i<num; i++){
		if (num%i==0){
			return 0;
		}
	}
	// if loop through all the possible factors of num return 1 
	return 1;
}

/* This functions prints out the first n prime numbers*/
void firstNPrime(int n){

	// case when n = 1 the output would not be pulural
	if(n==1){
		printf("The first prime number is ");
	}
	else {
		printf("The first %d prime numbers are: ",n);
	} 
    
    // count to count for number of prime numbers
    // and number represents the possible prime numbers
	int count = 0, number = 2;

	//loop until you get n prime numbers
	while (count<n){
        //checks if var number is prime or not
        // if it is then print that number and increment count
		if (isPrime(number)==1){
            //if else statment for output format
			if (count==n-1){
				printf("%d.\n",number);
			}
			else {
				printf("%d, ",number);
			}
			count++;
		}
		//number increment each time
		number++;
	}
}

/* This functions prints out the prime numbers between 
int a and int b*/
void primeNumBetween(int a, int b){
	// int high assumes that b is highest int and low assumes that a is lowest
	// numPrimes to count for number of primes
	int low=a,high=b,numPrimes=0;

	//if a is the higher value, switch the value of high and low
	if (a>b){
		high=a;
		low=b;
	}
	// allocated int array to hold the prime numbers
	int *primeList = (int*)calloc(high-low,sizeof(int));

    // for loop to search for prime numbers and add it to array
    // and post increment numPrimes for each prime found
	for(int i = low; i <= high; i++){
		if(isPrime(i)==1){
			primeList[numPrimes++]=i;
		}
	}
	
	// output depending if there are no prime numbers, one prime numbers,
	// or multiple prime numbers
	if(numPrimes==0){
		printf("There are no prime number between %d and %d.\n",a,b);
	}
	else if (numPrimes==1){
		printf("There is only one prime number between %d and %d: %d.\n",a,b,primeList[0]);
	}
	else{
		printf("There is %d prime numbers between %d and %d: ",numPrimes,a,b);
		// this print out all the prime numbers in the array
		for(int i = 0; i<numPrimes;i++){
			// if else statment for formating at the end
			// so the last number ends with a period instead of a comma.
			if(i==(numPrimes-1)){
				printf("%d.\n",primeList[i]);
			}
			else {
				printf("%d, ",primeList[i]);
			}
		}
	}
	//deallocate the array.
	free(primeList);

}

int main(int argc,char* argv[]){
	// data validation checks if there are no arguments or insufficient arguments
	if (argc<2){
		printf("%s launched with no arguments.\n Proper usage: prog m [n]\n",argv[0]);
	}

	// else if there is 2 total arguments run the firstNprime function
	else if (argc==2){
		int num;
		sscanf(argv[1],"%d",&num);
		//if arugment is not a positive integer print error message
		// otherwise run firstNPrime
		if (num < 1)
			printf("%s's argument is not a strictly positive interger.\n",argv[0]);
		else
			firstNPrime(num);
	}
	// else if there a 3 arguments run the primeNumBetween function
	else if (argc==3){
		// to convert the 2nd and 3rd argument from string to int
		int a,b;
		sscanf(argv[1],"%d",&a);
		sscanf(argv[2],"%d",&b);

	    //checks if a and b is positive if not print the error message
		if (a<1){
			printf("%s's first argument is not a strictly positive integer.\n", argv[0]);
		}
		if (b<1){
			printf("%s's second argument is not a strictly positive integer.\n",argv[0]);
		}
		else 
			primeNumBetween(a,b);
	}
	// else print error message for too much arguments.
	else {
		printf("%s launched with too much arguments.\nProper usage: prog m [n]\n",argv[0]);
	}

	return 0;
}
