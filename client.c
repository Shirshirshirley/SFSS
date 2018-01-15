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
    struct FilePackage pack(char tCmd,int tFilesize,int tAck, char *uname, char *tFilename,char *tBuf, int count){
        struct FilePackage item;
        item.cmd=tCmd;
        item.filesize=tFilesize;
        item.ack=tAck;
        printf("continue111...\n");
        strcpy(item.usrname,uname);
        printf("continue222...\n");
        strcpy(item.filename,tFilename);
        printf("continue333...\n");
        memcpy(item.buf,tBuf,count);
        printf("continue444...\n");
        return item;
    }
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
        printf("Please enter the filename you want to upload: \n");
        char filename[125];
        scanf("%s",filename);
        int fd;
        fd=open(filename,O_RDONLY);
        printf("File discretor= %d \n", fd);
        assert(fd>0);
        int filesize=lseek(fd,0,SEEK_END);
        lseek(fd,0,SEEK_SET);
        printf("File size= %d \n", filesize);
        int n_read=1;
        char usrname[]="shirley";
        while(n_read!=0)
        {
            n_read=read(fd,buffer,1024);
            printf("Content inside buffer is %s, number of word read is %d \n", buffer, n_read);
            item=pack('U',filesize,0,usrname, filename,buffer, n_read);
            printf("After packing, the content inside the item is %s", item.buf);
            SSL_write(cSSL, &item, sizeof(item));
            printf("continue...\n");
        }
        close(fd);
        /*sleep(2);
        printf("wake up for reading operation, ready to read from server...\n");
        memset(buffer,0, sizeof(buffer));
        n=SSL_read(cSSL, buffer, 1024);
        buffer[n]='\0';
        printf("Content read from server: %s", buffer);*/
        SSL_free(cSSL);
        close(sockfd);
        SSL_CTX_free(sslctx);
        exit(0);
    }
}

