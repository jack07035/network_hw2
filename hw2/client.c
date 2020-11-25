
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>


const unsigned long port = 8080;

int main(void)
{
	//創建socket
	int server_socket;
	server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	//初始化IP為127.0.0.1，端口為已設定的port
	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	//連接server
	connect(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));


	int flag = 1;
	while(flag)
	{
		 fd_set reads;
        FD_ZERO(&reads);
        FD_SET(server_socket, &reads);
#if !defined(_WIN32)
        FD_SET(0, &reads);
#endif

		struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 100000;

		if (select(server_socket+1, &reads, 0, 0, &timeout) < 0) {
            //fprintf(stderr, "select() failed. (%d)\n", GETSOCKETERRNO());
            return 1;
        }

		if (FD_ISSET(server_socket, &reads)) {
            char read[4096];
			memset(read,'\0',sizeof(read));
            int bytes_received = recv(server_socket, read, 4096, 0);
            if (bytes_received < 1) {
                printf("Connection closed by peer.\n");
                break;
            }
			if(strcmp(read,"quit\n")==0)
			{
				close(server_socket);
			}
			else
			{
				printf("\n%s\n", read);
			}
        }

		if(FD_ISSET(0, &reads)) {
            char read[4096];
			memset(read,'\0',sizeof(read));
            if (!fgets(read, 4096, stdin)) break;
			if(read[strlen(read)-1]=='\n')read[strlen(read)-1]='\0';
            int bytes_sent = send(server_socket, read, strlen(read), 0);
        }
	}
	close(server_socket);
	return 0;
}