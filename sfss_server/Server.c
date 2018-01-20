
//  Created by Liushirley on 2018-01-10.
//  Copyright © 2018 Liushirley. All rights reserved.
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/statfs.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#define portnumber 2222
#include <assert.h>
//the following libraries are used for opensslencry
#include<fcntl.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "upload_server.h"
#include "download_server.h"
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

int check_command(SSL *cSSL, char *cmd){
struct FilePackage buffer;
int err;
//read from client to get the command
err=SSL_read(cSSL, &buffer, sizeof(buffer));
if(err<0){
    perror("Failed to retrieve the command \n");
    return -1;
    }
*cmd=buffer.cmd;
return 0;
}

int main(){
    int sockfd, new_fd,n, error;
    struct FilePackage buffer;
    struct sockaddr_in servaddr;
    struct sockaddr_in clieaddr;
    char read_buf[1024];
    SSL_CTX *sslctx;
    SSL *cSSL;
    //creat oepnssl config and load cert and rsa
    InitializeSSL();
    sslctx=SSL_CTX_new(SSLv23_server_method());
    //SSL_CTX_set_options(sslctx,SSL_OP_SINGLE_DH_USE);
    int use_cert=SSL_CTX_use_certificate_file(sslctx,"cert.pem",SSL_FILETYPE_PEM);
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
        //check user command
        SSL_read(cSSL, &buffer, sizeof(buffer));
        char cmd;
        char usrname[50];
        cmd=buffer.cmd;
        strcpy(usrname, buffer.usrname);
        switch(cmd){
                int upload_filesize=buffer.filesize;
                int download_filesize;
                char *upload_filename;
                strcpy(upload_filename, buffer.filename);
                char download_filename[125];
            case 'U':
                if(check_freespace(upload_filesize)!=0) break;
                if(reply_space_check(cSSL,upload_filesize, usrname,upload_filename)!=0) break;
                if(uploading(cSSL)!=0) break;
                uploading_done(cSSL,upload_filesize, usrname, upload_filename);
                printf("Upload succeed !\n");
                break;
       /*     case 'D':
                memset(download_filename,0, 125);
                printf("Continue...\n");
                download_filesize=accept_download_request(cSSL,usrname,download_filename);
                printf("Continue...\n");
                downloading_server(cSSL,download_filesize,usrname,download_filename);
                printf("Continue...\n");
                downloaded_server(cSSL, usrname, download_filename);
                printf("Continue...\n");
                SSL_read(cSSL, &buffer, sizeof(buffer));
                if(buffer.ack==3){
                    printf("Downloading succeed !\n");
                    break;
                }else{
                    perror("Failed to download \n");
                    break;
                }*/
        }
        //read from client
       /* int n_read=SSL_read(cSSL,&buffer,sizeof(struct FilePackage));
        int fd_read=open(buffer.filename,O_WRONLY|O_CREAT,0755);
        write(fd_read,buffer.buf,1024);
        while(n_read>0){
            printf("data read is %d \n", n_read);
            n_read=SSL_read(cSSL,&buffer,sizeof(struct FilePackage));
            if(n_read<=0) break;
            write(fd_read,buffer.buf,1024);
        }
        close(fd_read);
        printf("Server received \n");
        //write to client

        printf("Prepare to write..\n");
        memset(read_buf,0, 1024);
        printf("please input the string going to client: \n");
        fgets(read_buf,1024,stdin);
        buffer=pack('U',1024,1,"shirley","zicun",read_buf,1024);
        SSL_write(cSSL,&buffer,sizeof(buffer));
        printf("Finsh writing to client");*/
        SSL_shutdown(cSSL);
        SSL_free(cSSL);
        close(new_fd);
        }
    close(sockfd);
    SSL_CTX_free(sslctx);
    return 0;
}


