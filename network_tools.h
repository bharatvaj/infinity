#ifndef CLIENT_H
#define CLIENT_H
#define _NET_TAG "NETWORK_TOOLS"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define LOG
#include "logc.h"

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 256
#endif

#ifndef CON_MAX_ATTEMPTS
#define CON_MAX_ATTEMPTS 5
#endif
    
ssize_t write_data(int sockfd, const char *msg){
	ssize_t sent_bytes = 0;
	if ((sent_bytes = write (sockfd, msg, BUFFER_SIZE)) < 0){
		log_err(_NET_TAG, "sending failed");
		return -1;
	}
	log_inf(_NET_TAG, "sent data: %s", msg);
	return sent_bytes;
}

/** Read data from the given socket
 * @param sockfd The socket descriptor to read form
 */
char *read_data (int sockfd){
	char *buffer = (char *)malloc(BUFFER_SIZE);
	if (read (sockfd, buffer, BUFFER_SIZE) < 0){
		log_err(_NET_TAG, "read failed");
		return NULL;
	}
	log_inf(_NET_TAG, "data received: %s", buffer);
	return buffer;
}

int disconnect_server(int sockfd){
	if(close(sockfd) == -1){
		log_err(_NET_TAG, "Disconnection Unsuccesful");
        return -1;
	}
	else log_inf(_NET_TAG, "Disconnection Successful");
    return 0;
}

int connect_server (const char * hostname, int port){
	struct sockaddr_in serv_addr;
	struct hostent *server;
	//checking whether port is between 0 and 65536
	if (port < 0 || port > 65535){
		log_err (_NET_TAG, "invalid port number, port number should be between 0 and 65536");
		return -1;
	}
	//Create socket
	int sockfd = socket(AF_INET , SOCK_STREAM , 0);
	if (sockfd == -1){
		log_err(_NET_TAG, "Could not create socket");
		return -1;
	}
	log_inf(_NET_TAG, "Socket created");
	if((server = gethostbyname(hostname))==NULL){
		log_err(_NET_TAG, "no such host found");
		return -1;
	}
	memset((char *)&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	memcpy((char*)&serv_addr.sin_addr.s_addr, (char*)server->h_addr, server->h_length);
	serv_addr.sin_port = htons( port );
	int i = 0;
	while (connect (sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1){
		if(i++ > CON_MAX_ATTEMPTS){
			//guess other hostnames for the user
			close(sockfd);
			log_err(_NET_TAG, "cannot establish connection to %s on port %d", hostname, port);
			return -1;
		}
	}
	log_inf(_NET_TAG, "connection established successfully to %s on port %d", hostname, port);
	return sockfd;
}

#ifndef SERV_BACKLOG
#define SERV_BACKLOG 10
#endif

/** Starts the server with the standard IPv4 and TCP stack
 * @param port Port number for the server to start
 * @return Socket descriptor of the started server
 */
int start_server(int port){
	//Create socket
	int servfd = socket(PF_INET , SOCK_STREAM , 0);
	if (servfd == -1){
		log_err(_NET_TAG, "could not create socket");
		return -1;
	}

	struct sockaddr_in server, client;
    	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons( port );
	//Bind
	if( bind(servfd,(struct sockaddr *)&server , sizeof(server)) < 0){
		log_err(_NET_TAG, "bind failed");
		return -1;
	}
	//Listen
	listen(servfd , SERV_BACKLOG);
	//Accept and incoming connection
	log_inf(_NET_TAG, "Waiting for incoming connections...");
	//accept connection from an incoming client
	socklen_t cli_size = sizeof(struct sockaddr_in);
	int clifd = accept(servfd, (struct sockaddr *)&client, &cli_size);
	if (clifd < 0){
		log_err(_NET_TAG, "Accept failed");
		return -1;
	}
	log_inf(_NET_TAG, "Connection accepted");
	return clifd;
}
#endif
