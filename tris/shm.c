#include "semfun.h"
#include "semfun.c"

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

main()
{
  int n,semid,shmid,val,*addr;
  long i;

  semid = semget(IPC_PRIVATE,1,0600);
  if (semid == -1) perror("Creazione semaforo");
  seminit(semid,0,1);
  shmid = shmget(IPC_PRIVATE,sizeof(int),0600);
  if (shmid == -1) perror("Creazione memoria condivisa");
  addr = (int *) shmat(shmid, 0, 0);
  if ((int)addr == -1) perror("Attach memoria condivisa");
  *addr = 10; /* inizializza il dato condiviso */

  n= fork();
  if (n == -1) perror("Fork");

  if (n == 0)
  {
    down(semid,0); 
    (*addr)++;
    printf("Valore numero condiviso: %d\n", *addr);
    up(semid,0); 
  }
  else
  {
    down(semid,0); 
    (*addr)++;
    printf("Valore numero condiviso: %d\n", *addr);
    up(semid,0); 

    wait(NULL);
    if (semctl(semid,0,IPC_RMID)==-1) perror("Rimozione semaforo");
    if (shmctl(shmid,IPC_RMID,0)==-1) perror("Rimozione shm");
  }
}
