/* Exercice 2 - Client */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define PORT 5555


int main(int argc, char *argv[])
{
  int sockfd, numbytes;
  char buf[100];
  struct sockaddr_in their_addr;
  struct hostent *he;

  if (argc != 2) {
    fprintf(stderr, "Donner le nom de la machine"
	    " distante en argument.");
    return EXIT_FAILURE;
  }


  if ((he=gethostbyname(argv[1])) == NULL) {
    perror("Client: gethostbyname");
    return EXIT_FAILURE;
  }


  if ((sockfd = socket(PF_INET,
		       SOCK_STREAM, 0)) == -1) {
    perror("Client: socket");
    return EXIT_FAILURE;
  }

  their_addr.sin_family = AF_INET;
  their_addr.sin_port = htons(PORT);
  their_addr.sin_addr = *((struct in_addr*)he->h_addr);
  memset(&(their_addr.sin_zero), '\0', 8);





  if (connect(sockfd, (struct sockaddr *)&their_addr,
	      sizeof(struct sockaddr)) == -1) {
    perror("Client: connect");
    return EXIT_FAILURE;
  }


  if ((numbytes=recv(sockfd, buf, 99, 0)) == -1) {
    perror("Client: recv");
    return EXIT_FAILURE;
  }
  buf[numbytes] = '\0';
  printf("Message recu du serveur: %s",buf);






  close(sockfd);
  return EXIT_SUCCESS;
}
