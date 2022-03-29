#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#define MAXLINE 1024
#define PORT 8088
#define SA struct sockaddr

//TCP Client

void func(int sockfd, char* message)
{
	int n;
	write(sockfd, message, 1024);
	printf("Sent to Server : %s\n", message);

	bzero(message, MAXLINE);
	read(sockfd, message, sizeof(message));
	printf("From Server : %s\n", message);

}

int main(int argc, char *argv[])
{
	int sockfd, connfd;
	struct sockaddr_in servaddr, cli;
	char sendBuffer[MAXLINE];

	printf("Program name %s\n", argv[0]);

	if(argc == 2)
	{
		printf("Check Wallet Operation for %s\n", argv[1]);
	}
	else if (argc == 4)
	{
		printf("Transaction Operation from %s to %s for %s coins\n", argv[1], argv[2], argv[3]);
	}
	else
	{
		printf("Invalid number of arguements.");
		exit(0);
	}

	bzero(sendBuffer, MAXLINE);


	for(int i = 1;i < argc; i++)
	{
		char tempString[MAXLINE];
		strcpy(tempString,argv[i]);
		
		if(i != argc -1)
		{
			strcat(tempString, " ");
		}
		strcat(sendBuffer, tempString );


	}
	puts(sendBuffer);

	
	// socket create and verification
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
	{
		printf("socket creation failed...\n");
		
	}
	else
		printf("Socket successfully created..\n");
	bzero(&servaddr, sizeof(servaddr));

	// assign IP, PORT
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_port = htons(PORT);

	// connect the client socket to server socket
	if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
		printf("connection with the server failed...\n");
		exit(0);
	}
	else
		printf("connected to the server..\n");

	// function for chat


	func(sockfd, sendBuffer);

	// close the socket
	close(sockfd);
}
