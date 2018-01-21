//show the content of the current folder
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
#include <dirent.h>
//the following libraries are used for opensslencry
#include<fcntl.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "upload_server.h"
#include "download_server.h"
#include "filebrowse_server.h"

typedef struct FilePackage{
    char cmd;   //operation /command
    int filesize;
    int ack;     //flag
    char usrname[50];    //clientname
    char filename[125];   //filename
    char buf[1024];     //filecontent
};

int response_request_display(SSL *cSSL, char *usrname){
    struct FilePackage item;
    char buffer[1024];
    int fd, err;
    file_browse();
    fd=open("contents", O_RDONLY);
    if(fd<=0){
        printf("Failed to open the file, fd=%d\n", fd);
        return -1;
    }
    memset(buffer, '\0', sizeof(buffer));
    err=read(fd, buffer, 1023);
    while(err>0){
        item=pack('S',0, 1, usrname,NULL,buffer,sizeof(buffer));
        err=SSL_write(cSSL,&item, sizeof(item));
        if(err<0){
            return -1;
        }
        memset(buffer, '\0', sizeof(buffer));
        err=read(fd, buffer, 1023);
    }
    printf("____End of sending filename contents___\n");
    close (fd);
    unlink ("contents");
    return 0;
}

 int file_browse(){
   int fd;
   fd=open("contents",O_WRONLY|O_CREAT,0755);
   DIR * d;
   struct dirent *dir;
   d=opendir(".");
   if(d){
    while((dir=readdir(d))!=NULL){
            char buffer[256];
            memset(buffer, '\0',sizeof(buffer));
            int count;
            strcpy(buffer,dir->d_name);
            if( strcmp(buffer, ".")&&strcmp(buffer, "..") ){
                strcat(buffer,"\n");
                count=strlen(buffer);
                write(fd,buffer,count);
            }
    }
    closedir(d);
    close(fd);
   }
   return 0;
 }

