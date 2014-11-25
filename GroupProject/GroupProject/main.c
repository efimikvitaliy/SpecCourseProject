#include <stdio.h>
#include <string.h>
#include "sqlite3.h"

char login[100], password[100];
int type;
int countOfUsers;


static int callback(void *NotUsed, int argc, char **argv, char **azColName){
   int i;
   for(i=0; i<argc; i++){
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
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
	
	if(type == 1){
		printf("\nHello Admin!\nInput command<= ");
		while(isTrue){
			scanf("%s", str);//fgets(str, 100, stdin);
			if(!strcmp(str, "help")){
				printf("\n\n");
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