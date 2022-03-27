// Client side implementation of UDP client-server model
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>

#define PORT1	 8080
#define  TCP_PORT 8088
#define MAXLINE 1024
#define SA struct sockaddr


// Driver code
int main() {
	
	bool isContinue = true;
	int sockfd1, tcpSockFd, connfd, len;
	char buffer[MAXLINE];
	char *hello = "Hello from client";
	struct sockaddr_in	 serv1addr, tcpServAddr, cli;

	//set up the  tcp socket to receive message

	// socket create and verification
	tcpSockFd = socket(AF_INET, SOCK_STREAM, 0);
	if (tcpSockFd == -1) {
		printf("socket creation failed...\n");
		exit(0);
	}
	else
	{
		printf("Socket successfully created..\n");
	}

	bzero(&tcpServAddr, sizeof(tcpServAddr));
	
	// assign IP, PORT
	tcpServAddr.sin_family = AF_INET;
	tcpServAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	tcpServAddr.sin_port = htons(TCP_PORT);

		// Binding newly created socket to given IP and verification
	if ((bind(tcpSockFd, (SA*)&tcpServAddr, sizeof(tcpServAddr))) != 0) {
		printf("socket bind failed...\n");
		exit(0);
	}
	else
	{
		printf("Socket successfully binded..\n");
	}

	// Now server is ready to listen and verification
	if ((listen(tcpSockFd, 5)) != 0) {
		printf("Listen failed...\n");
		exit(0);
	}
	else
	{
		printf("Server listening..\n");
	}
	len = sizeof(cli);len = sizeof(cli);

	// Accept the data packet from client and verification
	connfd = accept(tcpSockFd, (SA*)&cli, &len);
	if (connfd < 0) {
		printf("server accept failed...\n");
		exit(0);
	}
	else
	{
		printf("server accept the client...\n");
	}
	//---- END TCP SET UP REGION


	//----Setup for UDP client to server 1 
    // Creating socket file descriptor
    if ( (sockfd1 = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket  to server 1 creation failed");
        exit(EXIT_FAILURE);
    }
   
    memset(&serv1addr, 0, sizeof(serv1addr));
       
    // Filling server information
    serv1addr.sin_family = AF_INET;
    serv1addr.sin_port = htons(PORT1);
    serv1addr.sin_addr.s_addr = INADDR_ANY;
    

	int n;
	
	// infinite loop for sending and receiving messages
	while(isContinue) 
	{
		bzero(buffer, MAXLINE);

		// read the message from client and copy it in buffer
		read(connfd, buffer, sizeof(buffer));
		// print buffer which contains the client contents
		printf("From client: %s\t To client : ", buffer);

		int m, len;
	
		sendto(sockfd1, (const char *)buffer, strlen(buffer),
			MSG_CONFIRM, (const struct serv1addr *) &serv1addr,
			sizeof(serv1addr));
		
		printf("Hello message sent.\n");
		
		m = recvfrom(sockfd1, (char *)buffer, MAXLINE,
				MSG_WAITALL, (struct serv1addr *) &serv1addr,
				&len);
				
		buffer[m] = '\0';
		printf("Server : %s\n", buffer);




		// and send that buffer to client
		write(connfd, buffer, sizeof(buffer));

		// if msg contains "Exit" then server exit and chat ended.
		if (strncmp("exit", buffer, 4) == 0) {
			isContinue = false;
			printf("Server Exit ...\n");
			break;
		}
	}

	close(sockfd1);
	return 0;
}
