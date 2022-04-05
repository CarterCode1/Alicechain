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

#define SERVER_PORT	 24494
#define PORT	23494
#define MAXLINE 1024
#define LOAD_DATA "LOAD_DATA"
#define LOAD_DATA_ACK "LOAD_DATA_ACK"

// Driver code
int main() {
	
	bool isContinue = true;
	int sockfd;
	char buffer[MAXLINE];
	int maxTransId = -1;
	char *filename = "block3.txt";
	const char seperator[2] = " ";
	char transaction[MAXLINE];
	int len, n;

	struct sockaddr_in servaddr, clientaddr;
	
	// Creating socket file descriptor
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}
	
	memset(&servaddr, 0, sizeof(servaddr));
	memset(&clientaddr, 0, sizeof(clientaddr));
	
	// Filling server information
	servaddr.sin_family = AF_INET; // IPv4
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(SERVER_PORT);


	// Filling client information
	clientaddr.sin_family = AF_INET;
	clientaddr.sin_port = htons(PORT);
	clientaddr.sin_addr.s_addr = INADDR_ANY;

	// Bind the socket with the client address
	if ( bind(sockfd, (const struct sockaddr *)&clientaddr,
			sizeof(clientaddr)) < 0 )
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

	len = sizeof(servaddr); //len is value/resuslt
	
		
	sendto(sockfd, (const char *)LOAD_DATA, strlen(LOAD_DATA),
		MSG_CONFIRM, (const struct sockaddr *) &servaddr,
			sizeof(servaddr));
	printf("Hello message sent.\n");	


	FILE *fp = fopen(filename, "r");
	if (fp == NULL)
	{
		printf("Error: could not open file %s", filename);
		return 1;
	}


	while (fgets(transaction, MAXLINE, fp))
	{
		//Send each transaction to middle server
		printf("%s", transaction );

		sendto(sockfd, (const char *)transaction, strlen(transaction),
			MSG_CONFIRM, (const struct sockaddr *) &servaddr, len);
		
		bzero(transaction, MAXLINE);
	}

	printf("Exit: %s\n", LOAD_DATA_ACK );
	sendto(sockfd, (const char *)LOAD_DATA_ACK, strlen(LOAD_DATA_ACK),
		MSG_CONFIRM, (const struct sockaddr *) &servaddr, len);

	fclose(fp);


		/*
		char parseBuffer[MAXLINE];
		strcpy(parseBuffer, buffer);
		int numTokens = 0;
		char* token = strtok(parseBuffer, seperator);
		
		while(token != NULL)
		{
			token = strtok(NULL, seperator );
			numTokens++;
		}


		if(numTokens == 4)
		{
			FILE *fp = fopen(filename, "a");
			if (fp == NULL)
   			{
        		printf("Error: could not open file %s", filename);
        		return 1;
    		}

			printf("Writing Trans : %s\n", buffer);
			fputs(buffer,fp);
			fputs("\n", fp);
			fclose(fp);
	
			printf("Writing Trans : %s\n", buffer);
		}



		printf("Num of tokens %d\n", numTokens );






		if (strncmp("exit", buffer, 4) == 0) {
			isContinue = false;
			printf("Server Exit...\n");
			break;
		}
		
		else 
		{
			printf("Outgoing Message : %s\n", buffer);
			sendto(sockfd, (const char *)buffer, strlen(buffer),
				MSG_CONFIRM, (const struct sockaddr *) &cliaddr,
					len);
			printf("Outgoing Message2 : %s\n", buffer);
		} 
		
	}
	*/
	close(sockfd);
	
	return 0;
}
