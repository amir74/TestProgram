#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFSIZE 1024
int get_time(int sock, char * packet, uint length){
	struct timeval tv_tod, tv_ioctl;
	int error;
	gettimeofday(&tv_tod, 0);
	error = ioctl(sock, SIOCGSTAMP, &tv_ioctl);
	printf("gettimeofday: %d.%d\t ioctl: %d.%d\t delta: %d.%d\n",
	       tv_tod.tv_sec, tv_tod.tv_usec,
	       tv_ioctl.tv_sec, tv_ioctl.tv_usec,
	       tv_tod.tv_sec - tv_ioctl.tv_sec,
	       tv_tod.tv_usec - tv_ioctl.tv_usec);
	return 1;
}

int main(int argc, char *argv[]){
	
	if (argc != 2) {
		printf("Usage: %s <port> \n", argv[0]);
		exit(0);
	}
	
	
	
	char *ip = "127.0.0.1" ;
	int port = atoi(argv[1]);
	
	int sockfd;
	struct sockaddr_in server_addr;
	struct sockaddr_in remaddr;
	socklen_t addrlen = sizeof(remaddr);
	char buffer[1024];
	char timebuf[2048];
	socklen_t addr_size;
	int n;
	
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sockfd < 0){
		perror("[-]socket error");
		exit(1);
	}
	
	memset(&server_addr, '\0', sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = inet_addr(ip);
	
	n = bind(sockfd, (struct sockaddr*)&server_addr, 
		 sizeof(server_addr));
	if ( n < 0){
		perror("[-]bind error");
		exit(1);
	}
	
	for(;;){
		printf("waiting on port %d\n", port);
		int recvlen = recvfrom(sockfd, buffer, BUFSIZE, 0, (struct sockaddr *)&remaddr,
				       &addrlen);
		get_time(sockfd,timebuf, recvfrom);
		printf("received %d bytes\n", recvlen);
		if(recvlen > 0){
			buffer[recvlen] = 0;
			printf("received message: \"%s\"\n", buffer);
		}
	}
}