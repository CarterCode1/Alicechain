#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#define MAX 80
#define TCP_PORT 8088
#define SA struct sockaddr
void func(int sockfd)
{
	char buff[MAX];
	int n;
	for (;;) {
		bzero(buff, sizeof(buff));
		printf("Enter the string : ");
		n = 0;
		while ((buff[n++] = getchar()) != '\n')
			;
		write(sockfd, buff, sizeof(buff));
		bzero(buff, sizeof(buff));
		read(sockfd, buff, sizeof(buff));
		printf("From Server : %s", buff);
		if ((strncmp(buff, "exit", 4)) == 0) {
			printf("Client Exit...\n");
			break;
		}
	}
}

int main()
{
	int tcp_sock_fd;
	struct sockaddr_in tcpServAddress;

	// socket create and verification
	tcp_sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (tcp_sock_fd == -1) {
		printf("socket creation failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully created..\n");
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
		printf("connected to the server..\n");

	// function for chat
	func(tcp_sock_fd);

	// close the socket
	close(tcp_sock_fd);
}
