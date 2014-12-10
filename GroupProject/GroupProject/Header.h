

int checkCorrectId(char* arg) {
	if (!atoi(arg))
	{
		return 1;
	}
	return 0;
}


int checkAccountId(char* arg) {
	if (!atoi(arg))
		return 1;
	int a= atoi(arg);
	if (a <= 0)
		return 1;
	return 0;
}