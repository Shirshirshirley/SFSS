//  Created by Liushirley on 2018-01-16.
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
#include <dirent.h>
#include <sys/statfs.h>
#include "upload_server.h"
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
    strcpy(item.usrname,uname);
    strcpy(item.filename,tFilename);
    memcpy(item.buf,tBuf,count);
    return item;
};
int check_freespace(int filesize){
    struct statfs stat;
    statfs("./", &stat);
    /*size=(stat.f_bavail)*(stat.f_bsize);
    printf("The freespace inside this disk is %lu", size);
    freespace=(int) size;
    printf("The freespace inside this disk is %d", freespace);*/
    if((stat.f_bavail)*(stat.f_bsize)<filesize){
        perror("No enough free space \n");
        return -1;
    }
    return 0;
}

int reply_space_check(SSL* cSSL, int filesize, char *usrname,char *filename ){
    int err;
    struct FilePackage item;
    err=check_freespace(filesize);
    if(err<0){
        item=pack('U', filesize, 1, usrname, filename,NULL, 0);
        SSL_write(cSSL, &item ,sizeof(item));
        return 1;
    }else{
        item=pack('U', filesize, 0, usrname, filename,NULL, 0);
        SSL_write(cSSL, &item ,sizeof(item));
        return 0;
    }
    perror("Reply error\n");
    return -1;
}

int check_client_read(SSL *cSSL){
    struct FilePackage buffer;
    SSL_read(cSSL, &buffer, sizeof(buffer));
    int ack=buffer.ack;
    if(ack==2){
        printf("Client is ready for uploading..\n");
        return 0;
    }
    perror("Check client read status error\n");
    return -1;
}

int uploading2(SSL *cSSL){
    struct FilePackage buffer;
    int error=SSL_read(cSSL, &buffer, sizeof(buffer));
    int ack=buffer.ack;
    printf("ack= %d", ack);
    int fd=open(buffer.filename, O_WRONLY|O_CREAT, 0755);
    if(fd<=0){
        perror("File open error\n");
        return -1;
    }
    while(error>0 && ack==2){
        int n_byte=write(fd, buffer.buf, strlen(buffer.buf));
        if(n_byte<0){
            perror("Socket reading error \n");
            return -1;
        }
        error=SSL_read(cSSL, &buffer, sizeof(buffer));
        ack=buffer.ack;
    }
    if(error<=0){
        perror("Uploading error \n");
        return -1;
    }
    close(fd);
    return 0;
}


int uploading(SSL *cSSL){
    struct FilePackage buffer;
    int error=SSL_read(cSSL, &buffer, sizeof(buffer));
    int ack=buffer.ack;
    int fd=open(buffer.filename, O_WRONLY|O_CREAT, 0755);
    if(fd<=0){
        perror("File open error\n");
        return -1;
    }
    while(error>0 && ack==2){
        int n_byte=write(fd, buffer.buf, 1024);
        if(n_byte<0){
            perror("Socket reading error \n");
            return -1;
        }
        error=SSL_read(cSSL, &buffer, sizeof(buffer));
        ack=buffer.ack;
    }
    if(error<=0){
        perror("Uploading error \n");
        return -1;
    }
    close(fd);
    return 0;
}

int uploading_done(SSL *cSSL,int filesize, char *usrname, char *filename){
    struct FilePackage item;
    int err;
    item=pack('U',filesize, 3,usrname,filename,NULL, 0);
    err=SSL_write(cSSL, &item, sizeof(item));
    if(err<0) return -1;
    return 0;
}









