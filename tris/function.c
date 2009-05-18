#include	"lib/basic.h"
#include 	"users.c"

/* costanti protocollo tris */
#define HELO						0				/* authentication message */
#define SROK						1				/* authentication ok */
#define ERRR						2				/* generic error*/
#define LIST						3				/* request list of available users */
#define PUSH						4				/* messaggio di invio coordinate giuoco */
#define RQOK						5				/* richiesta ok */
#define	QUIT						6				/* richiesta chiusura */
#define RLIS						7				/* invia elenco degli utenti disponibili*/
#define RUSR						8				/* scelta del giocatore */
#define USOK						9				/* user choice ok */
/* Error code
** 330 -> request play against himself  
** 440 -> nickname already exixts
** 550 -> request user id doesn't exixt
*/
/* funzioni principali per il gioco del tris
 * Parte di autenticazione
 * Parte del parsing del messaggio (comune client e server)
 */

char* fgetsn (char * str){
  char* p;
  //Devo eliminare lo \n, è importantissimo, altrimenti si creerebbe \n\r\n 
  //la readline lato server, si fermerebbe al 1° \n e si metterebbe in attesa 
  //per \r\n, crando confusione...

  p=fgets(str,MAXLINE,stdin);
  str[strlen(str)-1]='\0';
  fflush (stdout);
  fflush(stdin);
  return p;
}

/*Splitta una stringa in n parti e ne ritorna il numero*/
int split_string(char* input,char sep,char **out)
{
  int i=0;

  char* p=NULL;
  char* ss=NULL;
	// Bisogna passare i parametri senza /0 altrimenti ci sono problemi nel parsing
  for(p = strtok_r(input,&sep,&ss),i=0;p!=NULL;p=strtok_r(NULL,&sep,&ss),i++)
  {
    out[i]=(char*)malloc(MAXLINE);
    memset (out[i],'\0',MAXLINE);
    sprintf(out[i],"%s",p);
  }
  return i;
}

/***************************** 
			Server function
******************************/

int serverAuth( int sockfd, int ret )
{
	char message[MAXLINE], type[5];
	
	/* type identifica il tipo di messaggio da inviare. In questo caso
	 * e' uguale a zero -> autenticazione */
	if (ret == SROK)
 		sprintf(type,"SROK\0");
	else
		sprintf(type,"ERRR\0");
	
	memset(message, '\0', MAXLINE);
	snprintf(message, sizeof(message), "%s 440 NULL NULL\r\n", type);

	Write(sockfd, message, strlen(message));
	return 0;
}



/* ritorna il codice del messaggio passato come argomento 
 * il tipo del messaggio e' dato dal primo carattere (numerico) */
messageType(char *message)
{
	return(atoi(&message[0]));
}

int parseMessage(char *buff, char **param)
{
	
	if(split_string(buff, 32, param) <= 4){
		//printf("split_string return value: %s\n", param[0]);
		if (strcmp(param[0], "HELO") == 0) return HELO;
		if (strcmp(param[0], "SROK") == 0) return SROK;
		if (strcmp(param[0], "ERRR") == 0) return ERRR; 
		if (strcmp(param[0], "LIST") == 0) return LIST; 
		if (strcmp(param[0], "RLIS") == 0) return RLIS; 
		if (strcmp(param[0], "RQOK") == 0) return RQOK;
		if (strcmp(param[0], "RUSR") == 0) return RUSR;
		if (strcmp(param[0], "USOK") == 0) return USOK;
		if (strcmp(param[0], "QUIT") == 0) return QUIT;
	}
	else 
		return -1;
}


/***************************** 
		Client & server function
******************************/

/* server message management 
** sockfd: client socket file descriptor */
int messagemng(char* buff, t_user* players, int sockfd)
{
	int n,k;
	char* param [10];
	char* list[FD_SETSIZE];
	memset (list, '\0', FD_SETSIZE);
	if( (n = parseMessage(buff, param)) >= 0 ){
		printf("\nmessage type: %d\n", n);
		switch (n){
			case HELO:
				if  ((k = addUser(param[1],players,sockfd)) == 0) 
					serverAuth( sockfd, SROK );
				else 	serverAuth( sockfd, ERRR );
			break;
			case LIST:
				/* prepare and send the list of all available users */
				prepareList(players, list, sockfd);
			break;
			case RQOK:
			break;
			case RUSR:
				printf("RUSR request received\n");
				/* control the user request, param[1] contains the user ID */
				controlUser(players, sockfd, param[1]);
			break;
			case QUIT:
			break;	
		}		

	}
	return (0);
}

/***************************** 
			Client function
******************************/

/* funzione per l'autenticazione del giocatore 
** sockfd: server socket file descriptor */
int auth(int sockfd)
{
	char username[MAXLINE], message[MAXLINE];
	/* type identifica il tipo di messaggio da inviare. In questo caso
	 * e' uguale a zero -> autenticazione */
	int type = HELO; 
	memset(username, '\0', MAXLINE);
	memset(message, '\0', MAXLINE);
	printf("\nPlease insert your username: ");
	fgetsn(username);
	snprintf(message, sizeof(message), "HELO %s NULL NULL\r\n", username);
	/* spedisce la stringa al server */
	Write(sockfd, message, strlen(message));
	return 0;
}

/* funzione che serve al client per richiedere la lista degli utenti collegati
** sockfd: server socket file descriptor */
int askList(int sockfd)
{
	char message[MAXLINE];
	memset(message, '\0', MAXLINE);
	snprintf(message, sizeof(message), "LIST NULL NULL NULL\r\n");
	/* spedisce la stringa al server */
	Write(sockfd, message, strlen(message));
	printf("\nList of available users |id - nickname|\n\n");
	return 0;	

}

/* Client message management. Find the type of messagge with parseMessage
** and then call the relative function 
** sockfd: server socket file descriptor 
** buff: string received by client */
int clientMessagemng (char* buff, int sockfd)
{
	int n,k;
	char * param [10];
	//printf("call parse message\n");
	if( (n = parseMessage(buff, param)) >= 0 ){
		//printf("parseMessage return value: %d\n", n);
		switch (n)
		{
			case SROK:
				/* request list of available users */
				askList(sockfd);
			break;
			case ERRR:
				printf("\nError: %s\n", param[1]);
				if (atoi(param[1]) == 440)
					auth( sockfd );
				else
					askList(sockfd);
			break;
			case RLIS:
				if ((strcmp(param[1], "NULL")) == 0)
					printf("No available user\n");
				else
					printf("%s\n",param[1]);
				chooseOption(sockfd);
				//selectPlayer(sockfd);
			break;
			case USOK:
				printf("USOK reply received\n");
				//beginGame(sockfd);
			break;
			default: 
				printf("Unknown message\n");
			break;
		}
	}
	return 0;
}

/* select player by ID 
** sockfd: server socket file descriptor */
int selectPlayer(int sockfd)
{
	char message[MAXLINE];
	char id[FD_SETSIZE];
	memset(id, '\0', FD_SETSIZE);
	printf("Please select player to play the match.\n");
	printf("Insert player's ID: ");
	/* inserire controllo input */
	fgetsn(id);
	memset(message, '\0', MAXLINE);
	snprintf(message, sizeof(message), "RUSR %s NULL NULL\r\n", id);
	Write(sockfd, message, strlen(message));
}

int chooseOption(int sockfd)
{
	char choice[3];
	memset(choice, '\0', 3);
	printf("What would you like to do?\n");
	printf("1 - Wait a player\n2 - Choose player\n3 - Reload List\n\n");
	printf("Insert your choice: ");
	fgetsn(choice);
	switch (atoi(choice))
	{
		case 1:
		break;
		case 2:
			selectPlayer(sockfd);
		break;
		case 3:
			system("clear");
			askList(sockfd);
		break;
		default:
		break;
	}	
}
