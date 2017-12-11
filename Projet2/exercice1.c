/* Exercice 1 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main()
{
  char name[100];
  struct hostent *h;
  
  if(gethostname(name, sizeof(name))==-1) {
    perror("gethostname: ");
    return EXIT_FAILURE;  
  }
  
  if ((h=gethostbyname(name)) == NULL) { 
    herror("gethostbyname: ");
    return EXIT_FAILURE;
  }
  
  printf("Host name  : %s\n", name);
  printf("IP Address : %s\n", 
	 inet_ntoa(*((struct in_addr *)h->h_addr)));
  
  return EXIT_SUCCESS;
} 


