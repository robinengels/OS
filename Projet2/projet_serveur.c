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
  if (send(dest,"w ",500,0)==-1) {
    perror("Serveur: send");
  }
  client *current;
  current = &head;
  current = current->next;
  while(current->next != NULL)
  {
    if (send(dest, current->name,500,0)==-1) {
      perror("Serveur: send");
    }
    current = current->next;
  }
  if (send(dest, current->name,500,0)==-1) {
    perror("Serveur: send");

  }
  if (send(dest,"E Terminated",500,0)==-1) {
    perror("Serveur: here send");

  }
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
  char message[10];
  char online[100] = "found";
  char inc_msg[499];
  char pseudo[100];


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
    affiche_client(client_socket);
  }

  else if(buf[0]=='2')
  {
    printf("Quelqun veut chatter");
    if ((numbytes=recv(client_socket, buf, 500, 0)) == -1) {
      perror("Client: recv");
    }
    client *current;
    current = &head;
    int found = 0;
    while(current->next != NULL)
    {
      if(strcmp(current->name,buf) == 0){
        found = current->fd;
      }
      current = current->next;
    }
    if(strcmp(current->name,buf) == 0){
      found = current->fd;
    }

    printf("%i\n",found );

    if (found != 0)
    {
    if (send(client_socket,online,100,0)==-1) {
      perror("Serveur: here send");
    }


    printf("Test\n");

    if ((numbytes=recv(client_socket, inc_msg, 500, 0)) == -1) {
      perror("Client: recv");
    }
    printf("Message reçu du buf%s\n",inc_msg );

    strcpy(message,"m ");
    strcat(message,inc_msg);
    printf("Message %s\n",message );
    if (send(found,message,500,0)==-1) {
      perror("Serveur: here send");
    }
  }
  else
  {
    printf("Mauvais pseudo");
    if (send(client_socket,"Error client not found",100,0)==-1) {
      perror("Serveur: here send");
    }
  }





  }

  }
  close(client_socket);
  del_client(client_socket);
  return 0;
}





int add_client(int sock_fd, struct sockaddr_in client_addr){
    int new_fd,numbytes,valid,result;
    int *new_sock;
    unsigned int sin_size = sizeof(struct sockaddr_in);
    char buf[100];

    if ((new_fd = accept(sock_fd, (struct sockaddr *)&client_addr, &sin_size)) == -1 )
    {
      perror("Serveur: accept");
    }
    valid = 0;
    char response[100]="0 w";
    while(valid == 0){
    if ((numbytes=recv(new_fd, buf, 100, 0)) == -1) {
      perror("Client: recv");
      return EXIT_FAILURE;
    }
    buf[numbytes] = '\0';
    client *current;
    current = &head;
    int found = 0;
    while(current->next != NULL)
    {
      if(strcmp(current->name,buf) == 0){
        found = 1;
      }
      current = current->next;
    }
    if(strcmp(current->name,buf) == 0){
      found = 1;
    }

    if (found == 0)
    {
      response[0] = 'y';
      valid = 1;
    }

    if (send(new_fd, response,100,0)==-1) {
      perror("Serveur: send");
    }

    }
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
        add_client(sock_fd,client_addr);
        }


        }
      return 0;
}
