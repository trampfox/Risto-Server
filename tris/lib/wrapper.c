#include	"basic.h"

int Write(int sockfd, char *buff, int len)
{
	int n;
	n = write(sockfd, buff, len);
	if (n <= 0) {
		if (errno == EPIPE) {
			printf("EPIPE: chiusura anomala\n");
			exit(1);
		}
	/* se ricevo un errore di tipo EINTR non lo considero */
		else if (errno == EINTR);
		else if (errno == ETIMEDOUT) {
			printf("timeout\n");
			close(sockfd);
			exit(1);
		}
	}
	return(n);
}

int Read(int sockfd, char *rcv, int len)
{
	int n;
	n = read(sockfd, rcv, len);
	/* ricevo stringa nulla, quindi il server non e' piu' online */
	if (n <= 0) {
		if (errno == EINTR) {
			printf("\nerror connect reset: \n");
			exit(-1);
		}
		else {
			printf("\nserver not present \n");
			exit(-1);
		}
	}
	return (n);
}
