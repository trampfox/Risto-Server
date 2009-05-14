#include	"lib/basic.h"

/* costanti protocollo tris */
#define HELO						0				/* messaggio di autenticazione */
#define SROK						1				/* autenticazione ok */
#define ERRR						2				/* autenticazione errata */
#define LIST						3				/* messaggio di richiesta lista giocatori disponibili */
#define REQQ						4				/* messaggio di richiesta partita */
#define RQOK						5				/* richiesta ok */
#define	QUIT						6				/* richiesta chiusura */

/* funzioni principali per il gioco del tris
 * Parte di autenticazione
 * Parte del parsing del messaggio (comune client e server)
 */

/* funzione per l'autenticazione del giocatore */
int auth(int sockfd)
{
	char username[MAXLINE], message[MAXLINE];
	/* type identifica il tipo di messaggio da inviare. In questo caso
	 * e' uguale a zero -> autenticazione */
	int type = HELO; 
	memset(username, '\0', MAXLINE);
	memset(message, '\0', MAXLINE);
	printf("welcome\n\nPlease insert your username: ");
	scanf("%s", &username);
	snprintf(message, sizeof(message), "HELO %s %s\r\n", username, username);
	printf("message: %s\n", message);
	/* spedisce la stringa al server */
	Write(sockfd, message, strlen(message));
}

/* ritorna il codice del messaggio passato come argomento 
 * il tipo del messaggio è dato dal primo carattere (numerico) */
messageType(char *message)
{
	return(atoi(&message[0]));
}

int parseMessage(char *buff)
{
	char tmp[5];
	memset(tmp, '\0', 5);
	strncpy(tmp, buff, 4);
	if (strcmp(tmp,"HELO") == 0) return HELO;
	if (strcmp(tmp,"SROK") == 0) return SROK;
	if (strcmp(tmp,"ERRR") == 0) return ERRR; 
	if (strcmp(tmp,"LIST") == 0) return LIST; 
	if (strcmp(tmp,"REQQ") == 0) return REQQ; 
	if (strcmp(tmp,"RQOK") == 0) return RQOK;
	if (strcmp(tmp,"QUIT") == 0) return QUIT;   
}

void messagemng(char *buff)
{
	int n;
	n = parseMessage(buff);
	printf("n: %d\n", n);
}
