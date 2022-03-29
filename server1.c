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

#define PORT	 8080
#define MAXLINE 1024

// Driver code
int main() {
	
	bool isContinue = true;
	int sockfd;
	char buffer[MAXLINE];
	char *loadTransData = "LoadTransData";
	int maxTransId = -1;
	char *filename = "block1.txt";


	struct sockaddr_in servaddr, cliaddr;
	
	// Creating socket file descriptor
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		perror("socket creation failed");
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


	
	int len, n;
	int loopControl = 0;
	len = sizeof(cliaddr); //len is value/resuslt
	
	while(loopControl < 5)
	{
		const char seperator[2] = " ";

		n = recvfrom(sockfd, (char *)buffer, MAXLINE,
					MSG_WAITALL, ( struct sockaddr *) &cliaddr,
					&len);
		buffer[n] = '\0';
		printf("Incoming Message : %s\n", buffer);
		
		
		// Request from ServerM to load initial data.
		if(strcmp( loadTransDataloadTransData, buffer) == 0 )
		{
			FILE *fp = fopen(filename, "r");
			if (fp == NULL)
   			{
        		printf("Error: could not open file %s", filename);
        		return 1;
    		}

			char transaction[MAXLINE];
			char *token;
			char strMaxTransId[10];

			while (fgets(transaction, MAXLINE, fp))
			{
				token = strtok(transaction, seperator);
				int tokenInt = atoi(token);
				if(tokenInt > maxTransId)
				{
					maxTransId = tokenInt;
				}
			}
			sprintf(strMaxTransId, "%d", maxTransId);

			sendto(sockfd, (const char *)strMaxTransId, strlen(strMaxTransId),
			MSG_CONFIRM, (const struct sockaddr *) &cliaddr,
				len);
			bzero(strMaxTransId, 10);
			bzero(buffer, MAXLINE);
			printf("Response message sent: %s.\n", strMaxTransId);
			fclose(fp);

		}
		
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
	
	close(sockfd);
	
	return 0;
}
