#include	"basic.h"
#include	"my_signal.h"

Sigfunc *
signal(int signo, Sigfunc *func)
{
	struct sigaction	act, oact;

	act.sa_handler = func;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	if (signo != SIGALRM) {
	   act.sa_flags |= SA_RESTART;	
	}
	if (sigaction(signo, &act, &oact) < 0)
		return(SIG_ERR);
	return(oact.sa_handler);
}


void
gestisci_zombie (int segnale)
{

        int status;
        int pid;

        while((pid = waitpid(-1, &status, WNOHANG)) > 0)
           printf("child %d terminato\n", pid);

        return;

}

              
void
gestisci_ctrlc (int segnale){
	printf("Ciao!\n");
	exit(0);
	return;

}