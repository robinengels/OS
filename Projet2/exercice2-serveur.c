/* Exercice 2 - Serveur */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#define MYPORT 3490 
#define BACKLOG 10  

int main()
{
  int sockfd, new_fd;  
        // listen on sock_fd, new connection on new_fd
  struct sockaddr_in my_addr;    
        // my address information
  struct sockaddr_in their_addr; 
        // connector's address information
  unsigned int sin_size;
  int yes=1;
  
  if ((sockfd = socket(PF_INET, 
		       SOCK_STREAM, 0)) == -1) {
    perror("Serveur: socket");
    return EXIT_FAILURE;
  }
  
  if (setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,
		 &yes,sizeof(int)) == -1) {
    perror("Serveur: setsockopt");
    return EXIT_FAILURE;
  }
        
  my_addr.sin_family = AF_INET;         
  my_addr.sin_port = htons(MYPORT);     
  my_addr.sin_addr.s_addr = INADDR_ANY; 
  memset(&(my_addr.sin_zero), '\0', 8); 
  




  if (bind(sockfd, (struct sockaddr *)&my_addr, 
	   sizeof(struct sockaddr)) == -1) {
    perror("Serveur: bind");
    return EXIT_FAILURE;
  }
  
  if (listen(sockfd, BACKLOG) == -1) {
    perror("Serveur: listen");
    return EXIT_FAILURE;
  }
  
  
  sin_size = sizeof(struct sockaddr_in);
  new_fd = accept(sockfd, 
		  (struct sockaddr *)&their_addr,
		  &sin_size);
  if (new_fd == -1) {  
    perror("Serveur: accept");
    return EXIT_FAILURE;
  }
  
  printf("Serveur:  connection recue du client %s\n",
	 inet_ntoa(their_addr.sin_addr));
  
  if (send(new_fd, "Bonjour client!\n",16,0) == -1){
    perror("Serveur: send");
    return EXIT_FAILURE;
  }

  close(sockfd);
  close(new_fd);
  return EXIT_SUCCESS;
}

