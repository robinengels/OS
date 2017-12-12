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

#define PORT 5555

int main()
{

  int sock_fd, new_fd,numbytes; //Listen sock_fd, get new connection on new_fd

  char buf[100];


  struct sockaddr_in server_addr; //Server address information

  struct sockaddr_in client_addr; //Client address information maybe a list ?

  int yes = 1; //Parameters for setsockopt

  unsigned int sin_size = sizeof(struct sockaddr_in);

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
      new_fd = accept(sock_fd, (struct sockaddr *)&client_addr, &sin_size);
      if (new_fd == -1) {
      perror("Serveur: accept");
      }

      if (fork() == 0){

      if (send(new_fd,"Bonjour client\n",16,0) == -1) {
        perror("Serveur: send");
        return EXIT_FAILURE;
        }


      if ((numbytes=recv(new_fd, buf, 99, 0)) == -1) {
        perror("Client: recv");
        return EXIT_FAILURE;
      }
      buf[numbytes] = '\0';
      printf("Message recu du client: %s\n",buf);

      }

      close(new_fd);
    }

}
