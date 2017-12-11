/* Exercice 5*/

#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc,char *argv[]) {
  struct sockaddr_in addr;
  struct hostent *hp;
  int	sockfd;
  char	*hostname,*request,*cp;
  FILE	*local; 
  char	buff[BUFSIZ];
  int	i,l,nrv;
   
  // Vérifier le nombre d'arguments effectifs
  if(argc!=3) {
    printf("Argument 1 = URL, argument 2 = fichier\n");
    return EXIT_FAILURE;
  }

  //Découper l'URL en nom d'hôte et chemin de fichier.
  if((cp=strchr(argv[1],'/'))) {
    *cp = '\0';
    l = strlen(argv[1]);
    hostname = malloc(l+1);
    strcpy(hostname,argv[1]);
    *cp = '/';
    l = strlen(cp);
    request = malloc(l+1);
    strcpy(request,cp);
  }
  else {
    hostname = argv[1];
    request = "/";
  }
  
  //Chercher l'adresse IP de la machine distante
  if ((hp=gethostbyname(hostname)) == NULL) {   
    perror("gethostbyname");
    return EXIT_FAILURE;
  }
  
  //Initialiser le struct sockaddr_in
  addr.sin_family = AF_INET;  
  addr.sin_port = htons(80);  
  addr.sin_addr = *((struct in_addr *)hp->h_addr);
  memset(&(addr.sin_zero), '\0', 8); 
  
  if ((sockfd = socket(PF_INET, 
		       SOCK_STREAM, 0)) == -1) {
    perror("socket");
    return EXIT_FAILURE;
  }

  if((local = fopen(argv[2],"w")) == NULL) {
    perror("fopen");
    return EXIT_FAILURE;
  }

  
  if (connect(sockfd, (struct sockaddr *)&addr,
	      sizeof(struct sockaddr)) == -1) {
    perror("connect");
    return EXIT_FAILURE;
  }

  // Préparer la requète HTTP
  sprintf(buff,"GET %s HTTP/1.1\r\nHost: %s"
	  "\r\nConnection: close\r\n\r\n",
	  request,hostname);
  l = strlen(buff);

  // Envoyer la requète au serveur
  write(sockfd,buff,l);
  

  // Stocker la réponse sur fichier
  do {
    nrv = read(sockfd,buff,BUFSIZ);
    if(nrv > 0)  {
      for(i=0;i<nrv;i++) 
	putc(buff[i],local);
    }
    else
      break;
  } 
  while(1);

  close(sockfd);
  fclose(local);

  return EXIT_SUCCESS;
}

