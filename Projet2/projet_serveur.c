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
  int sock_fd, new_fd; //Listen sock_fd, get new connection on new_fd

  int fd_list[100]={0};

  int next_free=0;

  struct sockaddr_in server_addr; //Server address information

  struct sockaddr_in client_addr[100]; //Client address information maybe a list ?

  int yes = 1; //Parameters for setsockopt

  unsigned int sin_size = sizeof(struct sockaddr_in);

  fd_set readfds;

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

  FD_ZERO(&readfds);
  FD_SET(sock_fd,&readfds);
  int max;
  while(1){

    max = sock_fd;
    for(int i=0;i<100;i++)
    {
      if (fd_list[i] > max)
      {
        max = fd_list[i];
      }
    }

    select(max+1,&readfds,NULL,NULL,NULL);

    if (FD_ISSET(sock_fd,&readfds))
    {
      new_fd = accept(sock_fd,(struct sockaddr *)&client_addr[0],&sin_size);
      fd_list[next_free] = new_fd;
      FD_SET(new_fd,&readfds);

      if(fork()==0){

      if (send(fd_list[next_free],"Bonjour client\n",16,0) == -1) {
        perror("Serveur: send");
        return EXIT_FAILURE;
      }
      }
      next_free+=1;
    }

    for(int i=0;i<100;i++)
    {
      if(FD_ISSET(fd_list[i],&readfds))
      {
        if(fork()==0){
        if (send(fd_list[i],"Bonjour client\n"+i,16,0) == -1) {
          perror("Serveur: send");
          return EXIT_FAILURE;
        }
      }
      }
    }
  }



}
