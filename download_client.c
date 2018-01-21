//
//  download_client.c
//  SFSS
//
//  Created by Liushirley on 2018-01-16.
//  Copyright © 2018 Liushirley. All rights reserved.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/statfs.h>
#include <netdb.h>
#include <string.h>
#include <assert.h>
#define portnumber 2222
#include<fcntl.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "upload.h"
#include "download_client.h"
typedef struct FilePackage{
    char cmd;   //operation /command
    int filesize;
    int ack;     //flag
    char usrname[50];    //clientname
    char filename[125];   //filename
    char buf[1024];     //filecontent
};
int download_request(SSL *cSSL, char *usrname, char *filename){
    struct FilePackage item;
    int err;
    item=pack('D',0, 9,usrname, filename, NULL, 0);
    printf("Filename= %s\n", filename);
    err=SSL_write(cSSL,&item, sizeof(item));
    printf("error =%d \n", err);
    if(err<=0){
        perror("Failed to send download request \n");
        return -1;
    }
    printf("Finish sending download request \n");
    return 0;
}

int downloading(SSL *cSSL){
    struct FilePackage buffer;
    int err,ack;
    char filename[125];
    int fd,n_byte;
    n_byte=SSL_read(cSSL, &buffer, sizeof(buffer));
    printf("Number of words read: %d\n", n_byte);
    if(n_byte<=0){
        perror("Downloading: failed to read from server \n");
        return -1;
    }
    ack=buffer.ack;
    strcpy(filename, buffer.filename);
    fd=open(filename,O_WRONLY|O_CREAT, 0755);
    printf("Open file=%d \n", fd);
    if(fd<0){
        perror("Downloading: failed to open/ create file \n");
        return -1;
    }
    while(n_byte>0 && ack==2){
        int n_write;
        n_write=write(fd,buffer.buf, 1024);
        printf("Number of words write: %d\n", n_write);
        if(n_write<0){
            perror("Downloading: failed to write to file \n");
            return -1;
        }
        n_byte=SSL_read(cSSL, &buffer, sizeof(buffer));
        ack=buffer.ack;
    }
    if(n_byte<0){
        perror("Downloading: failed to read from server \n");
        return -1;
    }
    return 0;
}

int downloaded(SSL *cSSL, char *usrname, char *filename){
    struct FilePackage item;
    int err;
    item=pack('D',0, 3,usrname, filename, NULL, 0);
    err=SSL_write(cSSL,&item, sizeof(item));
    if(err<=0){
        perror("Failed to confirm download operation \n");
        return -1;
    }
    return 0;
}
