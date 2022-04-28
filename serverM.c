// Client side implementation of UDP client-server model
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <time.h>

#define PORT 24494
#define  TCP_PORT_A 25494
#define  TCP_PORT_B 26494
#define MAXLINE 1024
#define MAX_TRANSACTIONS 256
#define LOAD_DATA "LOAD_DATA"
#define LOAD_DATA_ACK "LOAD_DATA_ACK"
#define TXLIST "TXLIST"
#define SA struct sockaddr
#define TRUE 1

typedef struct  {
	int transID;
	char sendName[50];
	char recvName[50];
	int amount;
} Transaction ;




void printTransactions(Transaction *transactions, int size)
{
	if(size >0)
	{
		for(int i = 0; i<size; i++)
		{
			printf("%d : %s : %s : %d \n", transactions[i].transID, transactions[i].sendName, transactions[i].recvName, transactions[i].amount );
		}
	}
}

void sortTransactions(Transaction *transactions, int size)
{
    int i, j;
    Transaction temp;
    
    for (i = 0; i < size - 1; i++)
    {
        for (j = 0; j < (size - 1-i); j++)
        {
            if (transactions[j].transID > transactions[j + 1].transID)
            {
                temp = transactions[j];
                transactions[j] = transactions[j + 1];
                transactions[j + 1] = temp;
            } 
        }
    }
}

Transaction parseTransactionString(char * transString)
{
	Transaction newTransaction;
	char parseBuffer[MAXLINE];
	const char seperator[2] = " ";
	int i = 0;

	int transID;
	char sendName[50];
	char recvName[50];
	double amount;
	
	strcpy(parseBuffer, transString);
	char* token = strtok(parseBuffer, seperator);

	while(token != NULL )
	{
		if(i ==0){
			transID = atoi(token);
		}
		if(i==1){
			strcpy(sendName,  token);
		}
		if(i==2){
			strcpy(recvName,  token);
		}
		if(i==3){
			amount = atoi(token);
		}
		i++;
		token = strtok(NULL, seperator );
	}
	newTransaction.transID = transID;
	strcpy(newTransaction.sendName, sendName );
	strcpy(newTransaction.recvName, recvName );
	newTransaction.amount = amount;

	return newTransaction;
}

int addTransaction(Transaction *transaction, int size, Transaction newTrans )
{
	if(size < MAX_TRANSACTIONS)
	{
		transaction[size] = newTrans;
		size++;
	}
	return size;
}

int calcTransBalance(Transaction *transactions, int size, char* accountName )
{
	int balance = 0;
	int initialBalance = 1000;

	if(size > 0)
	{
		for(int i = 0; i<size; i++)
		{
			if(strcmp(transactions[i].recvName, accountName) == 0)
			{
				balance += transactions[i].amount;
			}
			if(strcmp(transactions[i].sendName, accountName) == 0)
			{
				balance -= transactions[i].amount;
			}
		}
	}
	return initialBalance + balance;
}

bool isAccountValid(Transaction *transactions, int size, char* accountName )
{
	bool isValid = false;

	if(size > 0)
	{
		for(int i = 0; i<size; i++)
		{
			if((strcmp(transactions[i].sendName, accountName) == 0) ||
				 (strcmp(transactions[i].recvName, accountName) == 0))
			{
				isValid = true;
			}
		}
	}
	return isValid;
}

int getNextTransId(Transaction *transactions, int size)
{
	 int transId = 0;
	 if(size > 0)
	 {
		 transId =  transactions[size-1].transID +1;
	 }
	return transId;
}

// Driver code
int main() {

	Transaction transactions[MAX_TRANSACTIONS];
	int numberOfTransactions = 0;

	char buffer[MAXLINE];
	char messageBuffer[MAXLINE];
	struct sockaddr_in servaddr;
	fd_set readFDs;
	
	bool socketOption = TRUE;
	bool isContinue = true;
	int maxClients = 3;
	int clientSockets[3];
	int clientsConnected = 0;
	struct sockaddr_in client_addr[3];
	int clientLen[3];
	int UDPsocket, maxSocket, sd;
	int activity;
	int n;

	printf("The main server is up and running.\n");
	//initialise all client_socket[] to 0 so not checked
    for (int i = 0; i < maxClients; i++) 
    {
        clientSockets[i] = 0;
		memset(&client_addr[i], 0, sizeof(client_addr[i]));
		clientLen[i] = sizeof(client_addr[i]);
    }
    // Creating socket file descriptor
    if ( (UDPsocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket  to server 1 creation failed");
        exit(EXIT_FAILURE);
    }
   
    memset(&servaddr, 0, sizeof(servaddr));
	
	//set master socket to allow multiple connections 
    //if( setsockopt(UDPsocket, SOL_SOCKET, SO_REUSEADDR, (char *)&socketOption, sizeof(socketOption)) < 0 )
    //{
    //   perror("setsockopt");
    //    exit(EXIT_FAILURE);
    //}
	
       
    // Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = INADDR_ANY;

	//bind the socket to localhost port 24494
    if (bind(UDPsocket, (struct sockaddr *)&servaddr, sizeof(servaddr))<0) 
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

	while(clientsConnected < maxClients)
	{	
		//clear socket set and add UDP socket
		FD_ZERO(&readFDs);
		FD_SET(UDPsocket, &readFDs );
		maxSocket = UDPsocket;

		for(int i =0; i < maxClients; i++)
		{
			sd = clientSockets[i];
			if(sd > 0) {
				FD_SET(sd,&readFDs);
			}
			//update maximum FD value
			if(sd > maxSocket) {
				maxSocket = sd;
			}
		}

		activity = select( maxSocket + 1 , &readFDs , NULL , NULL , NULL);
		if ((activity < 0) && (errno!=EINTR)) 
		{
			printf("select error");
		}

		if (FD_ISSET(UDPsocket, &readFDs)) {
			n = recvfrom(UDPsocket, buffer, MAXLINE, MSG_WAITALL, (struct sockaddr*)&client_addr[clientsConnected], &clientLen[clientsConnected]);

			buffer[n] = '\0';
			
			if(strcmp(LOAD_DATA, buffer) == 0 ){
				isContinue = true;
				while(isContinue)
				{
					bzero(buffer, MAXLINE);
					n = recvfrom(UDPsocket, buffer,MAXLINE, MSG_WAITALL, (struct sockaddr*)&client_addr[clientsConnected], &clientLen[clientsConnected]);
					buffer[n] = '\0';

					if(strcmp( LOAD_DATA_ACK, buffer) == 0 ){
						isContinue = false;
					}
					else {
						Transaction newTrans = parseTransactionString(buffer);
						numberOfTransactions = addTransaction(transactions, numberOfTransactions, newTrans);
					}
				}
			}
			clientsConnected++;
		}
	}

	sortTransactions(transactions, numberOfTransactions );

	//-----TCP Server Vars--------------------------------
	int sockFD_A, sockFD_B, maxSD;
	int connFD_A, connFD_B;
	int valueRead;
	int lenA, lenB;
	int numTokens = 0;
	int currentBalance = 0;
	int selectResult;

	bool validCommand = true;
	bool isServerA = true;
	const char seperator[2] = " ";
	struct sockaddr_in servaddrA, cli_A;
	struct sockaddr_in servaddrB, cli_B;
	int connected_Tcp_Port;
	fd_set tcp_read_fds;
	Transaction tempTrans;

	// TCP create socket  A 
	sockFD_A = socket(AF_INET, SOCK_STREAM, 0);
	if (sockFD_A == -1) {
		printf("socket A creation failed...\n");
		exit(0);
	}
	// TCP create socket B
	sockFD_B = socket(AF_INET, SOCK_STREAM, 0);
	if (sockFD_B == -1) {
		printf("socket B creation failed...\n");
		exit(0);
	}

	bzero(&servaddrA, sizeof(servaddrA));
	bzero(&servaddrB, sizeof(servaddrB));

	// assign IP, PORT
	servaddrA.sin_family = AF_INET;
	servaddrA.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddrA.sin_port = htons(TCP_PORT_A);

	servaddrB.sin_family = AF_INET;
	servaddrB.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddrB.sin_port = htons(TCP_PORT_B);


	// Binding newly created socket to given IP and verification
	if ((bind(sockFD_A, (SA*)&servaddrA, sizeof(servaddrA))) != 0) {
		printf("socket A bind failed...\n");
		exit(0);
	}
	if ((bind(sockFD_B, (SA*)&servaddrB, sizeof(servaddrB))) != 0) {
		printf("socket B bind failed...\n");
		exit(0);
	}

	// Now server A is ready to listen and verification
	if ((listen(sockFD_A, 5)) != 0) {
		printf("Listen failed...\n");
		exit(0);
	}

	lenA = sizeof(cli_A);

		// Now server B is ready to listen and verification
	if ((listen(sockFD_B, 5)) != 0) {
		printf("Listen failed...\n");
		exit(0);
	}

	lenB = sizeof(cli_B);

//----------------------- Server logic loop for handling incoming requests
	for( ; ; )
	{
		//reset loop values
		numTokens = 0;
		currentBalance = 0;
		bzero(buffer, MAXLINE);
		bzero(messageBuffer, MAXLINE);
		connected_Tcp_Port = 0;
		validCommand = true;

		FD_ZERO(&tcp_read_fds);
		FD_SET(sockFD_A, &tcp_read_fds);
		FD_SET(sockFD_B, &tcp_read_fds);
		//determine the max socket value between A & B
		maxSD = (sockFD_A > sockFD_B) ? sockFD_A : sockFD_B;

		selectResult = select(sockFD_B + 1 ,&tcp_read_fds, NULL , NULL , NULL);

	   	if(selectResult<0)
		{
			printf("select() failed\n");
		}

		if(FD_ISSET(sockFD_A, &tcp_read_fds))
		{
			connFD_A = accept(sockFD_A, (SA*)&servaddrA, &lenA);
			if (connFD_A < 0) {
				printf("server accept failed...\n");
				exit(0);
			}
			isServerA = true;

			
			if(( valueRead = read(connFD_A, buffer, MAXLINE)) == 0 )
			{
				printf("connection A disconnected\n");
				close(sockFD_A);
				break;
			}

			getsockname(connFD_A , (struct sockaddr*)&servaddrA , (socklen_t*)&lenA);
			connected_Tcp_Port = ntohs(servaddrA.sin_port);
		}

		if(FD_ISSET(sockFD_B, &tcp_read_fds))
		{
			connFD_B = accept(sockFD_B, (SA*)&servaddrB, &lenB);
			if (connFD_B < 0) {
				printf("server accept failed...\n");
				exit(0);
			}

			isServerA = false;

			if(( valueRead = read(connFD_B, buffer, MAXLINE)) == 0 )
			{
				printf("connection B disconnected\n");
				close(sockFD_B);
				break;
			}

			getsockname(connFD_B , (struct sockaddr*)&servaddrB , (socklen_t*)&lenB);
			connected_Tcp_Port = ntohs(servaddrB.sin_port);
		}

		printf("buffer: %s \n", buffer);
		
		//seed our random number. Well use this to choose between server 1-3
		srand(time(0));

		char parseBuffer[MAXLINE];
		strcpy(parseBuffer, buffer);
		
		char* token = strtok(parseBuffer, seperator);
		
		while(token != NULL)
		{
			token = strtok(NULL, seperator );
			numTokens++;
		}

		//This signals a TXCOINS operation.  Add a transaction Id to beginning of tthe buffer string 
		if(numTokens == 3)
		{
			int Id = getNextTransId(transactions, numberOfTransactions );
			char tempBuffer[MAXLINE];
	
			strcpy(tempBuffer, buffer);
			sprintf(buffer, "%d %s", Id, tempBuffer);
			tempTrans = parseTransactionString(buffer);
			sprintf(messageBuffer,"The main server received from \"%s\" to transfer %d coins to \"%s\" using TCP over port %d.\n", tempTrans.sendName, tempTrans.amount, tempTrans.recvName, connected_Tcp_Port);
			printf("%s",messageBuffer );

			if ( !isAccountValid(transactions, numberOfTransactions, tempTrans.sendName )) {
				validCommand = false;
				bzero(messageBuffer, MAXLINE);
				sprintf(messageBuffer, "Unable to proceed with the transaction as %s is not part of the network.", tempTrans.sendName );
				printf("%s",messageBuffer );
			}

			if ( !isAccountValid(transactions, numberOfTransactions, tempTrans.recvName )) {
				validCommand = false;
				bzero(messageBuffer, MAXLINE);
				sprintf(messageBuffer, "Unable to proceed with the transaction as %s is not part of the network.", tempTrans.recvName );
				printf("%s",messageBuffer );
			}

			if (( !isAccountValid(transactions, numberOfTransactions, tempTrans.recvName )) 
				&& ( !isAccountValid(transactions, numberOfTransactions, tempTrans.sendName )))
				{
					validCommand = false;
					bzero(messageBuffer, MAXLINE);
					sprintf(messageBuffer, "Unable to proceed with the transaction as %s and %s are not part of the network.", tempTrans.sendName, tempTrans.recvName );
					printf("%s",messageBuffer );
				}

			currentBalance = calcTransBalance(transactions, numberOfTransactions,tempTrans.sendName );

			if( currentBalance - tempTrans.amount  < 0) {
				validCommand = false;
				printf("insufficient balance!\n");
			}

			if(validCommand) {
				numberOfTransactions = addTransaction(transactions, numberOfTransactions, tempTrans);
				currentBalance = currentBalance - tempTrans.amount;
			}
		}

		if(numTokens == 1)
		{
			printf("onetoken\n");
			if(strcmp(TXLIST, buffer) == 0 )
			{
				printf("need to write to TXLIST functionality.\n");
			}
			else
			{
				sprintf(messageBuffer,"The main server received input= %s from client using TCP over port %d.\n", buffer, connected_Tcp_Port);
				printf("%s",messageBuffer );
				bool isChkWalletFound = isAccountValid(transactions, numberOfTransactions, buffer );

				if(isChkWalletFound)
				{
					validCommand = true;
					int balance = calcTransBalance(transactions, numberOfTransactions, buffer );
					bzero(messageBuffer, MAXLINE);
					sprintf(messageBuffer, "The current balance of %s is : %d alicoins.\n", buffer, balance);
				}
				else
				{
					validCommand = false;
					printf("not found\n");
				}
			}
			//If the command has been validate, we need to send to one of the 3 servers to write to the blockchain
			if(validCommand)
			{
	
				int serverNumber = (rand() % 3) ;
				serverNumber = 2;

				printf("The main server sent a request to server %d.\n", serverNumber);
				sendto(UDPsocket, (const char *)buffer, MAXLINE, 
				MSG_CONFIRM, (const struct sockaddr *) &client_addr[serverNumber],
					clientLen[serverNumber]);

				bzero(buffer, MAXLINE);
				n = recvfrom(UDPsocket, buffer, MAXLINE, MSG_WAITALL, (struct sockaddr*)&client_addr[serverNumber], &clientLen[serverNumber]);

				buffer[n] = '\0';
				printf("buffer %s\n",buffer );

			}
		}

		printf("command: %s\n", buffer);	
		printTransactions(transactions, numberOfTransactions );

		if(validCommand) {
	
			int serverNumber = (rand() % 3) ;
			serverNumber = 2;

			printf("The main server sent a request to server %d.\n", serverNumber);
			sendto(UDPsocket, (const char *)buffer, MAXLINE, 
			MSG_CONFIRM, (const struct sockaddr *) &client_addr[serverNumber],
				clientLen[serverNumber]);

			bzero(buffer, MAXLINE);
			n = recvfrom(UDPsocket, buffer, MAXLINE, MSG_WAITALL, (struct sockaddr*)&client_addr[serverNumber], &clientLen[serverNumber]);

			buffer[n] = '\0';
			printf("buffer %s\n",buffer );
		}

		printf(" Command was %s\n", validCommand ? "true" : "false");

		if(isServerA) {
			write(connFD_A, buffer, MAXLINE);
		}
		else {
			write(connFD_B, buffer, MAXLINE);
		}
	}

	close(UDPsocket);
	close(sockFD_A);
	close(sockFD_B);


	return 0;

}

