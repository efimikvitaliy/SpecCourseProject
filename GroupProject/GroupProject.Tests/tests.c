#include <stdio.h>
#include <string.h>

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