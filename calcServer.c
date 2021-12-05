#include <stdio.h>      /* for snprintf */
#include "csapp.h"
#include "calc.h"
#include <pthread.h>

struct aThread {
  int client_fd;
  struct Calc *calc;
};

int create_server_socket(int port) {
  struct sockaddr_in serveraddr = {0};
  int ssock_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (ssock_fd < 0)
    return -1;
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
  serveraddr.sin_port = htons((unsigned short)port);
  if (bind(ssock_fd, (struct sockaddr *) &serveraddr,
	   sizeof(serveraddr)) < 0)
    return -1;
  if (listen(ssock_fd, 5) < 0) return -1;
  return ssock_fd;
}

int accept_connection(int ssock_fd, struct sockaddr_in clientaddr) {
  unsigned clientlen = sizeof(clientaddr);
  int childfd = accept(ssock_fd,
		       (struct sockaddr *) &clientaddr,
		       &clientlen);
  if (childfd < 0)
    return -1;
  return childfd;
}

int chat_with_client(struct Calc *calc, int client_fd) {
  
  rio_t rio;
  int val;
  char temp[1024];
  rio_readinitb(&rio, client_fd);
  char buf[1024];
  // Read line from client
  ssize_t rc = rio_readlineb(&rio, buf, sizeof(buf)-1);
  if (rc < 0) { return 1; } // error reading data from client
  buf[rc] = '\0';

  int ex = 1;
  while (ex == 1){
    
    if (strcmp(buf, "quit\n") == 0 || strcmp(buf, "quit\r\n") == 0) {
      ex = 0;
    } else if( strcmp(buf, "shutdown\n") == 0 || strcmp(buf, "shutdown\r\n") == 0) {
      exit(1);
    }  else {
      
      calc_eval(calc, buf, &val);
      
      snprintf(temp, 1024, "%d\n", val);
      
      rio_writen(client_fd, temp, strlen(temp));
      ex = 1;
      memset(buf, 0, sizeof(buf));
      rc = rio_readlineb(&rio, buf, sizeof(buf)-1);
    }
    
  }
  return ex;
}

void * work(void* arg) {
  pthread_detach(pthread_self());
  
  struct aThread *obj = arg;
  int keep_going = 1;
  int client_fd = obj->client_fd;
  while (keep_going != -1) {
    if (client_fd > 0) {
      keep_going = chat_with_client(obj->calc, client_fd);
    }
    if(keep_going == 0) {
      close(client_fd);
    }
  }
  close(client_fd);
  return NULL;
}

int main(int argc, char **argv) {
  if (argc != 2) { 
    return -1; 
  }
  int port = atoi(argv[1]);
  //default to 1024
  if (port < 1024) {
    port = 1024;
  }
  
  //create the calculator
  struct Calc *calc = calc_create();
  
  int server_fd = open_listenfd(argv[1]);
  if (server_fd < 0) { return -1; }
  //int keep_going = 1;
  while(1){
    int clientfd = Accept(server_fd, NULL, NULL);
    if (clientfd < 0) {
      continue;
    }
    
    /* create aThread object */
    struct aThread *info = malloc(sizeof(struct aThread));
    info->client_fd = clientfd;
    info->calc = calc;
    
    /* start new thread to handle client connection */
    pthread_t thr_id;
    if (pthread_create(&thr_id, NULL, work, info) != 0) {
      exit(1);
    }
  }
  
  
  close(server_fd); // close server socket
  return 0;
}
