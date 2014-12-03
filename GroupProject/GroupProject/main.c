#include <stdio.h>
#include <string.h>
#include "sqlite3.h"


#define LIMITCHAR 20
#define COUNT 300
#define SHOWCLIENTS 10001
#define SHOWACCOUNTS 10002
#define SHOWCARDS 10003
const char msgAddClient[] = "addClient\n";
const char msgShowClients[] = "showClients\n";
const char msgDeleteClient[] = "deleteClient\0";
const char msgUpdateClient[] = "updateClient\0";
const char msgShowAccounts[] = "showAccounts\n";
const char msgShowBalance[] = "showBalance";
const char msgInsertCard[] = "addCard\0";

const char msgEnterFirstName[] = "Enter First Name: \n";
const char msgEnterSecondName[] = "Enter Second Name: \n";
const char msgEnterEmail[] = "Enter Email address: \n";
const char msgEnterPassword[] = "Enter Password: \n";


char login[100], password[100];
int type;
int countOfUsers;
int findId, bFindId;//
sqlite3 *db;
char str[100];
int isTrue = 1;
char *zErrMsg = 0;
int rc;
int resSel = 0;
int retRows = 0;


struct Client{
	int id;
	char FirstName[LIMITCHAR];
	char SecondName[LIMITCHAR];
	char Email[COUNT];
	char Password[LIMITCHAR];
};
typedef struct Client Client;

struct Config{
	double interestRate;
	int monthlyQuota;
	double perTransactionFee;
	int id;
	double perDayFee;
	int daysLimit;
	char date[COUNT];
};typedef struct Config Config;

static int callback(void *NotUsed, int argc, char **argv, char **azColName){
   int i;
   for(i=0; i<argc; i++){
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}

static int callbackClient(void *user, int argc, char **argv, char **azColName){
	int i;
	Client* client = (Client*)user;
	for (i = 0; i < argc; i++){
		if (strcmp(azColName[i], "id") == 0) {
			client->id = atoi(argv[i]);
		}
		if (strcmp(azColName[i], "first_name") == 0)
			strncpy_s(client->FirstName, LIMITCHAR, argv[i], LIMITCHAR);
		if (strcmp(azColName[i], "second_name") == 0)
			strncpy_s(client->SecondName, LIMITCHAR, argv[i], LIMITCHAR);
		if (strcmp(azColName[i], "email") == 0)
			strncpy_s(client->Email, COUNT, argv[i], COUNT);
		if (strcmp(azColName[i], "password") == 0)
			strncpy_s(client->Email, LIMITCHAR, argv[i], LIMITCHAR);
	}
	return 0;
}
static int callbackConfig(void *user, int argc, char **argv, char **azColName){
	int i;
	Config* config = (Config*)user;
	for (i = 0; i < argc; i++){
		if (strcmp(azColName[i], "id") == 0) {
			config->id = atoi(argv[i]);
		}
		if (strcmp(azColName[i], "daysLimit") == 0)
			config->daysLimit = atoi(argv[i]);
		if (strcmp(azColName[i], "date") == 0)
			strncpy_s(config->date, COUNT, argv[i], COUNT);
		if (strcmp(azColName[i], "interestRate") == 0)
			config->interestRate = atof(argv[i]);
		if (strcmp(azColName[i], "monthlyQuota") == 0)
			config->monthlyQuota = atoi(argv[i]);
		if (strcmp(azColName[i], "perDayFee") == 0)
			config->perDayFee = atof(argv[i]);
		if (strcmp(azColName[i], "perTransactionFee") == 0)
			config->perTransactionFee = atof(argv[i]);
	}
	return 0;
}
static int callbackInt(void *NotUsed, int argc, char **argv, char **azColName){
	resSel = atoi(argv[0]);
	return 0;
}
static int callbackShow(void *type, int argc, char **argv, char **azColName){
	int i, j;
	const int t = (int)(type);
	for (i = 0; i < argc; i++){
		printf(argv[i]);
		if (i != argv - 1) {
			int lim = 0;
			switch (t) {
			case SHOWCLIENTS:
				lim = LIMITCHAR;
				if (i == 0)
					lim = 7;
				break;
			case SHOWACCOUNTS:
				lim = 10;
				break;
			case SHOWCARDS:
				lim = 10;
				break;
			}
			for (j = strlen(argv[i]); j < lim; j++){
				printf(" ");
			}
		}
	}
	printf("\n");
	return 0;
}
void setInterestRate(){
	char query[500];
	char buf[50];
	struct Config *config = (Config*)malloc(sizeof(Config));
	double rate;
	char result[500];
	time_t rawtime;
	struct tm * timeinfo;
	int id = 0;
	printf("Set interest rate\n");
	printf("Enter interest rate: ");
	scanf("%lf", &rate);

	rc = sqlite3_exec(db, "SELECT MAX(id) FROM BANK_CONFIG", callbackInt, 0, &zErrMsg);
	if( rc!=SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		return;
	}
	id = resSel;
	strcpy(query, "SELECT * FROM BANK_CONFIG WHERE id=");
	itoa(id, buf, 10);
	strcat(query, buf);
	rc = sqlite3_exec(db, query, callbackConfig, config, &zErrMsg);
	if( rc!=SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		return;
	}
	++id;
	config->interestRate = rate;
	config->id = id;
	time ( &rawtime );
	timeinfo = localtime ( &rawtime );
	strcat(query,  asctime (timeinfo));
	strcpy(config->date,  asctime (timeinfo));
    sprintf_s(result, 500, "INSERT INTO BANK_CONFIG(date, daysLimit, id, interestRate, monthlyQuota, perDayFee, perTransactionFee) VALUES('%s','%d','%d','%f', '%d', '%f', '%f')",
		config->date, config->daysLimit, config->id, config->interestRate, config->monthlyQuota, config->perDayFee,config->perTransactionFee);
	rc = sqlite3_exec(db, result, 0, 0, &zErrMsg);
	if( rc!=SQLITE_OK )
	{
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		return;
	}
	else 
		printf("OK\n");
}
void setPerDayFee(){
	char query[500];
	char buf[50];
	struct Config *config = (Config*)malloc(sizeof(Config));
	double perDayFee;
	char result[500];
	time_t rawtime;
	struct tm * timeinfo;
	int id = 0;
	printf("Set per day fee\n");
	printf("Enter per day fee: ");
	scanf("%lf", &perDayFee);

	rc = sqlite3_exec(db, "SELECT MAX(id) FROM BANK_CONFIG", callbackInt, 0, &zErrMsg);
	if( rc!=SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		return;
	}
	id = resSel;
	strcpy(query, "SELECT * FROM BANK_CONFIG WHERE id=");
	itoa(id, buf, 10);
	strcat(query, buf);
	rc = sqlite3_exec(db, query, callbackConfig, config, &zErrMsg);
	if( rc!=SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		return;
	}
	++id;
	config->perDayFee = perDayFee;
	config->id = id;
	time ( &rawtime );
	timeinfo = localtime ( &rawtime );
	strcat(query,  asctime (timeinfo));
	strcpy(config->date,  asctime (timeinfo));
    sprintf_s(result, 500, "INSERT INTO BANK_CONFIG(date, daysLimit, id, interestRate, monthlyQuota, perDayFee, perTransactionFee) VALUES('%s','%d','%d','%f', '%d', '%f', '%f')",
		config->date, config->daysLimit, config->id, config->interestRate, config->monthlyQuota, config->perDayFee,config->perTransactionFee);
	rc = sqlite3_exec(db, result, 0, 0, &zErrMsg);
	if( rc!=SQLITE_OK )
	{
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		return;
	}
	else 
		printf("OK\n");
}
void setMonthlyQuota(){
	char query[500];
	char buf[50];
	struct Config *config = (Config*)malloc(sizeof(Config));
	int monthlyQuota;
	char result[500];
	time_t rawtime;
	struct tm * timeinfo;
	int id = 0;
	printf("Set monthly quota\n");
	printf("Enter monthly quota: ");
	scanf("%d", &monthlyQuota);

	rc = sqlite3_exec(db, "SELECT MAX(id) FROM BANK_CONFIG", callbackInt, 0, &zErrMsg);
	if( rc!=SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		return;
	}
	id = resSel;
	strcpy(query, "SELECT * FROM BANK_CONFIG WHERE id=");
	itoa(id, buf, 10);
	strcat(query, buf);
	rc = sqlite3_exec(db, query, callbackConfig, config, &zErrMsg);
	if( rc!=SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		return;
	}
	++id;
	config->monthlyQuota = monthlyQuota;
	config->id = id;
	time ( &rawtime );
	timeinfo = localtime ( &rawtime );
	strcat(query,  asctime (timeinfo));
	strcpy(config->date,  asctime (timeinfo));
    sprintf_s(result, 500, "INSERT INTO BANK_CONFIG(date, daysLimit, id, interestRate, monthlyQuota, perDayFee, perTransactionFee) VALUES('%s','%d','%d','%f', '%d', '%f', '%f')",
		config->date, config->daysLimit, config->id, config->interestRate, config->monthlyQuota, config->perDayFee,config->perTransactionFee);
	rc = sqlite3_exec(db, result, 0, 0, &zErrMsg);
	if( rc!=SQLITE_OK )
	{
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		return;
	}
	else 
		printf("OK\n");
}
void setPerTransactionFee(){
	char query[500];
	char buf[50];
	struct Config *config = (Config*)malloc(sizeof(Config));
	double perTransactionFee;
	char result[500];
	time_t rawtime;
	struct tm * timeinfo;
	int id = 0;
	printf("Set per transaction fee\n");
	printf("Enter per transaction fee: ");
	scanf("%lf", &perTransactionFee);

	rc = sqlite3_exec(db, "SELECT MAX(id) FROM BANK_CONFIG", callbackInt, 0, &zErrMsg);
	if( rc!=SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		return;
	}
	id = resSel;
	strcpy(query, "SELECT * FROM BANK_CONFIG WHERE id=");
	itoa(id, buf, 10);
	strcat(query, buf);
	rc = sqlite3_exec(db, query, callbackConfig, config, &zErrMsg);
	if( rc!=SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		return;
	}
	++id;
	config->perTransactionFee = perTransactionFee;
	config->id = id;
	time ( &rawtime );
	timeinfo = localtime ( &rawtime );
	strcat(query,  asctime (timeinfo));
	strcpy(config->date,  asctime (timeinfo));
    sprintf_s(result, 500, "INSERT INTO BANK_CONFIG(date, daysLimit, id, interestRate, monthlyQuota, perDayFee, perTransactionFee) VALUES('%s','%d','%d','%f', '%d', '%f', '%f')",
		config->date, config->daysLimit, config->id, config->interestRate, config->monthlyQuota, config->perDayFee,config->perTransactionFee);
	rc = sqlite3_exec(db, result, 0, 0, &zErrMsg);
	if( rc!=SQLITE_OK )
	{
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		return;
	}
	else 
		printf("OK\n");
}
void findClientsBySecondName()
{
	char query[500];
	char secondName[100];
	printf("Find clients by second name\n");
	printf("Enter second name of the client: ");
	scanf("%s", &secondName);
	strcpy(query, "SELECT * FROM CLIENT WHERE second_name='");
	strcat(query, secondName);
	strcat(query, "'");
	rc = sqlite3_exec(db, query, callback, 0, &zErrMsg);
	if( rc!=SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		return;
	}
	else
		printf("OK\n");
}
static int findIdForNewAccount(void *NotUsed, int argc, char **argv, char **azColName){//
   int i;
   if(bFindId)
	   findId++;
   for(i=0; i<argc; i++){
      if(bFindId && findId-1!=atoi(argv[i])){
		  --findId;
		  bFindId = 0;
	  }
   }
   i++;
   return 0;
}

static int addAccount(){
	char query[1000];
	char clientCode[100], startBalance[100];
	char accFindNewId[] = "SELECT BANK_ACCOUNTS.account_id FROM BANK_ACCOUNTS ORDER BY BANK_ACCOUNTS.account_id ASC;";
	char accInsertOperation[] = "INSERT INTO BANK_ACCOUNTS (balance, client_id, type_of_account, totalTransactions) VALUES ('%s', '%s', 'actual', '0');";
	char ans;

	bFindId = 1; findId = 1;
	printf("Input client code<= ");
	scanf("%s", clientCode);
	printf("Input start balance<= ");
	scanf("%s", startBalance);
	printf("Data about account:\nclient code: '%s'\nStart balance: '%s'\nAdd new account y/n?",clientCode,startBalance);
	scanf("%c",&ans);
	scanf("%c",&ans);
	if(ans == 'y'){
		sprintf_s(query, 1000, accInsertOperation, startBalance, clientCode);
		rc = sqlite3_exec(db,query, callback, 0, &zErrMsg);
		if (rc != SQLITE_OK){
			fprintf(stderr, "SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
		}
		printf("\nOK\n");
		}
	else{
		printf("\nCancel\n");
	}
}



static int delAccount(){
	char query[1000], idChar[100], ans;
	char showDataAboutAccount[] = "SELECT * FROM BANK_ACCOUNTS WHERE BANK_ACCOUNTS.account_id = '%s';";
	char deleteAccount[] = "DELETE FROM BANK_ACCOUNTS WHERE BANK_ACCOUNTS.account_id = '%s';";
	char delCard[] = "DELETE FROM Card WHERE Card.accNum = '%s';";
	char insertClientIntoHistory[] = "INSERT INTO CLIENT_DEL SELECT * FROM CLIENT WHERE CLIENT.id IN (SELECT client_id FROM BANK_ACCOUNTS WHERE BANK_ACCOUNTS.account_id = '%s');";
	char insertCardsIntoHistory[] = "INSERT INTO CARD_DEL SELECT * FROM CARD WHERE CARD.accNum = '%s';"; //id, pass, accNum, csv, expireDate, isDeleted, isLocked
	char insertTransactionsIntoHistory[] = "INSERT INTO TRANSACTION_DEL SELECT * FROM \"TRANSACTION\" WHERE Account_number = '%s';";
	char insertAccountIntoHistory[] = "INSERT INTO BANK_ACCOUNTS_DEL SELECT * FROM BANK_ACCOUNTS WHERE BANK_ACCOUNTS.account_id = '%s';";
	char updateAccountIntoHistory[] = "UPDATE BANK_ACCOUNTS_DEL SET isDeleted = '1' WHERE BANK_ACCOUNTS_DEL.account_id = '%s';";
	char updateCardsIntoHistory[] = "UPDATE CARD_DEL SET isDeleted = '1' WHERE CARD_DEL.accNum = '%s';";
	printf("Input account id <= ");
	scanf("%s", idChar);
	sprintf_s(query, 1000, showDataAboutAccount, idChar);
	printf("Delete this account? y/n\n");
	rc = sqlite3_exec(db,query, callback, 0, &zErrMsg);
	scanf("%c",&ans);
	scanf("%c",&ans);
	if(ans == 'y'){
		sprintf_s(query,1000,insertClientIntoHistory, idChar);
		rc = sqlite3_exec(db,query, callback, 0, &zErrMsg);
		sprintf_s(query,1000,insertAccountIntoHistory, idChar);
		rc = sqlite3_exec(db,query, callback, 0, &zErrMsg);
		sprintf_s(query,1000,insertTransactionsIntoHistory, idChar);
		rc = sqlite3_exec(db,query, callback, 0, &zErrMsg);
		sprintf_s(query,1000,insertCardsIntoHistory, idChar);
		rc = sqlite3_exec(db,query, callback, 0, &zErrMsg);
		sprintf_s(query, 1000, deleteAccount, idChar);
		rc = sqlite3_exec(db,query, callback, 0, &zErrMsg);
		sprintf_s(query,1000,updateAccountIntoHistory, idChar);
		rc = sqlite3_exec(db,query, callback, 0, &zErrMsg);
		sprintf_s(query,1000,updateCardsIntoHistory, idChar);
		rc = sqlite3_exec(db,query, callback, 0, &zErrMsg);
		printf("\nOK\n");
	}
	else{
		printf("\nCancel\n");
	}
}

static int lockCard() {
	char query[1000], idChar[100];
	char lock[] = "UPDATE CARD SET isLocked = '1' WHERE id = '%s';";
	printf("Input card id <= ");
	scanf("%s", idChar);
	sprintf_s(query,1000,lock, idChar);
	rc = sqlite3_exec(db,query, callback, 0, &zErrMsg);
	printf("\nOK\n");
}

static int unlockCard() {
	char query[1000], idChar[100];
	char lock[] = "UPDATE CARD SET isLocked = '0' WHERE id = '%s';";
	printf("Input card id <= ");
	scanf("%s", idChar);
	sprintf_s(query,1000,lock, idChar);
	rc = sqlite3_exec(db,query, callback, 0, &zErrMsg);
	printf("\nOK\n");
}

int addClient() {
	char firstName[LIMITCHAR];
	char secondName[LIMITCHAR];
	char email[LIMITCHAR];
	char password[LIMITCHAR];
	char req[COUNT];
	printf(msgEnterFirstName);
	scanf("%s", firstName);
	printf(msgEnterSecondName);
	scanf("%s", secondName);
	printf(msgEnterEmail);
	scanf("%s", email);
	printf(msgEnterPassword);
	scanf("%s", password);
	sprintf_s(req, COUNT, "INSERT INTO CLIENT(first_name, second_name, email, password) VALUES('%s','%s','%s','%s')",
		firstName, secondName, email, password);
	rc = sqlite3_exec(db, req, NULL, NULL, &zErrMsg);
	printf("Added\n");
	return 0;
}
int showClientsFromDB(){
	
	char *zErrMsg = 0;
	int rc;
	printf("-id-   -FirstName-         -SecondName-        -Email-        -Password- \n");
	rc = sqlite3_exec(db, "SELECT * FROM CLIENT", callbackShow, SHOWCLIENTS, &zErrMsg);
	if (rc != SQLITE_OK){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	return 0;
}

int deleteClient(char* input) {
	struct Client *client = (Client*)malloc(sizeof(Client));
	int pos = strlen(msgUpdateClient), id;
	char* arg = (input + pos + 1);
	char req[COUNT];
	char query[COUNT], idChar[100], ans;
	char deleteClient[] = "DELETE FROM CLIENT WHERE CLIENT.id = '%d';";
	char insertClientIntoHistory[] = "INSERT INTO CLIENT_DEL SELECT * FROM CLIENT WHERE CLIENT.id = '%d';";
	char insertAccountIntoHistory[] = "INSERT INTO BANK_ACCOUNTS_DEL SELECT * FROM BANK_ACCOUNTS WHERE BANK_ACCOUNTS.client_id = '%d';";
	char insertCardsIntoHistory[] = "INSERT INTO CARD_DEL SELECT * FROM CARD WHERE CARD.accNum IN (SELECT account_id FROM BANK_ACCOUNTS WHERE BANK_ACCOUNTS.client_id = '%d')";
	char insertTransactionsIntoHistory[] = "INSERT INTO TRANSACTION_DEL SELECT * FROM \"TRANSACTION\" WHERE Account_number IN (SELECT account_id FROM BANK_ACCOUNTS WHERE BANK_ACCOUNTS.client_id = '%d');";
	char updateClientIntoHistory[] = "UPDATE CLIENT_DEL SET isDeleted = '1' WHERE CLIENT_DEL.id = '%d';";
	char updateAccountIntoHistory[] = "UPDATE BANK_ACCOUNTS_DEL SET isDeleted = '1' WHERE BANK_ACCOUNTS_DEL.client_id = '%d';";
	char updateCardsIntoHistory[] = "UPDATE CARD_DEL SET isDeleted = '1' WHERE CARD.accNum IN(SELECT account_id FROM BANK_ACCOUNTS WHERE BANK_ACCOUNTS.client_id = '%d')";
		
	input[strlen(input) - 1] = '\0';
	if (!atoi(arg))
	{
		printf("Incorrect id\n");
		return -1;
	}
	id = atoi(arg);
	client->id = -1;
	sprintf_s(req, COUNT, "select * from CLIENT WHERE id = '%d' ", id);
	rc = sqlite3_exec(db, req, callbackClient, client, &zErrMsg);
	if (rc != SQLITE_OK){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	if (client->id == -1) {
		printf("Invalid clientId\n");
		return -1;
	}
	sprintf_s(query, COUNT, insertClientIntoHistory, client->id);
	rc = sqlite3_exec(db, query, callback, 0, &zErrMsg);
	sprintf_s(query, COUNT, insertAccountIntoHistory, client->id);
	rc = sqlite3_exec(db, query, callback, 0, &zErrMsg);
	sprintf_s(query, COUNT, insertCardsIntoHistory, client->id);
	rc = sqlite3_exec(db, query, callback, 0, &zErrMsg);
	sprintf_s(query, COUNT, insertTransactionsIntoHistory, client->id);
	rc = sqlite3_exec(db, query, callback, 0, &zErrMsg);
	sprintf_s(query, COUNT, updateClientIntoHistory, client->id);
	rc = sqlite3_exec(db, query, callback, 0, &zErrMsg);
	sprintf_s(query, COUNT, updateAccountIntoHistory, client->id);
	rc = sqlite3_exec(db, query, callback, 0, &zErrMsg);
	sprintf_s(query, COUNT, updateCardsIntoHistory, client->id);
	rc = sqlite3_exec(db, query, callback, 0, &zErrMsg);
	sprintf_s(query, COUNT, deleteClient, client->id);
	rc = sqlite3_exec(db, query, callback, 0, &zErrMsg);
	printf("\nDeleted\n");
	free(client);
	return 0;
}
int deleteClientFromDB(char* id){
	char *zErrMsg = 0;
	int rc;
	char req[COUNT];

	sprintf_s(req, COUNT, "DELETE FROM CLIENT WHERE id = '%s'",
			id);
	rc = sqlite3_exec(db, req, NULL, NULL, &zErrMsg);
	if (rc != SQLITE_OK){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else {
		printf("Client deleted \n");
	}

	return 0;
}

int updateClient(char* input) {
	struct Client *client = (Client*)malloc(sizeof(Client));
	int pos = strlen(msgUpdateClient), id;
	char* arg = (input + pos + 1);
	char req[COUNT];
	char req1[COUNT];
	char email[COUNT];
	char password[LIMITCHAR];
	input[strlen(input) - 1] = '\0';
	if (!atoi(arg))
	{
		printf("Incorrect id\n");
		return -1;
	}
	id = atoi(arg);
	client->id = -1;
			
	sprintf_s(req, COUNT, "select * from CLIENT WHERE id = '%d' ", id);
	rc = sqlite3_exec(db, req, callbackClient, client, &zErrMsg);
	if (rc != SQLITE_OK){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	if (client->id == -1) {
	     printf("Invalid clientId\n");
		 return -1;
	}
	printf("CLient info : \n First Name : %s \n Second Name : %s \n Password : %s \n Email : %s \n",
		client->FirstName, client->SecondName, client->Password, client->Email);
	printf(msgEnterEmail);
	fgets(email, COUNT, stdin);
	pos = strlen(email) - 1;
	if (email[pos] == '\n')
		email[pos] = '\0';
	if (strcmp(email,"") != 0)
		strcpy_s(client->Email, COUNT, email);
	printf(msgEnterPassword);
	fgets(password, LIMITCHAR, stdin);
	pos = strlen(password) - 1;
	if (password[pos] == '\n')
		password[pos] = '\0';
	if (strcmp(password,"") != 0)
	    strcpy_s(client->Password, LIMITCHAR, password);
		
	sprintf_s(req1, COUNT, "UPDATE CLIENT SET email='%s', password='%s' WHERE id='%d'",
		client->Email, client->Password, client->id);
	rc = sqlite3_exec(db, req1, NULL, NULL, &zErrMsg);
	printf("updated\n");
	free(client);
	return 0;
}

int showAccounts() {
	char query[1000];
	char clientID[100];
	char showAccountsTemplate[] = "SELECT * FROM BANK_ACCOUNTS WHERE BANK_ACCOUNTS.client_id = '%s';";
	printf("Enter client identification number:");
	scanf("%s", clientID);
	sprintf_s(query, 1000, showAccountsTemplate, clientID);
	rc = sqlite3_exec(db, query, callback, 0, &zErrMsg);

	return 0;
}

int showBalance(char *updateBalanceTemplate, char *showBalanceTemplate) {
	char query[1000], ID[100];
	char getViewBalanceFeeQuery[] = "SELECT BANK_CONFIG.viewBalanceFee FROM BANK_CONFIG";
	int viewBalanceFee;
	int balance;
	scanf("%s", ID);
	sprintf_s(query, 1000, showBalanceTemplate, ID);
	rc = sqlite3_exec(db, query, callbackInt, 0, &zErrMsg);
	balance = resSel;
	rc = sqlite3_exec(db, getViewBalanceFeeQuery, callbackInt, 0, &zErrMsg);
	viewBalanceFee = resSel;
	balance -= viewBalanceFee;
	if (balance < 0){ balance = 0; }
	sprintf_s(query, 1000, updateBalanceTemplate, balance, ID);
	rc = sqlite3_exec(db, query, NULL, 0, &zErrMsg);
	sprintf_s(query, 1000, showBalanceTemplate, ID);
	rc = sqlite3_exec(db, query, callback, 0, &zErrMsg);
	return 0;
}

int showBalanceCalled() {

	char showAccountBalanceTemplate[] = "SELECT BANK_ACCOUNTS.balance FROM BANK_ACCOUNTS WHERE BANK_ACCOUNTS.account_id = '%s';";
	char updateAccountBalanceTemplate[] = "UPDATE BANK_ACCOUNTS SET balance = '%d' WHERE BANK_ACCOUNTS.account_id = '%s';";
	char showCardBalanceTemplate[] = "SELECT BANK_ACCOUNTS.balance FROM BANK_ACCOUNTS INNER JOIN CARD ON BANK_ACCOUNTS.account_id = CARD.accNum WHERE CARD.id = '%s';";
	char updateCardBalanceTemplate[] = "UPDATE BANK_ACCOUNTS SET balance = '%d' WHERE BANK_ACCOUNTS.account_id = (SELECT CARD.accNum FROM CARD WHERE CARD.id = '%s');";
	char balanceType[100];
	printf("Enter whether you want to see balance of a card or account (C/A):");
	
	scanf("%s", &balanceType);
	if (balanceType[0] == 'C' || balanceType[0] == 'c'){
		printf("Enter card number:");
		showBalance(updateCardBalanceTemplate, showCardBalanceTemplate);
	}
	else if (balanceType[0] == 'A' || balanceType[0] == 'a'){
		printf("Enter account number:");
		showBalance(updateAccountBalanceTemplate, showAccountBalanceTemplate);
	}
	else{
		printf("Invalid balance type.\n");
	}

	return 0;
}

static int getTypeOfUser(void *NotUsed, int argc, char **argv, char **azColName){
    if( 0 < argc ){
      type = atoi(argv[2]);
	}
    printf("\n");
    return 0;
}

static int getCountOfUsersWithLogin(void *NotUsed, int argc, char **argv, char **azColName){
	countOfUsers = atoi(argv[0]);
	return 0;
}

void getAddMoney(int ot){
	char queryBalance[500];
	char buf[50];
	int acc_num = 0;
	int add_bal = 0;
	int balance = 0;
	int type = 0;
	time_t rawtime;
	struct tm * timeinfo;
	if(ot == 1){
		printf("Get money\n");
	}
	else if(ot == 2){
		printf("Add money\n");
	}
	printf("Choose type: 1- card, 2- account\n");
	scanf("%d", &type);

	if(type == 1){
		printf("Card number:\n");
		scanf("%s", &buf);
		strcpy(queryBalance, "SELECT accNum FROM CARD WHERE id = ");
		strcat(queryBalance, buf);

		rc = sqlite3_exec(db, queryBalance, callbackInt, 0, &zErrMsg);
		if( rc!=SQLITE_OK ){
			fprintf(stderr, "SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
			return;
		}
		acc_num = resSel;
	}
	else if(type == 2){
		printf("Account number:\n");
		scanf("%d", &acc_num);
	}
	else{
		printf("Incorrect type\n");
		return;
	}
	strcpy(queryBalance, "SELECT balance FROM BANK_ACCOUNTS WHERE account_id =");
	itoa(acc_num, buf, 10);

	strcat(queryBalance, buf);
	rc = sqlite3_exec(db, queryBalance, callbackInt, 0, &zErrMsg);
	if( rc!=SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		return;
	}
	balance = resSel;
	printf("Sum:\n");
	scanf("%d", &add_bal);
	if(ot == 1){
		if(balance < add_bal){
			printf("This is so much\n");
			return;
		}
		balance -= add_bal;
	}
	else if(ot == 2){
		balance += add_bal;
	}
	rc = sqlite3_exec(db, "BEGIN", NULL, 0, &zErrMsg);
	if( rc!=SQLITE_OK ){
			fprintf(stderr, "SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
			return;
	}
	strcpy(queryBalance, "UPDATE BANK_ACCOUNTS SET balance = ");
	itoa(balance, buf, 10);
	strcat(queryBalance, buf);
	strcat(queryBalance, " WHERE account_id = ");
	itoa(acc_num, buf, 10);
	strcat(queryBalance, buf);

	rc = sqlite3_exec(db, queryBalance, 0, 0, &zErrMsg);
	if( rc!=SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_exec(db, "ROLLBACK", 0, 0, &zErrMsg);
		sqlite3_free(zErrMsg);
		return;
	}
	strcpy(queryBalance, "SELECT TotalTransactions FROM BANK_ACCOUNTS WHERE account_id = ");
	itoa(acc_num, buf, 10);
	strcat(queryBalance, buf);
	rc = sqlite3_exec(db, queryBalance, callbackInt, 0, &zErrMsg);
	if( rc!=SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_exec(db, "ROLLBACK", 0, 0, &zErrMsg);
		sqlite3_free(zErrMsg);
		return;
	}
	resSel++;
	strcpy(queryBalance, "UPDATE BANK_ACCOUNTS SET totalTransactions = ");
	itoa(resSel, buf, 10);
	strcat(queryBalance, buf);
	strcat(queryBalance, " WHERE account_id = ");
	itoa(acc_num, buf, 10);
	strcat(queryBalance, buf);
	rc = sqlite3_exec(db, queryBalance, 0, 0, &zErrMsg);
	if( rc!=SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_exec(db, "ROLLBACK", 0, 0, &zErrMsg);
		sqlite3_free(zErrMsg);
		return;
	}
	sqlite3_exec(db, "COMMIT", 0, 0, &zErrMsg);
	printf("OK\n");
}
int accountList(){
	char query[1000] = "SELECT * FROM BANK_ACCOUNTS;";
	rc = sqlite3_exec(db, query, callback, 0, &zErrMsg);
	if( rc!=SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		return;
	}
	return 0;
}


static int bankAccCallback(void *NotUsed, int argc, char **argv, char **azColName){
	retRows = argc;
	return 0;
}


int createCard() {
	int accaunt_number, rc, card_number, card_csv;
	char *zErrMsg = 0;
	char query[1000], expire_date[20],card_pin[50];
	char acc_query[] = "SELECT * FROM BANK_ACCOUNTS WHERE account_id=%d;";
	char card_insert[] = "INSERT INTO CARD (id, pass, accNum, csv, expireDate,isDeleted,isLocked) VALUES (%d,'%s',%d,%d,'%s',0,0);";
	printf("Adding card....\n");
	printf("Please enter accaunt number for this card:\n");
	
	do {
		retRows = 0;
		scanf("%d", &accaunt_number);

		if (accaunt_number == 0) return;
		sprintf_s(query, 1000, acc_query, accaunt_number);
		rc = sqlite3_exec(db, query, bankAccCallback, 0, &zErrMsg);
		if (rc != SQLITE_OK) {
			fprintf(stderr, "SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
		}
		if (retRows != 0) {
			break;
		}
		else {
			printf("Sorry, but accaunt with this number does not exist.\n");
			printf("Reenter number or type 0 for exit form this operation.\n");
		}
	} while (1);

	printf("Enter card number for this card:\n");
	scanf("%d", &card_number);
	printf("Enter card csv:\n");
	scanf("%d", &card_csv);
	printf("Enter card expire date (ex. 8/16):\n");
	scanf("%s", &expire_date);
	printf("Enter card pin:\n");
	scanf("%s", &card_pin);
	printf("Creating card....\n");
	sprintf_s(query, 1000, card_insert, card_number, card_pin, accaunt_number, card_csv, expire_date);
	rc = sqlite3_exec(db, query, 0, 0, &zErrMsg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	printf("Card added.\n");
	return 0;
}

int main()
{
	int a;
	char query[1000];
	char format[] = "SELECT * FROM Personal WHERE Login = '%s' AND Password = '%s';";
	char find[] = "SELECT Count(Personal.Login) AS MyCount FROM Personal WHERE Login = '%s';";
	type = 0;
	countOfUsers = 0;

	rc = sqlite3_open("Database2.db", &db);
	sqlite3_exec(db, "PRAGMA foreign_keys = ON", NULL, NULL, NULL);
	
	if( rc ){	
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      sqlite3_close(db);
      return(1);
	}

	printf("This is Personal Application\n");
	while(!type){
		while(!countOfUsers){
			printf("Login: ");
			scanf("%s", login);
			sprintf_s(query, 100, find, login);
			rc = sqlite3_exec(db,query,getCountOfUsersWithLogin, 0 ,&zErrMsg);
			if(!countOfUsers)
				printf("No Such User. Enter Login Again\n");
		}
		printf("password: ");
		scanf("%s", password);
		sprintf_s(query,100, format , login, password);
		
		rc = sqlite3_exec(db,query, getTypeOfUser, 0, &zErrMsg);
	
		if( rc!=SQLITE_OK ){
		  fprintf(stderr, "SQL error: %s\n", zErrMsg);
		  sqlite3_free(zErrMsg);
		}
		if(!type)
			printf("Incorrect password\n");
	}
	fgets(str, 100, stdin);
	if(type == 1){
		printf("\nHello Admin!\nInput command<= ");
		while(isTrue){
			//scanf("%s", str);
			fgets(str, 100, stdin);
			if(!strcmp(str, "help\n")){
				printf("\naddClient - add client\nupdateClient client_id - update client\n");
				printf("\naddAccount - add account\ndelAccount - delete account\nlockCard - lock card\nunlockCard - unlock card\n");
				printf("\naddCard - add new card to some account\n");
			}
			else if(!strcmp(str, "addAccount\n" )){
				addAccount();
			}
			else if(!strcmp(str,"delAccount\n")){
				delAccount();
			}
			else if(!strcmp(str,"lockCard\n")){
				lockCard();
			}
			else if(!strcmp(str,"unlockCard\n")){
				unlockCard();
			}
			else if (strcmp(str, msgAddClient) == 0){
				addClient();
			}
			else if (strcmp(str, msgShowClients) == 0) {
				showClientsFromDB();
			}
			else if(strcmp(str, msgShowAccounts) == 0){
				accountList();
			}
			else if (strncmp(str, msgDeleteClient, strlen(msgDeleteClient)) == 0) {
				deleteClient(str);
			}
			else if (strncmp(str, msgUpdateClient, strlen(msgUpdateClient)) == 0) {
				updateClient(str);
			}
			else if (strncmp(str, msgInsertCard, strlen(msgInsertCard)) == 0) {
				createCard();
			}
			else if(!strcmp(str,"exit\n")){
				isTrue = 0;
				printf("Bye!");
				break;
			}
			else{
				fgets(str, 100, stdin);
				printf("Unknown command\n");
			}
			printf("Input command<= ");
		}
	}
	else if(type == 2){
		printf("\nHello Operationist!\nInput command<= ");
		while(isTrue){
			scanf("%s", str);//fgets(str, 100, stdin);
			if(!strcmp(str,"help")){
				printf("\nshowAccounts - show accounts of the client\nshowBalance - show balance of the account or card\n");
				}
			else if (!strcmp(str, msgShowAccounts)){
				showAccounts();
			}
			else if (!strcmp(str, msgShowBalance)){
				showBalanceCalled();
			}
			else if(!strcmp(str,"exit")){
				isTrue = 0;
				printf("Bye!");
				break;
			}
			else if(!strcmp(str,"get_money")){
				getAddMoney(1);
			}
			else if(!strcmp(str,"add_money")){
				getAddMoney(2);
			}
			else{
				fgets(str, 100, stdin);
				printf("Unknown command\n");
			}
			printf("Input command<= ");
		}
	}
	sqlite3_close(db);
	return 0;
}
