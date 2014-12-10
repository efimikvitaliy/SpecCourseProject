#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include "../GroupProject/Header.h"



int successfullTest(){
	return 0;
}

int failureTest(){
	return 1;
}


int testDeleteClient() {
	int ret = 0;
	if (checkCorrectId("asd") == 1) {
		printf("Test 1 DeleteClient OK\n");
	}
	else {
		printf("Test 1 DeleteClient FAILED\n");
		ret = 1;
	}
	if (checkCorrectId("1") == 0) {
		printf("Test 2 DeleteClient OK\n");
	}
	else {
		printf("Test 2 DeleteClient FAILED\n");
		ret = 1;
	}
	return ret;
}

int testDelAccount() {
	int ret = 0;
	if (checkAccountId("q") == 1) {
		printf("Test 1 of delAccount OK\n");
		
	}
	else {

		printf("Test 1 of delAccount FAILED\n"); 
		ret = 1;
		
	}
	if (checkAccountId("1") == 0) {
		printf("Test 2 of delAccount OK\n");
		
	}
	else {

		printf("Test 2 of delAccount FAILED\n");
		ret = 1;
	}if (checkAccountId("-1") == 1) {
		printf("Test 3 of delAccount OK\n");
		
	}
	else {

		printf("Test 3 of delAccount FAILED\n");
		ret = 1;
	}
	return ret;
}


int main()
{
	int res = 0;
	res += successfullTest();
	res += testDeleteClient();
	res += testDelAccount();

	if (res != 0){
		printf("Tests failed!");
	}
	else{
		printf("Tests succeeded!");
	}
	
	return res;
}