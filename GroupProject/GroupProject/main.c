#include <stdio.h>
#include <string.h>
#include "sqlite3.h"

char login[100], password[100];
int type;
int countOfUsers;
int findId, bFindId;

static int callback(void *NotUsed, int argc, char **argv, char **azColName){
   int i;
   for(i=0; i<argc; i++){
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}

void printIdName(){
	printf(" %-20s ", "id");
	printf("%s\n", "name");}
static int showClients(void *NotUsed, int argc, char **argv, char **azColName){
   printf(" %-20s ", argv[0] ? argv[0] : "NULL");
   printf("%s ", argv[1] ? argv[1] : "NULL");
   printf("\n");
   return 0;
}



static int findIdForNewClient(void *NotUsed, int argc, char **argv, char **azColName){
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
	sqlite3 *db;
	char str[100];
	int isTrue = 1;
	int a;
    char *zErrMsg = 0;
    int rc;
	char format[] = "SELECT * FROM Personal WHERE Login = '%s' AND Password = '%s';";
	char find[] = "SELECT Count(Personal.Login) AS MyCount FROM Personal WHERE Login = '%s';";
	char showAllAccounts[] = "SELECT BANK_ACCOUNTS.number_of_account , BANK_ACCOUNTS.type_of_account , BANK_ACCOUNTS.client_id FROM  BANK_ACCOUNTS ORDER BY BANK_ACCOUNTS.number_of_account ASC ";
	char findAccountsOfClient[] = "SELECT * FROM BANK_ACCOUNTS WHERE BANK_ACCOUNTS.client_id = '%s';";
	char findAccountsOfClientFree[] = "SELECT * FROM BANK_ACCOUNTS WHERE BANK_ACCOUNTS.client_id IS NULL;";
	char setAccountForClient[] = "UPDATE BANK_ACCOUNTS SET client_id = '%s' WHERE number_of_account = '%s' AND client_id IS NULL;";
	char deleteAccountFromClient[] = "UPDATE BANK_ACCOUNTS SET client_id = NULL WHERE number_of_account = '%s' AND client_id = '%s';";
	char showAllClients[] = "SELECT client.id , client.full_name FROM client ORDER BY client.id ASC;";
	
	
	char query[1000];
	char fname[100],lname[100], email[100], password[100], ans, idChar[100], idChar2[100];
	char accType[100];
	char date[100] , interestRate[100] , monthlyQuota[100] , perTransactionFee[100] , perDayFee[100];
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
	
	if(type == 1){
		printf("\nHello Admin!\nInput command<= ");
		while(isTrue){
			scanf("%s", str);//fgets(str, 100, stdin);
			if(!strcmp(str, "help")){
				printf("\n-----Vitaliy Efimik--------\nshowClients - show all clients\naccountsOfClient - edit relationships of accounts and clients\n ------------------------------------\n");
			}
//----Vitaliy Efimik------------------------------------------------------------------------------------------------------------------------------------
			else if(!strcmp(str,"showClients")){
				printIdName();
				rc = sqlite3_exec(db,showAllClients, showClients, 0, &zErrMsg);
			}
			else if(!strcmp(str,"accountsOfClient")){
				printf("Input id client <= ");
				scanf("%s",idChar);
				sprintf_s(query, 1000, findAccountsOfClient, idChar);
				rc = sqlite3_exec(db,query, callback, 0, &zErrMsg);
				printf( "\nadd, delete, end\n");
				while(1){
					printf("   <=");
					scanf("%s", str);
					if(!strcmp(str,"add")){
						printf("Free accoutns: \n");
						sprintf_s(query, 1000, findAccountsOfClientFree);
						rc = sqlite3_exec(db,query, callback, 0, &zErrMsg);
						printf("Input account number <= ");
						scanf("%s",idChar2);
						printf("Save this data ? (y/n)");
						scanf("%c",&ans);
						scanf("%c",&ans);
						if(ans == 'y'){
							sprintf_s(query, 1000, setAccountForClient,idChar, idChar2);
							rc = sqlite3_exec(db,query, callback, 0, &zErrMsg);
							printf("\nOK\n");
						}
						else{
							printf("\nCancel\n");
						}
					}
					else if(!strcmp(str,"delete")){
						printf("Input account number <= ");
						scanf("%s",idChar2);
						printf("Save this data ? (y/n)");
						scanf("%c",&ans);
						scanf("%c",&ans);
						if(ans == 'y'){
							sprintf_s(query, 1000, deleteAccountFromClient,idChar2, idChar);
							rc = sqlite3_exec(db,query, callback, 0, &zErrMsg);
							printf("\nOK\n");
						}
						else{
							printf("\nCancel\n");
						}
						
					}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
					else if(!strcmp(str,"end")){
						break;
					}
				}
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