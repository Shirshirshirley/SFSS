#include <sqlite3.h>
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
#include "filebrowse_client.h"
#include "login_client.h"

typedef struct FilePackage{
    char cmd;   //operation /command
    int filesize;
    int ack;     //flag
    char usrname[50];    //clientname
    char filename[125];   //filename
    char buf[1024];     //filecontent
};

int send_login_request(SSL *cSSL){
    struct FilePackage item;
    int err,count;
    char usrname[125];
    char password[125];
    printf("Please login with your ID/password combination.\n");
    sleep(1);
    printf("ID: \n");
    scanf("%s", usrname);
    getchar();
    printf("Password: \n");
    scanf("%s", password);
    getchar();
    count=strlen(password);
    item=pack('L',0,9,usrname,NULL,password,count);
    err=SSL_write(cSSL, &item, sizeof(item));
    if(err<=0){
        printf("Failed to send login in request.\n");
        return -1;
    }
    return 0;
}

int login_request_reply(SSL *cSSL){
    struct FilePackage buffer;
    int err, ack;
    err=SSL_read(cSSL, &buffer, sizeof(buffer));
    if(err<1){
        printf("Failed to get reply from server.\n");
        return -1;
    }
    ack=buffer.ack;
    if(ack==0){
        printf("Wrong usrname/password...\n");
        return -1;
    }else if(ack==1){
        printf("Login successfully!\n");
        return  0;
    }else if(ack==2){
        printf("Reach the max clients.\n");
        return -1;
    }
    printf("Failed to retrieve command from server.\n");
    return -1;
}


