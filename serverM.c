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


int numberOfTransactions = 0;

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
	char buffer[MAXLINE];

	int length;
	struct sockaddr_in servaddr;
	fd_set readFDs;
	
	bool isContinue = true;
	bool socketOption = TRUE;
	int maxClients = 3;
	int clientSockets[3];
	int clientsConnected = 0;
	struct sockaddr_in client_addr[3];
	int clientLen[3];
	int UDPsocket, maxSocket, sd;
	int activity;
	int n;

	int transactionId = -1;


	printf("Server \n");
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
	printf("Listener on port %d \n", PORT);

	length = sizeof(servaddr);

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
			
			printf("%s\n",buffer);
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
	int sockFD_A, sockFD_B;
	int connFD_A, connFD_B;
	int lenA, lenB;
	int numTokens = 0;
	int currentBalance = 0;
	int selectResult;

	bool validCommand = true;
	const char seperator[2] = " ";
	int cycles = 0;
	struct sockaddr_in servaddrA, cli_A;
	struct sockaddr_in servaddrB, cli_B;
	fd_set tcp_read_fds;

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
	else
		printf("Server A listening..\n");

	lenA = sizeof(cli_A);

		// Now server B is ready to listen and verification
	if ((listen(sockFD_B, 5)) != 0) {
		printf("Listen failed...\n");
		exit(0);
	}
	else
		printf("Server B listening..\n");

	lenB = sizeof(cli_B);

//----------------------- Server logic loop for handling incoming requests
	for( ; ; )
	{
		FD_ZERO(&tcp_read_fds);
		FD_SET(sockFD_A, &tcp_read_fds);
		FD_SET(sockFD_B, &tcp_read_fds);
			
		selectResult = select(sockFD_B + 1,&tcp_read_fds, , NULL , NULL , NULL);

	   	if(selectResult<0)
		{
			printf("select() failed\n");
		}

		if(selectResult > 0 ) {

		// Accept the data packet from client and verification
	connFD_A = accept(sockFD_A, (SA*)&cli_A, &lenA);
	if (connFD_A < 0) {
		printf("server accept failed...\n");
		exit(0);
	}
	else
		printf("server accept the client...\n");



		numTokens = 0;
		currentBalance = 0;
		//seed our random number. Well use this to choose between server 1-3
		srand(time(0));

		bzero(buffer, MAXLINE);

		read(connFD_A, buffer, sizeof(buffer));

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
			char strTransId[MAXLINE];
			sprintf(strTransId, "%d", Id);
			strcat(strTransId , seperator);
			strcat(strTransId , buffer);
			bzero(buffer, MAXLINE);
			strcpy(buffer, strTransId);

			Transaction tempTrans = parseTransactionString(buffer);

			if ( !isAccountValid(transactions, numberOfTransactions, tempTrans.sendName )) {
				validCommand = false;
				printf("sender invalid!\n");
			}

			if ( !isAccountValid(transactions, numberOfTransactions, tempTrans.recvName )) {
				validCommand = false;
				printf("receiver invalid!\n");
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
				 bool isChkWalletFound = isAccountValid(transactions, numberOfTransactions, buffer );

				if(isChkWalletFound)
				{
					validCommand = true;
					int balance = calcTransBalance(transactions, numberOfTransactions, buffer );
					printf("%s has a current balance of: %d\n", buffer, balance);
				}
				else
				{
					validCommand = false;
					printf("not found\n");
				}
			}
		}

		printf("command: %s\n", buffer);	
		//printTransactions(transactions, numberOfTransactions );

		if(validCommand) {

			int serverNumber = (rand() % 3) ;
			serverNumber = 2;

			sendto(UDPsocket, (const char *)buffer, MAXLINE, 
			MSG_CONFIRM, (const struct sockaddr *) &client_addr[serverNumber],
				clientLen[serverNumber]);

			bzero(buffer, MAXLINE);
			n = recvfrom(UDPsocket, buffer, MAXLINE, MSG_WAITALL, (struct sockaddr*)&client_addr[serverNumber], &clientLen[serverNumber]);

			buffer[n] = '\0';
			printf("buffer %s\n",buffer );
			write(connFD_A, buffer, MAXLINE);
		}
		cycles++;
	}




	/*
	printTransactions(transactions, numberOfTransactions );
	printf("-----------\n");

	SortTransactions(transactions, numberOfTransactions );
	printTransactions(transactions, numberOfTransactions );
	int balance = calcTransBalance(transactions, numberOfTransactions, "Chinmay" );

	if(isAccountValid(transactions, numberOfTransactions, "Chinmay" ))
	{
		printf("True\n");
	}

	if(isAccountValid(transactions, numberOfTransactions, "Greg" ))
	{
		printf("True\n");
	}

	printf("Balance: %d\n", balance );

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
	*/

	close(UDPsocket);
	close(sockFD_A);


	return 0;
}
