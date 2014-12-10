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
	if (deleteClient("deleteClient asd") != 1) {
		printf("!!!!!!!!!!!!!!!!!!!!!!!!DeleteClien with correct id\n");
		ret = 0;
	}
	else {
		printf("------------------------DeleteClien with incorrect id\n");
		ret = 1;
	}
	return ret;
}

int main()
{
	int res = 0;
	res += successfullTest();
	res += testDeleteClient();

	if (res != 0){
		printf("Tests failed!");
	}
	else{
		printf("Tests succeeded!");
	}

	return res;
}