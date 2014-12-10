#include "Header.h"

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
			else if (strcmp(str, msgShowDeletedClients) == 0) {
				showDeletedClientsFromDB();
			}
			else if (strncmp(str, msgShowClient, strlen(msgShowClient)) == 0) {
				
				showClient(str);
			}
			else if(strcmp(str, msgShowAccounts) == 0){
				accountList();
			}
			else if (!strcmp(str, "setInterestRate\n")){
				setInterestRate();
			}
			else if (!strcmp(str, "setPerDayFee\n")){
				setPerDayFee();
			}
			else if (!strcmp(str, "setMonthlyQuota\n")){
				setMonthlyQuota();
			}
			else if (!strcmp(str, "setPerTransactionFee\n")){
				setPerTransactionFee();
			}
			else if (!strcmp(str, "findClientsBySecondName\n")){
				findClientsBySecondName();
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
