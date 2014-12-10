#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
//#include "../GroupProject/main.c"



int successfullTest(){
	return 0;
}

int failureTest(){
	return 1;
}

int main()
{
	int res = 0;
	res += successfullTest();
	res += failureTest();

	if (res != 0){
		printf("Tests failed!");
	}
	else{
		printf("Tests succeeded!");
	}

	return res;
}