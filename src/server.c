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
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <errno.h>
#include <fcntl.h>

#include "notebook_driver.h"
#include "my_bool.h"

#define MAX_CLIENTS 25
#define BUFFER_SIZE 256

int create_listening_socket(char* service);
void add_new_client(int newsockfd, struct pollfd fds[], int* nfds);
void delete_client(int i, struct pollfd fds[], int* nfds);
void* get_sockaddr(struct sockaddr* sa);
void initialiseFds(struct pollfd fds[], int nfds, int maxClients);
int set_nonblocking(int sockfd);

int main(int argc, char** argv) {

	RDictionary* notebookInstance = createNotebook();

	int sockfd, newsockfd;
	char buffer[BUFFER_SIZE];
	struct pollfd fds[MAX_CLIENTS];
	int timeout;
	int nfds = 1;
	socklen_t addrlen;
	struct sockaddr_storage remoteaddr; // Client address
	char remoteIP[INET_ADDRSTRLEN];

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

	// Set up the pollfd structure
	memset(fds, 0 , sizeof(fds));
	fds[0].fd = sockfd;
	fds[0].events = POLLIN;

	initialiseFds(fds, nfds, MAX_CLIENTS);

	timeout = (2500);

	for (;;) {
		printf("Number of clients: %d\n", nfds);

		// If timeout happens, poll() will return 0
		int poll_count = poll(fds, nfds, timeout);
		if (poll_count < 0) {
			perror("poll() failed");
			break;
		}
		printf("Iterating through poll() results...\n");
		for (int i = 0; i < nfds ;i++) {
			if (fds[i].revents & POLLIN) {
				printf("Event caught on socket %d\n", fds[i].fd);
				// sockfd is the listening socket
				// If it is readable, it means we have a new connection
				if (fds[i].fd == sockfd) {
					addrlen = sizeof remoteaddr;
					newsockfd = accept(sockfd, (struct sockaddr*)&remoteaddr, &addrlen);
					if (newsockfd < 0) {
						perror("accept() failed");
					} else {
						// Set the new socket to non-blocking
						if (set_nonblocking(newsockfd) < 0) {
							perror("set_nonblocking() failed");
							close(newsockfd);
							continue;
						}
						add_new_client(newsockfd, fds, &nfds);
						printf("Pollserver: new connection from %s on socket %d\n", 
								inet_ntop(remoteaddr.ss_family, get_sockaddr((struct sockaddr*) &remoteaddr), remoteIP, INET_ADDRSTRLEN), 
								newsockfd);
					}
				} else {
					// The coming data is not from the listening socket, so it is from an existing client
					// int nbytes = recv(fds[i].fd, buffer, sizeof buffer, 0);
					int sender_fd = fds[i].fd;
					char* readPool = NULL;
					size_t readPoolSize = 0; // Total size of readPool
					BOOL readComplete = FALSE;
					while (!readComplete) {
						errno = 0;
						int nbytes = recv(sender_fd, buffer, sizeof buffer, 0);
						if (nbytes <= 0) {
							if (errno == EAGAIN || errno == EWOULDBLOCK) {
								// No more data to read
								readPool = realloc(readPool, readPoolSize + 1);
								readPool[readPoolSize] = '\0';
								readComplete = TRUE;
								continue;
							} else {
								perror("recv() failed");
								close(fds[i].fd);
								delete_client(i, fds, &nfds);
								break;
							}
						} else {
							printf("Pollserver: received %d bytes from socket %d\n", nbytes, sender_fd);
							readPool = realloc(readPool, readPoolSize + nbytes);
							memcpy(readPool + readPoolSize, buffer, nbytes);
							readPoolSize += nbytes;
						}
					}
					printf("Pollserver: received message: %s\n", readPool);
					// TODO: Process the message
					cJSON* jsonStr = cJSON_Parse(readPool);
					if (jsonStr == NULL) {
						printf("Pollserver: failed to parse JSON string\n");
					} else {
						cJSON* response = processRequest(jsonStr, notebookInstance);
						char* responseStr = cJSON_Print(response);
						printf("Pollserver: response: %s\n", responseStr);
						send(sender_fd, responseStr, strlen(responseStr), 0);
						free(responseStr);
						cJSON_Delete(response);
						cJSON_Delete(jsonStr);
					}
				}
			}
		}
		printf("Iteration done.\n");
	}

	fprintf(stdout, "Closing sockets...\n");
	
	close(sockfd);

	fprintf(stdout, "Sockets closed. Exiting...\n");

	return 0;
}


/**
 * @brief Initialise the file descriptors, set all to -1
 * 
 * @param fds
 * @param nfds
 * @return void
 */
void initialiseFds(struct pollfd fds[], int startIdx, int maxClients) {
	for (int i = startIdx; i < maxClients; i++) {
		fds[i].fd = -1;
	}
}


/**
 * @brief Add a new client to the list of file descriptors
 * 
 * @param newsockfd
 * @param fds
 * @param nfds
 * @return void
 */
void add_new_client(int newsockfd, struct pollfd fds[], int* nfds) {
	int i;
	for (i = 1; i < MAX_CLIENTS; i++) {
		if (fds[i].fd == -1) {
			fds[i].fd = newsockfd;
			fds[i].events = POLLIN;
			(*nfds)++;
			break;
		}
	}
}


/**
 * @brief Delete a client from the list of file descriptors
 * 
 * @param i
 * @param fds
 * @param nfds
 * @return void
 */
void delete_client(int i, struct pollfd fds[], int* nfds) {
	fds[i].fd = -1;
	(*nfds)--;
}


/**
 * @brief Get the sockaddr, IPv4 or IPv6
 * 
 * @param sa
 * @return void*
 */
void* get_sockaddr(struct sockaddr* sa) {
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

// Define the set_nonblocking function
int set_nonblocking(int sockfd) {
    int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl failed");
        return -1;
    }
    flags |= O_NONBLOCK;
    if (fcntl(sockfd, F_SETFL, flags) == -1) {
        perror("fcntl failed");
        return -1;
    }
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

	// Set socket to be nonblocking,
		/* The following two lines are not true, it turns out that new fds also need to be manually set as non-blocking */
		// all of the sockets for the incoming connections will also be nonblocking
		// since they will inherit that state from the listening socket
	// fcntl called twice to set and get flags
	if (set_nonblocking(sockfd) < 0) {
		perror("set_nonblocking");
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
