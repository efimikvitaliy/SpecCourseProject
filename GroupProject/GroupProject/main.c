#include <stdio.h>
#include <string.h>
#include "sqlite3.h"


#define LIMITCHAR 20
#define COUNT 100
const char msgAddClient[] = "addClient\n";
const char msgUpdateClient[] = "updateClient\0";

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

struct Client{
	int id;
	char FirstName[LIMITCHAR];
	char SecondName[LIMITCHAR];
	char Email[COUNT];
	char Password[LIMITCHAR];
};
typedef struct Client Client;

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
static int callbackInt(void *NotUsed, int argc, char **argv, char **azColName){
	resSel = atoi(argv[0]);
	return 0;
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
	char accInsertOperation[] = "INSERT INTO BANK_ACCOUNTS (balance, account_id, client_id, type_of_account, totalTransactions) VALUES ('%s','%d', '%s', 'actual', '0');";
	char ans;

	bFindId = 1; findId = 1;
	rc = sqlite3_exec(db,accFindNewId, findIdForNewAccount, 0, &zErrMsg);
	printf("\nNew account number : %d\n", findId);
	printf("Input client code<= ");
	scanf("%s", clientCode);
	printf("Input start balance<= ");
	scanf("%s", startBalance);
	printf("Data about account:\nAccount id: %d\nclient code: '%s'\nStart balance: '%s'\nAdd new account y/n?",findId,clientCode,startBalance);
	scanf("%c",&ans);
	scanf("%c",&ans);
	if(ans == 'y'){
		sprintf_s(query, 1000, accInsertOperation, startBalance, findId,clientCode);
		rc = sqlite3_exec(db,query, callback, 0, &zErrMsg);
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
	printf("Input account id <= ");
	scanf("%s", idChar);
	sprintf_s(query, 1000, showDataAboutAccount, idChar);
	printf("Delete this account? y/n\n");
	rc = sqlite3_exec(db,query, callback, 0, &zErrMsg);
	scanf("%c",&ans);
	scanf("%c",&ans);
	if(ans == 'y'){
		sprintf_s(query, 1000, deleteAccount, idChar);
		rc = sqlite3_exec(db,query, callback, 0, &zErrMsg);
		sprintf_s(query, 1000, delCard, idChar);
		rc = sqlite3_exec(db,query, callback, 0, &zErrMsg);
		printf("\nOK\n");
	}
	else{
		printf("\nCancel\n");
	}
}

static int addClient() {
	char firstName[LIMITCHAR];
	char secondName[LIMITCHAR];
	char email[LIMITCHAR];
	char password[LIMITCHAR];
	printf(msgEnterFirstName);
	scanf("%s", firstName);
	printf(msgEnterSecondName);
	scanf("%s", secondName);
	printf(msgEnterEmail);
	scanf("%s", email);
	printf(msgEnterPassword);
	scanf("%s", password);
	char req[COUNT];
	sprintf_s(req, COUNT, "INSERT INTO CLIENT(first_name, second_name, email, password) VALUES('%s','%s','%s','%s')",
		firstName, secondName, email, password);
	rc = sqlite3_exec(db, req, NULL, NULL, &zErrMsg);
	printf("Added\n");
	return 0;
}

int updateClient(char* input) {
	int pos = strlen(msgUpdateClient);
	int delpos = strlen(input) - 1;
	char* args;
	if ((delpos >= pos) && (input[pos] == ' ')) {
		args = input + pos + 1;
		input[pos] = '\0';
		if (strcmp(input, msgUpdateClient) == 0) {
			char *arg1 = "";
			while ((*args != '\n') && (*args != ' ')) {
				args++;
			}
			*args = ' ';
			arg1 = input + pos + 1;
			struct Client *client = (Client*)malloc(sizeof(Client));
			client->id = -1;
			char *zErrMsg = 0;
			int rc;
			char req[COUNT];
			sprintf_s(req, COUNT, "select * from CLIENT WHERE id = \"%s\" ", arg1);
			rc = sqlite3_exec(db, req, callbackClient, client, &zErrMsg);
			if (rc != SQLITE_OK){
				fprintf(stderr, "SQL error: %s\n", zErrMsg);
				sqlite3_free(zErrMsg);
			}
			if (client->id == -1) {
				printf("Invalid clientId\n");
				return 0;
			}
			char email[COUNT];
			char password[LIMITCHAR];
			printf(msgEnterEmail);
			fgets(email, COUNT, stdin);
			int pos = strlen(email) - 1;
			if (email[pos] == '\n')
				email[pos] = '\0';
			strcpy_s(client->Email, COUNT, email);
			printf(msgEnterPassword);
			fgets(password, LIMITCHAR, stdin);
			pos = strlen(password) - 1;
			if (password[pos] == '\n')
				password[pos] = '\0';
			strcpy_s(client->Password, LIMITCHAR, password);
			char req1[COUNT];
			sprintf_s(req1, COUNT, "UPDATE CLIENT SET email='%s', password='%s' WHERE id='%d'",
				client->Email, client->Password, client->id);
			rc = sqlite3_exec(db, req1, NULL, NULL, &zErrMsg);
			printf("updated\n");
			free(client);			
		}
		input[pos] = ' ';
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
	strcpy(queryBalance, "UPDATE BANK_ACCOUNTS SET balance = ");
	itoa(balance, buf, 10);
	strcat(queryBalance, buf);
	strcat(queryBalance, " WHERE account_id = ");
	itoa(acc_num, buf, 10);
	strcat(queryBalance, buf);

	rc = sqlite3_exec(db, queryBalance, 0, 0, &zErrMsg);
	if( rc!=SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		return;
	}
	strcpy(queryBalance, "SELECT TotalTransactions FROM BANK_ACCOUNTS WHERE account_id = ");
	itoa(acc_num, buf, 10);
	strcat(queryBalance, buf);
	rc = sqlite3_exec(db, queryBalance, callbackInt, 0, &zErrMsg);
	if( rc!=SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
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
		sqlite3_free(zErrMsg);
		return;
	}
	printf("OK\n");
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
				printf("\naddAccount - add account\ndelAccount - delete account\n");
			}
			else if(!strcmp(str, "addAccount\n" )){
				addAccount();
			}
			else if(!strcmp(str,"delAccount\n")){
				delAccount();
			}
			else if (strcmp(str, msgAddClient) == 0){
				addClient();
			}
			else if (strncmp(str, msgUpdateClient, strlen(msgUpdateClient)) == 0) {
				updateClient(str);
			}
			else if(!strcmp(str,"exit\n")){
				isTrue = 0;
				printf("By!");
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
				
				}
			else if(!strcmp(str,"exit")){
				isTrue = 0;
				printf("By!");
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
