/* Exercice 4 - Client */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define PORT 3491

int main(int argc, char *argv[])
{
  int sockfd, numbytes;  
  struct sockaddr_in their_addr;
         // connector's address information 
  struct hostent *he;
  long int term1, term2, res;

  if (argc != 4) {
    fprintf(stderr, "Donner le nom du serveur"
	            " puis les deux entiers à"
                    " additionner en arguments.");
    return EXIT_FAILURE;
  }
   
  term1=atol(argv[2]);

  term2=atol(argv[3]);
      
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
   
  term1=htonl(term1);
  if (send(sockfd, &term1, sizeof(long int), 0) == -1){
    perror("Client: send 1");
    return EXIT_FAILURE;
  }

  term2=htonl(term2);
  if (send(sockfd, &term2, sizeof(long int), 0) == -1){
    perror("Client: send 2");
    return EXIT_FAILURE;
  }

  if ((numbytes=recv(sockfd, &res, 
		     sizeof(long int), 0)) == -1) {
    perror("Client: recv");
    return EXIT_FAILURE;
  }

  res=ntohl(res);
 
  printf("Somme recue du serveur: %ld\n", res);
   
  close(sockfd);
  return EXIT_SUCCESS;
} 
