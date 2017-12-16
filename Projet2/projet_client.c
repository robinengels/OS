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
#include <pthread.h>

#define PORT 5555

int sockfd;
static pthread_cond_t cond_stock = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t mutex_stock = PTHREAD_MUTEX_INITIALIZER;

void main_menu()
{
  printf("Bienvenu sur ChatCana le chat de Jacana\n");
  printf("Que voulez vous faire ?\n");
  printf("1) Afficher la liste des personnes connectées\n");
  printf("2) Commencer a chatter avec Hmedi\n");
  printf("3) Nous quitter :'( \n\n");
}

void *handle_client(void *socket)
{
  int client_socket = (int)socket;
  int numbytes,stop;
  char msg[100];
  int send_to,finish;
  stop = 0;


  while(stop != 1){
  if ((numbytes=recv(client_socket, msg, 500, 0)) == -1) {
    perror("Client: recv");
  }
  if(msg[0] == 'w'){

    printf("Client connecté : \n");
    finish = 0;
    while(finish != 1){
      if ((numbytes=recv(client_socket, msg, 500, 0)) == -1) {
        perror("Client: recv");
        return EXIT_FAILURE;
      }
      msg[numbytes] = '\0';
      if(msg[0] == 'E'){
        finish = 1;
      }
      else
      {
      printf("- %s\n",msg );
      }
    }
  }
  pthread_cond_signal(&cond_stock);

 }
}



int main(int argc, char *argv[])
{
  int numbytes,finish;
  char buf[100];
  struct sockaddr_in their_addr;
  struct hostent *he;

  char name[100];

  printf("Entrer le pseudo que vous désirer : ");
  scanf("%s",name );

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

  if (send(sockfd, name  ,16,0)==-1) {
    perror("Serveur: send");
    return EXIT_FAILURE;
  }

  int choice;




  //------------------------------------------------------------------------//
  pthread_t tid;
  if( pthread_create( &tid , NULL, handle_client, (void*) sockfd) < 0)
  {
      perror("could not create thread");
      return 1;
  }

  while(choice != 3){
    main_menu();
    scanf("%i",&choice );
    if (choice == 3){

    if (send(sockfd, "3 I'm leaving you"  ,50,0)==-1) {
      perror("Serveur: send");
      return EXIT_FAILURE;
    }
    }

    else if (choice == 2){
    if (send(sockfd, "2 I want to chat"  ,50,0)==-1) {
      perror("Serveur: send");
      return EXIT_FAILURE;
    }
    }



    else if (choice == 1){
      if (send(sockfd, "1 I want to see who's up"  ,50,0)==-1) {
        perror("Serveur: send");
        return EXIT_FAILURE;
      }
      /*
      printf("Client connecté : \n");
      finish = 0;
      while(finish != 1){
        if ((numbytes=recv(sockfd, buf, 100, 0)) == -1) {
          perror("Client: recv");
          return EXIT_FAILURE;
        }
        buf[numbytes] = '\0';
        if(buf[0] == 'E'){
          finish = 1;
        }
        else
        {
        printf("- %s\n",buf );
        }
      }
    printf("\n\n");
  }*/
  pthread_cond_wait(&cond_stock,&mutex_stock);
  }
}

  close(sockfd);


  return EXIT_SUCCESS;
}
