//
//  Server.c
//  SFSS
//
//  Created by Liushirley on 2018-01-10.
//  Copyright © 2018 Liushirley. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#define portnumber 2222
#include <assert.h>



int main(){
//create a socket, returns a non-negative socket descriptor
//int socket(int family, int type, int protocol);
    int sockfd;
    int new_fd;
    int n;
    char buffer[1024];
    int a=AF_INET;
    int b=SOCK_STREAM;
    sockfd=socket(a, b, 0);
    if(sockfd==-1){
        perror("Error: ");
        exit(1);
    }
    printf("Start ...\n");
//bind ther server info to this socket, this assigns a local protocol address to a socket, return 0 for succeeding
//int bind(int sockfd, const struct sockaddr *servaddr, socklen_t addrlen);
    int error;
    struct sockaddr_in servaddr;
    struct sockaddr_in clieaddr;
//fulfil information of server
    //servaddr.sin_len=0;
    bzero(&servaddr, sizeof(struct sockaddr_in));
    servaddr.sin_family=AF_INET;
    servaddr.sin_port=htons(portnumber);
    servaddr.sin_addr.s_addr=htonl(INADDR_ANY);//INADDR_ANY represents the host can be any IP addres
    error=bind(sockfd,(struct sockaddr*)(&servaddr),sizeof(struct sockaddr));
    if(error==-1){
        perror("Error: ");
        exit(1);
    }
     printf("Start ...\n");
//Listen function converts an unconneted socket into a passive socket, indicating that the kernel should accept incoming connection requests directed to this socket
//int listen(int sockfd, int backlog)
    int backlog=5;// this is the maxium number tat the kernel should queue for this socket;
    error=listen(sockfd,backlog);
    //assert(error>-1);
    if(error==-1){
        perror("Error: ");
        exit(1);
    }
    printf("Start listening client...\n");
//accept is used to retrieve a connect request and convert that into a request
//int accept(int sockfd, struct sockaddr *cliaddr, socklen_t *addrlen);
    while(1){
        int size=sizeof(struct sockaddr_in);
        new_fd=accept(sockfd, (struct sockaddr*)(&clieaddr),&size);
        //new_fd=accept(sockfd,NULL,NULL);
        if(new_fd==-1){
            perror("Error: ");
            exit(1);
        }
        printf("Server is connected...\n");
        //read from client
        n=read(new_fd,buffer,1024);//if n is inside a parentness, if will not be given the value.
        if(n==-1){
            perror("Error:");
            exit(1);
        }
        buffer[n]='\0';
        printf("Server received %s\n", buffer);
        //write to client
        memset(buffer,0,sizeof(buffer));
        fputs("Please enter string going to client: \n",stdout);
        fgets(buffer,1024,stdin);
        n=write(new_fd,buffer,1024);
        if(n==-1){
            perror("Error\n");
            exit(1);
        }
        close(new_fd);
    }
    close(sockfd);
    return 0;
}


