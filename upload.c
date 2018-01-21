//upload FILE
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
#include <dirent.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "upload.h"

typedef struct FilePackage{
    char cmd;   //operation /command
    int filesize;
    int ack;     //flag
    char usrname[50];    //clientname
    char filename[125];   //filename
    char buf[1024];     //filecontent
};

int min(int a, int b){
    if (a > b)
        return b;
    else
        return a;
}
struct FilePackage pack(char tCmd,int tFilesize,int tAck, char *uname, char *tFilename,char *tBuf, int count){
    struct FilePackage item;
    item.cmd=tCmd;
    item.filesize=tFilesize;
    item.ack=tAck;
    strcpy(item.usrname,uname);
    strcpy(item.filename,tFilename);
    printf("count is %d\n", count);
    //if(tBuf != NULL)
    if(tBuf != NULL){
        memcpy(item.buf, tBuf, min(count + 1, 1024));
        printf("[DEBUG][upload.c 37] strlen tBuf is %d\n", (int)strlen(tBuf));
        printf("[DEBUG][upload.c 38] strlen item.buf is %d\n", (int)strlen(item.buf));
    }

    return item;
};
int request_upload_file(SSL* cSSL,char *filename,char *usrname,int filesize){
    struct FilePackage packed_data;
    packed_data=pack('U',filesize,9,usrname,filename,NULL,0);
    printf("[DEBUG]File size is %d packet_data.fileszie is %d\n", filesize, packed_data.filesize);
    if(SSL_write(cSSL,&packed_data,sizeof(packed_data))==-1){
        perror("Failed to send request: \n");
        return -1;
    }
    return 0;
}

int file_size(char *filename){
    int fd=open(filename,O_RDONLY);
    if(fd<0){
        perror("Open file: \n");
        return -1;
    }
    int filesize=lseek(fd,0,SEEK_END);
    lseek(fd,0,SEEK_SET);
    close(fd);
    return filesize;
}

int check_request(SSL* cSSL){
    struct FilePackage buffer;
    if(SSL_read(cSSL,&buffer, sizeof(buffer))==-1){
        perror("Failed to receive uploading response: \n");
        return -1;
    }
    if(buffer.ack!=0){
        printf("Unable to upload. \n");
        return -1;
    }
    printf("Request accepted. \n");
    return 0;
}

int upload_file(SSL *cSSL, char *filename,int filesize,char *usrname){
    char buffer[1024];
    struct FilePackage item;
    int fd=open(filename,O_RDONLY);
    memset(buffer,'\0',sizeof(buffer));
    int n=read(fd, buffer, 1023);

    while(n>0){
        int j;
        printf("buffer length= %d\n", (int)strlen(buffer));
        item=pack('U',filesize,2,usrname,filename, buffer, strlen(buffer));
        printf("Size of buffer %d\n", (int)sizeof(item));
        printf("item.buff length= %d\n", (int)strlen(item.buf));
        j=SSL_write(cSSL, &item, sizeof(item));
        if(j<=0){
            perror("Failed to upload!\n");
            return -1;
        }
        memset(buffer,'\0',sizeof(buffer));
        n=read(fd, buffer,1023);
    }
    close(fd);
    item=pack('U',0,4,usrname,filename,NULL,0);
    int j=SSL_write(cSSL, &item, sizeof(item));
    if(j<=0){
        perror("Failed to upload!\n");
        return -1;
    }
    printf("Uploading file done.\n");
    return 0;
}
int check_response(SSL *cSSL){
    struct FilePackage buffer;
    SSL_read(cSSL, &buffer, sizeof(buffer));
    int ack=buffer.ack;
    if(ack==3){
        printf("Server has received. \n");
        return 0;
    }
    return -1;
}

int upload(SSL *cSSL, char* usrname){
    char filename[125];
    printf("Please enter the filename you want to upload: \n");
    scanf("%s",filename);
    int filesize=file_size(filename);
    if(filesize==-1) return -1;
    if(request_upload_file(cSSL,filename,usrname,filesize)==-1)return -1;
    if( check_request(cSSL)==-1) return -1;
    if(upload_file(cSSL,filename,filesize,usrname)==-1) return -1;
    return 0;
}


   /*     printf("Please enter the filename you want to upload: \n");
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
        //while(n_read!=0)
        //{
            n_read=read(fd,buffer,1024);
            printf("Content inside buffer is %s, number of word read is %d \n", buffer, n_read);
            item=pack('U',filesize,0,usrname, filename,buffer, n_read);
            printf("After packing, the content inside the item is %s", item.buf);
            SSL_write(cSSL, &item, sizeof(item));
            printf("continue...\n");
        //}
        close(fd);*/
