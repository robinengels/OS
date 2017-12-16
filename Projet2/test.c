#include <stdio.h>
#include <string.h>
#include <stdlib.h>

pthread_mutex_lock(&mutex_wait);
pthread_cond_wait(&cond_wait,&mutex_wait);
pthread_mutex_unlock(&mutex_wait);


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


void affiche_client()
{
  client *current;
  current = &head;
  while(current->next != NULL)
  {
    printf("%s\n",current->name );
    current = current->next;
  }
    printf("%s\n",current->name );

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


int main(int argc, char const *argv[]) {
  append_client(5,"André");
  append_client(6,"Mortibal");
  append_client(7,"Hihi");
  printf("%i\n",is_online("Hihi") );

  return 0;
}
