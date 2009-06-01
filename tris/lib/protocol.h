//#include	"basic.h"

/*Note USNO non è ancora stato implementato*/
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
#define USNO					  10
#define MVOK						11 			/* user move ok */
#define MVER						12			/* user move error */

#define MOSSA_VALIDA     1
#define MOSSA_NON_VALIDA 0
/* Game code 
** 110 -> start the game
** 220 -> wait the first push

/* Error code
** 330 -> request play against himself  
** 440 -> nickname already exixts
** 550 -> request user id doesn't exixt
*/
