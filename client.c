//
//  client.c
//  SFSS
//
//  Created by Liushirley on 2018-01-10.
//  Copyright © 2018 Liushirley. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <assert.h>
#define portnumber 2222


int main(int argc, char* argv[]){
    struct sockaddr_in servaddr;
    int error;
    int sockfd;
    char buffer[1024];
    int n;
//creat a client socket
    sockfd=socket(AF_INET,SOCK_STREAM,0);
    if(sockfd==-1){
        perror("Error:");
        exit(1);
    }
//get the hostname by calling hostname
    struct hostent *host;
    host=gethostbyname(argv[1]);
    printf("continue...\n");
//fulfill sock infromation of the client
    bzero(&servaddr, sizeof(struct sockaddr_in));
    servaddr.sin_family=AF_INET;
    servaddr.sin_port=htons(portnumber);
    servaddr.sin_addr=*((struct in_addr*)host->h_addr);
    printf("continue...\n");
//In TCP connect initiates a three-way handshake. connect returns only thwn the connection is established or when an error occurs.
    int flag;
    flag=connect(sockfd,(struct sockaddr*)(&servaddr), sizeof(struct sockaddr));
    assert(flag!=-1);
    printf("connect function call return value is %d\n", flag);
    if(flag==-1){
        perror("Error: ");
        exit(1);
    }else{
        printf("Connected to server...\n");
        //write to the server
        fputs("Please enter a string: \n",stdout);
        fgets(buffer, 1024, stdin);
        write(sockfd, buffer,strlen(buffer));
        sleep(2);
        printf("wake up for reading operation, ready to read from server...\n");
        memset(buffer,0, sizeof(buffer));
        n=read(sockfd, buffer, 1024);
        buffer[n]='\0';
        printf("Content read from server: %s", buffer);

        close(sockfd);
        exit(0);
    }
}

