// A simple server in the internet domain using TCP
// The port number is passed as an argument
// To compile: gcc server.c -o server
// Reference: Beej's networking guide, man pages

#define _POSIX_C_SOURCE 200112L
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <cjson/cJSON.h>

int create_listening_socket(char* service);

int main(int argc, char** argv) {
	int sockfd, newsockfd, n, port;
	char buffer[256], ip[INET_ADDRSTRLEN];
	struct sockaddr_in client_addr;
	socklen_t client_addr_size;

	if (argc < 2) {
		fprintf(stderr, "ERROR, no port provided\n");
		exit(EXIT_FAILURE);
	}

	fprintf(stdout, "Preparing to create listening socket on port %s ...\n", argv[1]);

	// Create the listening socket
	sockfd = create_listening_socket(argv[1]);

	fprintf(stdout, "Created listening socket.\n");

	// Listen on socket - means we're ready to accept connections,
	// incoming connection requests will be queued, man 3 listen
	if (listen(sockfd, 5) < 0) {
		perror("listen");
		exit(EXIT_FAILURE);
	}

	fprintf(stdout, "Started listening. \nReady.\n");

	// Accept a connection - blocks until a connection is ready to be accepted
	// Get back a new file descriptor to communicate on
	client_addr_size = sizeof client_addr;
	newsockfd = accept(sockfd, (struct sockaddr*)&client_addr, &client_addr_size);
	if (newsockfd < 0) {
		perror("accept");
		exit(EXIT_FAILURE);
	}

	// Print ipv4 peer information (can be removed)
	getpeername(newsockfd, (struct sockaddr*)&client_addr, &client_addr_size);
	inet_ntop(client_addr.sin_family, &client_addr.sin_addr, ip,
			  INET_ADDRSTRLEN);
	port = ntohs(client_addr.sin_port);
	fprintf(stdout, "new connection from %s:%d on socket %d\n", ip, port, newsockfd);

	while (1) {
		// Read characters from the connection, then process
		n = read(newsockfd, buffer, 255); // n is number of characters read
		if (n < 0) {
			perror("read");
			exit(EXIT_FAILURE);
		}
		// Null-terminate string
		buffer[n] = '\0';

		// Write message back
		fprintf(stdout, "Here is the message: %s\n", buffer);
		n = write(newsockfd, "I got your message\n", 19);
		if (n < 0) {
			perror("write");
			exit(EXIT_FAILURE);
		}
	}

	fprintf(stdout, "Closing sockets...\n");
	
	close(sockfd);
	close(newsockfd);

	fprintf(stdout, "Sockets closed. Exiting...\n");

	return 0;
}

int create_listening_socket(char* service) {
	int re, s, sockfd;
	struct addrinfo hints, *res;

	// Create address we're going to listen on (with given port number)
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;       // IPv4
	hints.ai_socktype = SOCK_STREAM; // Connection-mode byte streams
	hints.ai_flags = AI_PASSIVE;     // for bind, listen, accept
	// node (NULL means any interface), service (port), hints, res
	s = getaddrinfo(NULL, service, &hints, &res);
	if (s != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
		exit(EXIT_FAILURE);
	}

	fprintf(stdout, "Creating socket...\n");

	// Create socket
	sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (sockfd < 0) {
		perror("socket");
		exit(EXIT_FAILURE);
	}

	fprintf(stdout, "Socket created, binding address...\n");

	// Reuse port if possible
	re = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &re, sizeof(int)) < 0) {
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	// Bind address to the socket
	if (bind(sockfd, res->ai_addr, res->ai_addrlen) < 0) {
		perror("bind");
		exit(EXIT_FAILURE);
	}
	freeaddrinfo(res);

	fprintf(stdout, "Socket bound to address!\n");

	return sockfd;
}
