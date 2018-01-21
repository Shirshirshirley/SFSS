//
//  download_server.c
//  SFSS
//
//  Created by Liushirley on 2018-01-16.
//  Copyright © 2018 Liushirley. All rights reserved.
//
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
typedef struct FilePackage{
    char cmd;   //operation /command
    int filesize;
    int ack;     //flag
    char usrname[50];    //clientname
    char filename[125];   //filename
    char buf[1024];     //filecontent
};

int getfilesize(char *filename){
    struct stat filestat;
    int fd;
    int err;
    int filesize;
    fd=open(filename, O_RDONLY);
    if(fd<0){
        perror("Error \n");
        return -1;
    }
    err=fstat(fd,&filestat);
    if(err<0){
        perror("Error \n");
        return -1;
    }
    filesize=(int)filestat.st_size;
    close(fd);
    return filesize;
}

int accept_download_request(SSL *cSSL,char *usrname, char *filename){
    struct FilePackage item;
    int err;
    int filesize;
    printf("1\n");
    filesize=getfilesize(filename);
    printf("2\n");
    printf("the File you want to download has a size = %d\n", filesize);
    item=pack('D',filesize,0,usrname,filename, NULL,0);
    err=SSL_write(cSSL, &item, sizeof(item));
    if(err<0){
        perror("Error in responsing \n");
        return -1;
    }
    return filesize;
}

int downloading_server(SSL *cSSL, int filesize,char *usrname,char* filename){
    struct FilePackage item;
    char buffer[1024];
    int fd,n_read;
    fd=open(filename, O_RDONLY);
    printf("Openfile = %d \n", fd);
    n_read=read(fd,buffer,1024);
    printf("Number of words read: %d\n", n_read);
    if(n_read<0){
        perror("Error in reading from file \n");
        return -1;
    }
    while(n_read>0){
        int n;
        printf("Iteration.\n");
        item=pack('D',filesize,2,usrname, filename,buffer,sizeof(buffer));
        printf("Iteration.\n");
        n=SSL_write(cSSL, &item, sizeof(item));
        printf("Iteration.\n");
        if(n<0){
            perror("Downloading: Error in writing to socket \n");
            return -1;
        }
        n_read=read(fd,buffer, 1024);
        printf("Number of words read: %d\n", n_read);
    }
    close(fd);
    if(n_read==0) return 0;
    perror("Failed to download \n");
    return -1;
}

int downloaded_server(SSL *cSSL, char *usrname, char *filename){
    struct FilePackage item;
    item=pack('D',0,4,usrname, filename,NULL,0);
    int err;
    err=SSL_write(cSSL, &item, sizeof(item));
    if(err<0){
        perror("Downloading response error \n");
        return -1;
    }
    return 0;
}







