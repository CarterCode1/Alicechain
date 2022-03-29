// Server side implementation of UDP client-server model
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>

#define MAX 80
#define PORT	 8080
#define TCP_PORT 8088
#define MAXLINE 1024
#define SA struct sockaddr

// Driver code
int main() {

	bool isContinue = true;
	int sockfd, tcp_sock_fd;
	char buffer[MAXLINE];
	char *hello = "Hello from server";
	struct sockaddr_in servaddr, tcpServAddress, cliaddr;
	
	// Creating socket file descriptor
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) 
	{
		perror(" UDP socket creation failed");
		exit(EXIT_FAILURE);
	}

	memset(&servaddr, 0, sizeof(servaddr));
	memset(&cliaddr, 0, sizeof(cliaddr));
	
	// Filling server information
	servaddr.sin_family = AF_INET; // IPv4
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(PORT);
	
	// Bind the socket with the server address
	if ( bind(sockfd, (const struct sockaddr *)&servaddr,
			sizeof(servaddr)) < 0 )
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}



	// socket create and verification
	tcp_sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (tcp_sock_fd == -1)
	{
		printf("TCP socket creation failed...\n");
		exit(0);
	}
	else
	{
		printf(" TCP Socket successfully created..\n");
	}

	bzero(&tcpServAddress, sizeof(tcpServAddress));

	// assign IP, PORT
	tcpServAddress.sin_family = AF_INET;
	tcpServAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
	tcpServAddress.sin_port = htons(TCP_PORT);

	// connect the client socket to server socket
	if (connect(tcp_sock_fd, (SA*)&tcpServAddress, sizeof(tcpServAddress)) != 0) {
		printf("connection with the server failed...\n");
		exit(0);
	}
	else
	{
		printf("connected to the server..\n");
	}


	while(isContinue) 
	{
		int len, n;

		len = sizeof(cliaddr); //len is value/resuslt

		n = recvfrom(sockfd, (char *)buffer, MAXLINE,
					MSG_WAITALL, ( struct sockaddr *) &cliaddr,
					&len);
		buffer[n] = '\0';
		printf("Client : %s\n", buffer);

		char buff[MAX];
		int m;
		
		bzero(buff, sizeof(buff));
		printf("Enter the string : ");
		m = 0;
		while ((buff[m++] = getchar()) != '\n')
			;

		write(tcp_sock_fd, buff, sizeof(buff));
		bzero(buff, sizeof(buff));
		read(tcp_sock_fd, buff, sizeof(buff));
		printf("From Server : %s", buff);
		if ((strncmp(buff, "exit", 4)) == 0) 
		{
			isContinue = false;
			// close the socket
			printf("Client Exit...\n");
			break;
		}

		hello = buff;


		sendto(sockfd, (const char *)hello, strlen(hello),
			MSG_CONFIRM, (const struct sockaddr *) &cliaddr,
				len);
		printf("Hello message sent.\n");
		
	}
	
	close(tcp_sock_fd);
	return 0;
}
