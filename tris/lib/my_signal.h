#include <signal.h>
#include <sys/wait.h>

typedef void Sigfunc(int); 

Sigfunc*
signal(int signo, Sigfunc *handler);

void
gestisci_zombie (int segnale);
