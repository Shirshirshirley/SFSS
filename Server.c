
//  Created by Liushirley on 2018-01-10.
//  Copyright © 2018 Liushirley. All rights reserved.
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
//the following libraries are used for opensslencry
#include<fcntl.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
void InitializeSSL(){
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
}
    typedef struct FilePackage{
        char cmd;   //operation /command
        int filesize;
        int ack;     //flag
        char usrname[50];    //clientname
        char filename[125];   //filename
        char buf[1024];     //filecontent
    };
int main(){
//create a socket, returns a non-negative socket descriptor
//int socket(int family, int type, int protocol);
    int sockfd, new_fd,n, error;
    char buffer[1024];
    struct sockaddr_in servaddr;
    struct sockaddr_in clieaddr;
    SSL_CTX *sslctx;
    SSL *cSSL;

    InitializeSSL();
    sslctx=SSL_CTX_new(SSLv23_server_method());
    //SSL_CTX_set_options(sslctx,SSL_OP_SINGLE_DH_USE);
    int use_cert=SSL_CTX_use_certificate_file(sslctx,"cert.pem",SSL_FILETYPE_PEM);
    //assert(use_cert!=NULL);
    int use_prv=SSL_CTX_use_PrivateKey_file(sslctx,"key.pem", SSL_FILETYPE_PEM);
    if(use_prv<=0){
        perror("Error");}
    //verify privatekey
    if(!SSL_CTX_check_private_key(sslctx)){
        perror("Error \n");
        exit(1);}
    sockfd=socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd==-1){
        perror("Error: ");
        exit(1);
    }
    printf("Start ...\n");
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

    int backlog=5;// this is the maxium number tat the kernel should queue for this socket;
    error=listen(sockfd,backlog);
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
        cSSL=SSL_new(sslctx);
        SSL_set_fd(cSSL, new_fd);
        int ssl_err=SSL_accept(cSSL);
        if(ssl_err <0){
            exit(1);
        }
        if(new_fd==-1){
            perror("Error: ");
            exit(1);
        }
        printf("Server is connected...\n");
        //read from client
        n=SSL_read(cSSL,buffer,1024);//if n is inside a parentness, if will not be given the value.
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
        n=SSL_write(cSSL,buffer,1024);
        if(n==-1){
            perror("Error\n");
            exit(1);
        }

        SSL_shutdown(cSSL);
        SSL_free(cSSL);
        close(new_fd);
    }
    close(sockfd);
    SSL_CTX_free(sslctx);
    return 0;
}


