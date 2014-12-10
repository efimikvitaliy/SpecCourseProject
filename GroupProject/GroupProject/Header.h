

int checkCorrectId(char* arg) {
	if (!atoi(arg))
	{
		return 1;
	}
	return 0;
}


int checkAccountId(char* arg) {
	int a;

	if (!atoi(arg))
		return 1;
	a= atoi(arg);
	if (a <= 0)
		return 1;
	return 0;
}