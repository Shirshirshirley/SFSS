//filebrowse_client.c

//send display request
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
#include "upload.h"
#include "download_client.h"
#include "filebrowse_client.h"
typedef struct FilePackage{
    char cmd;   //operation /command
    int filesize;
    int ack;     //flag
    char usrname[50];    //clientname
    char filename[125];   //filename
    char buf[1024];     //filecontent
};

int send_request_display(SSL *cSSL, char *usrname){
    struct FilePackage item;
    int err;
    //printf("[Debug:] line 34 in subfile.\n");
    item=pack('S',0,9, usrname, NULL, NULL, 0);
    //printf("[Debug:] line 36 in subfile.\n");
    err=SSL_write(cSSL, &item, sizeof(item));
    //printf("[Debug:] line 38 in subfile.\n");
    if(err<0){
        perror("Failed to send request!\n");
        return-1;
    }
    printf("Send display request!\n");
    return 0;
}
int display_filename(SSL *cSSL){
    struct FilePackage buffer;
    char filename_string[1024];
    int n;
    memset(filename_string, '\0', sizeof(filename_string));
    n=SSL_read(cSSL, &buffer, sizeof(buffer));
    if(n<1){
        perror("Failed to read from server \n");
        return -1;
    }
    while(n>0){
        strcpy(filename_string, buffer.buf);
        printf("%s", filename_string);
        memset(filename_string, '\0', sizeof(filename_string));
        n=SSL_read(cSSL, &buffer, sizeof(buffer));
    }
    return 0;
}

