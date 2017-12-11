/* Exercice 4 - Serveur */

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

#define MYPORT1 3490
        //Le 1er port ("bonjour")
#define MYPORT2 3491
        //Le 2eme port (calculatrice)

#define BACKLOG 10

int main()
{

  int sockfd1,sockfd2, new_fd;
        // listen on sock_fd, new connection on new_fd
  int max;
  struct sockaddr_in my_addr;
        // my address information
  struct sockaddr_in their_addr;
        // connector's address information
  unsigned int sin_size;
  int yes=1;
  fd_set readfds;
  long int term1, term2, res;


  //Initialisation du 1er socket.
  if ((sockfd1 = socket(PF_INET,
			SOCK_STREAM, 0)) == -1) {
    perror("Serveur: socket 1");
    return EXIT_FAILURE;
  }

  if (setsockopt(sockfd1,SOL_SOCKET,SO_REUSEADDR,
		 &yes,sizeof(int)) == -1) {
    perror("Serveur: setsockopt 1");
    return EXIT_FAILURE;
  }

  my_addr.sin_family = AF_INET;
  my_addr.sin_port = htons(MYPORT1);
  my_addr.sin_addr.s_addr = INADDR_ANY;
  memset(&(my_addr.sin_zero), '\0', 8);

  if (bind(sockfd1, (struct sockaddr *)&my_addr,
	   sizeof(struct sockaddr)) == -1) {
    perror("Serveur: bind 1");
    return EXIT_FAILURE;
  }

  if (listen(sockfd1, BACKLOG) == -1) {
    perror("Serveur: listen 1");
    return EXIT_FAILURE;
  }

  sin_size = sizeof(struct sockaddr_in);

  //Initialisation du 2eme socket.
  if ((sockfd2 = socket(PF_INET,
			SOCK_STREAM, 0)) == -1) {
    perror("Serveur: socket 2");
    return EXIT_FAILURE;
  }

  if (setsockopt(sockfd2,SOL_SOCKET,SO_REUSEADDR,
		 &yes,sizeof(int)) == -1) {
    perror("Serveur: setsockopt 2");
    return EXIT_FAILURE;
  }

  my_addr.sin_family = AF_INET;
  my_addr.sin_port = htons(MYPORT2);
  my_addr.sin_addr.s_addr = INADDR_ANY;
  memset(&(my_addr.sin_zero), '\0', 8);

  if (bind(sockfd2, (struct sockaddr *)&my_addr,
	   sizeof(struct sockaddr)) == -1) {
    perror("Serveur: bind 2");
    return EXIT_FAILURE;
  }

  if (listen(sockfd2, BACKLOG) == -1) {
    perror("Serveur: listen 2");
    return EXIT_FAILURE;
  }


  while(1){

    //Initialisation du fd_set
    FD_ZERO(&readfds);
    FD_SET(sockfd1, &readfds);
    FD_SET(sockfd2, &readfds);
    max = (sockfd1 < sockfd2 ? sockfd2 : sockfd1);

    //Le select()
    select(max+1, &readfds, NULL, NULL, NULL);

    //Si c'est le service 1 qui est demand�...
    if (FD_ISSET(sockfd1, &readfds)){

      new_fd = accept(sockfd1, (struct sockaddr *)&their_addr, &sin_size);
      if (new_fd == -1) {
	perror("Serveur: accept");
      }
            printf("Serveur:  connection recue du"
	     " client %s pour le service 1\n",
	     inet_ntoa(their_addr.sin_addr));

      if (fork()==0) {
	/* This is the child process */
	close(sockfd1);
	close(sockfd2);
	if (send(new_fd,"Bonjour client\n",16,0) == -1) {
	  perror("Serveur: send");
	  return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
      }
    }

    //Si c'est le service 2 qui est demand�...
    else if(FD_ISSET(sockfd2, &readfds)){

      new_fd = accept(sockfd2,
		      (struct sockaddr *)&their_addr,
		      &sin_size);

      if (new_fd == -1) {
	perror("Serveur: accept");
      }

      printf("Serveur:  connection recue"
             " du client %s pour le service 2\n",
	     inet_ntoa(their_addr.sin_addr));

      if (fork()==0) {
	/* This is the child process */
	close(sockfd2);
	close(sockfd1);

	if (recv(new_fd, &term1,
		 sizeof(long int), 0) == -1) {
	  perror("Serveur: recv 1: ");
	  return EXIT_FAILURE;
	}
	term1=ntohl(term1);

	if (recv(new_fd, &term2,
		 sizeof(long int), 0) == -1) {
	  perror("Serveur: recv 2: ");
	  return EXIT_FAILURE;
	}
	term2=ntohl(term2);

	res=htonl(term1+term2);

	if (send(new_fd, &res,
		 sizeof(long int),0) == -1){
	  perror("Serveur: send");
	  return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
      }
    }
    else{
      fprintf(stderr, "Serveur: erreur de select\n");
    }
    close(new_fd);
  }

  return EXIT_SUCCESS;
}
