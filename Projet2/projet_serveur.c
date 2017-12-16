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
#include <string.h>
#include<pthread.h>
#define PORT 5555



typedef struct client{
  int fd;
  char name[100];
  struct client *next;
}client ;

client head = {-1,"HEAD",NULL};

void del_client(int client_socket)
{
  client *current;
  client *previous;
  current = &head;
  previous = &head;
  current = current->next;
  int found = 0;
  while(current->next != NULL && found == 0)
  {
    if (current->fd == client_socket){

      previous->next = current->next;
      found = 1;
      free(current);
    }
    previous = current;
    current = current->next;
  }
  if (current->fd == client_socket)
  {
    previous->next = current->next;
    found = 1;
    free(current);
  }

}

int is_online(char name[])
{
  client *current;
  current = &head;
  int found = 0;
  while(current->next != NULL)
  {
    if(strcmp(current->name,name) == 0){
      found = current->fd;
    }
    current = current->next;
  }

  if(strcmp(current->name,name) == 0){
    found = current->fd;
  }
  return found;
}


void affiche_client(int dest)
{
  if (send(dest,"w ",100,0)==-1) {
    perror("Serveur: send");
  }
  client *current;
  current = &head;
  current = current->next;
  while(current->next != NULL)
  {
    if (send(dest, current->name,100,0)==-1) {
      perror("Serveur: send");
    }
    current = current->next;
  }
  if (send(dest, current->name,100,0)==-1) {
    perror("Serveur: send");

  }
  if (send(dest,"E Terminated",100,0)==-1) {
    perror("Serveur: here send");

  }
  printf("J'ai terminé d'envoyer\n");
}

void append_client(int fd, char name[])
{
  client *current;
  client *new_client;

  new_client = (client *)malloc(sizeof(client));
  strcpy(new_client->name, name);
  new_client->fd = fd;


  current = &head;
  while(current->next != NULL)
  {
    current = current->next;
  }
  current->next = new_client;
}

void *handle_client(void *socket)
{
  int client_socket = (int)socket;
  int numbytes,stop;
  char buf[100];
  int send_to;


  while(stop != 1){

  if ((numbytes=recv(client_socket, buf, 500, 0)) == -1) {
    perror("Client: recv");
  }
  buf[numbytes] = '\0';
  if(buf[0] == '3')
  {
    stop = 1;
  }

  else if(buf[0] == '1')
  {
    printf("Un client demande a voir la liste\n");
    affiche_client(client_socket);
  }

  else if(buf[0]=='2')
  {
    if ((numbytes=recv(client_socket, buf, 500, 0)) == -1) {
      perror("Client: recv");
    }
    send_to = is_online(buf);

    if (send(client_socket, "c He's online"  ,500,0)==-1) {
      perror("Serveur: send");
      return 0;
    }

    if ((numbytes=recv(client_socket, buf, 500, 0)) == -1) {
      perror("Client: recv");
    }

    if (send(send_to, buf  ,500,0)==-1) {
      perror("Serveur: send");
      return 0;
    }




  }

  }
  close(client_socket);
  del_client(client_socket);
  return 0;
}





int add_client(int sock_fd, struct sockaddr_in client_addr){
    int new_fd,numbytes;
    int *new_sock;
    unsigned int sin_size = sizeof(struct sockaddr_in);
    char buf[100];

    if ((new_fd = accept(sock_fd, (struct sockaddr *)&client_addr, &sin_size)) == -1 )
    {
      perror("Serveur: accept");
    }


      if ((numbytes=recv(new_fd, buf, 99, 0)) == -1) {
        perror("Client: recv");
        return EXIT_FAILURE;
      }
      buf[numbytes] = '\0';

    append_client(new_fd,buf);


    pthread_t tid;
    new_sock = malloc(1);
    *new_sock = new_fd;

    if( pthread_create( &tid , NULL, handle_client, (void*) new_fd) < 0)
    {
        perror("could not create thread");
        return 1;
    }


    return new_fd;
}

int main()
{

  int sock_fd; //Listen sock_fd, get new connection on new_fd

  fd_set readfds;

  struct sockaddr_in server_addr; //Server address information

  struct sockaddr_in client_addr; //Client address information maybe a list ?

  int yes = 1; //Parameters for setsockopt

  if((sock_fd = socket(PF_INET,SOCK_STREAM,0)) == -1)
  {
    perror("Server : socket");
    return EXIT_FAILURE;
  }

  if (setsockopt(sock_fd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1)
  {
    perror("Server : setsockopt");
    return EXIT_FAILURE;
  }

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  server_addr.sin_addr.s_addr = INADDR_ANY;
  memset(&(server_addr.sin_zero), '\0', 8);

  if (bind(sock_fd, (struct sockaddr *)&server_addr,sizeof(struct sockaddr)) == -1)
  {
    perror("Server: bind");
    return EXIT_FAILURE;
  }

  if (listen(sock_fd, 20) == -1)
  {
    perror("Server: listen");
    return EXIT_FAILURE;
  }



  //------------------------------------------------------------------//

  while(1){
        FD_ZERO(&readfds);
        FD_SET(sock_fd,&readfds);
        struct timeval time_out = {5,0};
        select(sock_fd+1,&readfds,NULL,NULL,&time_out);

        if(FD_ISSET(sock_fd,&readfds))
        {
        printf("Nouvelle demande de connection\n");
        add_client(sock_fd,client_addr);
        }


        }
      return 0;
}
