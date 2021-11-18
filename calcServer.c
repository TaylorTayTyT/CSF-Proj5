#include <stdio.h>      /* for snprintf */
#include "csapp.h"
#include "calc.h"

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

	//printf("ENTERED \n");
	rio_t rio; int val;
	char* temp = "";
	rio_readinitb(&rio, client_fd);
	char buf[1024];
	// Read line from client
	ssize_t rc = rio_readlineb(&rio, buf, sizeof(buf)-1);
	if (rc < 0) { return 1; } // error reading data from client
	buf[rc] = '\0';

	printf("THIS IS STRING: %s\n", buf);

	if (strcmp(buf, "quit\n") == 0 || strcmp(buf, "quit\r\n") == 0) {
		return 0;
	} else {
		val = calc_eval(calc, buf, &val);
		/*
    //Not sure if we need all of this since we know that we are only reading 1 line per entry, and it is stored in buf
		FILE *in = fmemopen(buf, (size_t) rc, "r");
		while (fscanf(in, "%s", temp) == 1) {
			strcat(buf, temp);
		}
    //this refers to stuff above ^. I think we just need to get the evaluted value then have the server spit out the answer as a response.
	  int error = calc_eval(calc, buf, &val);
    if(error == 0) {
      return 0;
    }
    printf("\nHello!\n");
    //this is printing the buf.
    //Why?
		rio_writen(client_fd, buf, strlen(buf));
		return 1;
		*/
	}
	return 1;
}
	
int main(int argc, char **argv) {
	/* TODO: implement this program */
	//I literally just copies and pasted this form Lecture 27
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
	//not given the port number

		printf("ARGUMENT: %s", argv[1]);
		//int server_fd = create_server_socket(port);
		int server_fd = open_listenfd(argv[1]);
	//int server_fd = open_listenfd(argv[1]);
	if (server_fd < 0) { return -1; }
	int keep_going = 1;
	while (keep_going) {
		struct sockaddr_in clientaddr;
		//int client_fd = accept_connection(server_fd, clientaddr);
		int client_fd = Accept(server_fd, NULL, NULL);
		if (client_fd > 0) {
			keep_going = chat_with_client(calc, client_fd);
			//close(client_fd); // close the connection
		}
	}
	close(server_fd); // close server socket
	return 0;
}