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

void shift(char text[],int size)
{
  for (int i = 0; i < size-1; i++) {
    text[i] = text[i+1];
  }
}

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
  char msg[500];
  char *input = malloc(500);
  char *get_message = malloc(500);
  int finish;
  stop = 0;
  finish = 0;


  while(stop != 1){
  if ((numbytes=recv(client_socket, msg, 500, 0)) == -1) {
    perror("Client: recv");
  }
  msg[numbytes] = '\0';
  if(msg[0] == 'w'){

    printf("Client connecté : \n");
    finish = 0;
    while(finish != 1){
      if ((numbytes=recv(client_socket, input, 500, 0)) == -1) {
        perror("Client: recv");
        return EXIT_FAILURE;
      }
      if(input[0] == 'E'){
        finish = 1;
      }
      else
      {
      printf("- %s\n",input );
      }
    }
    pthread_cond_signal(&cond_stock);
  }
  else if(msg[0] == 'f')
  {
    printf("Message a envoyé : ");
    scanf(" %99[^\n]",get_message);

    if (send(sockfd, get_message ,500,0)==-1) {
      perror("Serveur: send");
      }
    pthread_cond_signal(&cond_stock);
  }
  else if(msg[0] == 'm')
  {
    shift(msg,500);
    printf("Message reçu de%s\n", msg );
    pthread_cond_signal(&cond_stock);
  }
  else if(msg[0] == 'e')
  {
    printf("Erreur reçue : %s\n",msg);
    pthread_cond_signal(&cond_stock);
  }

 }

 return 0;
}



int main(int argc, char *argv[])
{
  int numbytes;
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

  int valid_name = 0;
  while(valid_name == 0){

  if (send(sockfd, name  ,100,0)==-1) {
    perror("Serveur: send");
    return EXIT_FAILURE;
  }
  if ((numbytes=recv(sockfd, name, 500, 0)) == -1) {
    perror("Client: recv");
  }
  name[numbytes] = '\0';

  if(name[0] == 'y')
  {
    valid_name = 1;
  }
  else
  {
    printf("Pseudo déja pris entrer en un autre : ");
    scanf("%s",name );
    printf("%s\n",name );
  }

  }
  int choice;
  char chatter[100];




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
      if (send(sockfd, "1 I want to see who's up"  ,50,0)==-1) {
        perror("Serveur: send");
        return EXIT_FAILURE;
      }
    pthread_cond_wait(&cond_stock,&mutex_stock);

    if (send(sockfd, "2 I want to chat"  ,101,0)==-1) {
      perror("Serveur: send");
      return EXIT_FAILURE;
    }
    printf("Avec qui voulez vous chatter ? : ");
    scanf("%s", chatter );
    if (send(sockfd, chatter  ,100,0)==-1) {
      perror("Serveur: send");
      return EXIT_FAILURE;
    }
    pthread_cond_wait(&cond_stock,&mutex_stock);
    }



    else if (choice == 1){
      if (send(sockfd, "1 I want to see who's up"  ,50,0)==-1) {
        perror("Serveur: send");
        return EXIT_FAILURE;
      }
  pthread_cond_wait(&cond_stock,&mutex_stock);
  }
}

  close(sockfd);
  return EXIT_SUCCESS;
}
