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
#include <sys/stat.h>
#include <netdb.h>
#include <string.h>
#include <assert.h>
#define portnumber 2222
#include<fcntl.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "upload.h"
#include "download_client.h"
void InitializeSSL(){
    SSL_load_error_strings();
    SSL_library_init();
    OpenSSL_add_all_algorithms();
}
typedef struct FilePackage{
    char cmd;   //operation /command
    int filesize;
    int ack;     //flag
    char usrname[50];    //clientname
    char filename[125];   //filename
    char buf[1024];     //filecontent
};

struct FilePackage pack_init(){
    printf("Please input the content:\n");
    char buffer[1024];
    fgets(buffer,1024,stdin);
    printf("Please input the command: \n");
    char cmd;
    scanf("%s",&cmd);
    printf("Please input the ack:");
    int ack;
    scanf("%d",&ack);
    printf("Please input the usrname: \n");
    char usrname[50];
    scanf("%s",usrname);
    printf("Please input the filename: \n");
    char Filename[125];
    scanf("%s",Filename);

    int count;
    count=strlen(buffer);
    int filesize=count;
    struct FilePackage item;
    item=pack(cmd,filesize,ack,usrname,Filename,buffer,count);
    return item;
}

int main(int argc, char* argv[]){
    struct sockaddr_in servaddr;
    int error;
    int sockfd;
    char buffer[1024];
    struct FilePackage item;
    int n;
    SSL_CTX *sslctx;
    SSL *cSSL;

    InitializeSSL();
    sslctx=SSL_CTX_new(SSLv23_client_method());
    if(sslctx==NULL)
    exit(1);
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

    cSSL=SSL_new(sslctx);
    SSL_set_fd(cSSL, sockfd);
    if(SSL_connect(cSSL)==-1)
        perror("Error ");
    //get the remore certificate into the x509 structure
    SSL_get_peer_certificate(cSSL);

    if(flag==-1){
        perror("Error: ");
        exit(1);
    }else{
        printf("Connected to server...\n");
        //upload function
    char usrname[]="Shirley";
    printf("Please enter the command \n");
    char cmd;
    scanf("%c", &cmd);
    switch(cmd){
        char upload_filename[125];
        char download_filename[125];
        case 'U':
            printf("Please enter the file you want to upload: \n");
            scanf("%s", upload_filename);
            int upload_filesize;
            upload_filesize=file_size(upload_filename);
            if(request_upload_file(cSSL,upload_filename,usrname,upload_filesize)!=0) break;
            if(check_request(cSSL)!=0) break;
            upload_file(cSSL, upload_filename,upload_filesize,usrname);
            check_response(cSSL);
            break;
    /*    case 'D':
            printf("Please enter the file you want to download: \n");
            scanf("%s", download_filename);
            printf("Continue...\n");
            download_request(cSSL, usrname, download_filename);
            printf("Continue...\n");
            downloaded(cSSL, usrname, download_filename);
            printf("Continue...\n");
            break;*/

    }
        //download function
        //cSSL=SSL_new(sslctx);
        //SSL_set_fd(cSSL, sockfd);
    /*printf("start to download...\n");
    memset(&item, 0, sizeof(item));
    SSL_read(cSSL, &item,sizeof(item));
    printf("Data received : %s \n", item.buf);*/
    SSL_shutdown(cSSL);
    SSL_free(cSSL);
    close(sockfd);
    SSL_CTX_free(sslctx);
    exit(0);
    }
}

