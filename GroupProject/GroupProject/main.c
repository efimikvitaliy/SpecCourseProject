#include <stdio.h>
#include <string.h>
#include "sqlite3.h"

char login[100], password[100];
int type;
int countOfUsers;
int findId, bFindId;//
sqlite3 *db;
char str[100];
int isTrue = 1;
char *zErrMsg = 0;
int rc;

static int callback(void *NotUsed, int argc, char **argv, char **azColName){
   int i;
   for(i=0; i<argc; i++){
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
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
		printf("\nOK\n");
	}
	else{
		printf("\nCancel\n");
	}
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
	
	if(type == 1){
		printf("\nHello Admin!\nInput command<= ");
		while(isTrue){
			scanf("%s", str);//fgets(str, 100, stdin);
			if(!strcmp(str, "help")){
				printf("\naddAccount - add account\ndelAccount - delete account\n");
			}
			else if(!strcmp(str, "addAccount" )){
				addAccount();
			}
			else if(!strcmp(str,"delAccount")){
				delAccount();
			}
			else if(!strcmp(str,"exit")){
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
