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


struct Transaction {
	int transID;
	char name[50];
	double amount;
}

struct Transaction transactions[256]


// Driver code
int main() {
	
	bool isContinue = true;
	int transactionId = -1;
	int sockfd1;
	char buffer[MAXLINE];

	struct sockaddr_in	 serv1addr;
	//----Setup for UDP client to server 1----------------------------------------------------
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
    //----Setup for UDP client to server 1----------------------------------------------------

	// Get transaction max id
	char *loadTransData = "LoadTransData";
	char maxTransBuffer1[MAXLINE];
	char maxTransBuffer2[MAXLINE];
	char maxTransBuffer3[MAXLINE];
	char *ptr1, *ptr2, *ptr3;
	int max1, max2, max3;
	int length1, length2, length3;
	int len1;

	sendto(sockfd1, (const char *)maxTransactionReq, strlen(maxTransactionReq), 
		MSG_CONFIRM, (const struct serv1addr *) &serv1addr,
		sizeof(serv1addr));

	bzero(maxTransBuffer1, MAXLINE);
	length1 = recvfrom(sockfd1, (char *)maxTransBuffer1, MAXLINE,
				MSG_WAITALL, (struct serv1addr *) &serv1addr,
				&len1);

	maxTransBuffer1[length1] = '\0';
	max1 = strtol(maxTransBuffer1, &ptr1 , 10);
	printf("Max 1: %d\n", max1);
	transactionId = max1;



	/*
	//set up the  tcp socket to receive message
	int sockfdA, connfdA, lenA;
    struct sockaddr_in servaddrA, cliA;
	// socket create and verification
	sockfdA = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfdA == -1) {
		printf("socket creation failed...\n");
		exit(0);
	}
	else
	{
		printf("Socket successfully created..\n");
	}

	bzero(&servaddrA, sizeof(servaddrA));
	
	// assign IP, PORT
	servaddrA.sin_family = AF_INET;
	servaddrA.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddrA.sin_port = htons(TCP_PORT);

		// Binding newly created socket to given IP and verification
	if ((bind(sockfdA, (SA*)&servaddrA, sizeof(servaddrA))) != 0) {
		printf("socket bind failed...\n");
		exit(0);
	}
	else
	{
		printf("Socket successfully binded..\n");
	}

	// Now server is ready to listen and verification
	if ((listen(sockfdA, 5)) != 0) {
		printf("Listen failed...\n");
		exit(0);
	}
	else
	{
		printf("Server listening..\n");
	}
	lenA = sizeof(cliA);



	// Accept the data packet from client and verification
	connfdA = accept(sockfdA, (SA*)&cliA, &lenA);
	if (connfdA < 0) {
		printf("server accept failed...\n");
		exit(0);
	}
	else
	{
		printf("server accept the client...\n");
	}

	// infinite loop for sending and receiving messages
	while(isContinue)
	{
		int numTokens = 0;
		int testInt = 0;
		const char seperator[2] = " ";

		bzero(buffer, MAXLINE);

		// read the message from client and copy it in buffer
		testInt = read(connfdA, buffer, sizeof(buffer));
		// print buffer which contains the client contents
		// printf("From client: %s \n", buffer);

		int m, len;


		char parseBuffer[MAXLINE];
		strcpy(parseBuffer, buffer);
		
		char* token = strtok(parseBuffer, seperator);
		
		while(token != NULL)
		{
			token = strtok(NULL, seperator );
			numTokens++;
		}

		printf("Value Check : %s : %s \n", buffer, parseBuffer);
		printf("Num of tokens %d\n", numTokens );

		//This signals a TXCOINS operation.  Add a transaction Id to beginning of tthe buffer string 
		if(numTokens == 3)
		{
			char strTransId[MAXLINE];
			sprintf(strTransId, "%d", ++transactionId);
			strcat(strTransId , seperator);
			strcat(strTransId , buffer);
			bzero(buffer, MAXLINE);
			strcpy(buffer, strTransId);
		}

	 	printf("Add TransactionId: %s \n", buffer);


		sendto(sockfd1, (const char *)buffer, strlen(buffer),
			MSG_CONFIRM, (const struct serv1addr *) &serv1addr,
			sizeof(serv1addr));
		
		printf("Message sent to server: %s \n", buffer);
		bzero(buffer, MAXLINE);
		
		m = recvfrom(sockfd1, (char *)buffer, MAXLINE,
				MSG_WAITALL, (struct serv1addr *) &serv1addr,
				&len);
				
		buffer[m] = '\0';
		printf("From Server : %s\n", buffer);




		// and send that buffer to client
		write(connfdA, buffer, sizeof(buffer));

		printf("Message sent to client : %s\n", buffer);
		// if msg contains "Exit" then server exit and chat ended.
		if (strncmp("exit", buffer, 4) == 0) {
			isContinue = false;
			printf("Server Exit ...\n");
			break;
		}
	}

	close(sockfdA);
	*/
	close(sockfd1);
	return 0;
}
