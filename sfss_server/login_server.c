#include <sqlite3.h>
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
#include "filebrowse_server.h"
#include "login_server.h"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   ]

typedef struct FilePackage{
    char cmd;   //operation /command
    int filesize;
    int ack;     //flag
    char usrname[50];    //clientname
    char filename[125];   //filename
    char buf[1024];     //filecontent
};

int check_authentication(char *usrname, char *password){
    sqlite3 *db;
    sqlite3_stmt *stmt;
    int rc;
    char pwd[50];
    sqlite3_open("Authentication.db", &db);
    if(db==NULL){
        printf("Failed to open DB\n");
        return -1;
    }
    printf("Performing query..\n");
    sqlite3_prepare_v2(db, "SELECT Name,Password FROM ACCOUNTS WHERE Name = ? ",-1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, usrname, strlen(usrname), 0);
    printf("[Debug] usrname is %s, length of usrname is %d.\n",usrname, (int)strlen(usrname));
    rc=sqlite3_step(stmt);
    printf("rc= %d.\n", rc);
    if(rc==SQLITE_ROW){
        strcpy(pwd,(char*)sqlite3_column_text(stmt,1));
        if(strcmp(password,pwd)==0){
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            return 0;
        }
        return -1;
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return -1;
}

int receive_login_request(SSL *cSSL){
    struct FilePackage buffer;
    struct FilePackage item;
    int err, count;\
    char usrname[50];
    memset(usrname,'\0', 50);
    char password[50];
    memset(password,'\0', 50);
    err=SSL_read(cSSL, &buffer, sizeof(buffer));
    if(err<=0){
        printf("Failed to read from SSL.\n");
        return -1;
    }
    strcpy(usrname, buffer.usrname);
    strcpy(password, buffer.buf);
    printf("Check usr anthentication.\n");
    if(check_authentication(usrname,password)!=0){
        printf("Unauthentication usr.\n");
        item=pack('L',0,0,usrname,NULL,NULL,0);
        SSL_write(cSSL,&item, sizeof(item));
        return -1;
    }
    printf("authentication usr.\n");
    item=pack('L',0,1,usrname,NULL,NULL,0);
    SSL_write(cSSL,&item, sizeof(item));
    return 0;
}

