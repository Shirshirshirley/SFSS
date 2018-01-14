#include <stdio.h>
#include <stdlib.h>
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
        item->cmd=tCmd;
        item->filesize=tFilesize;
        item->ack=tAck;
        item->usrname=uname;
        item->filename=tFilename;
        memcpy(item->buf,tBuf,count);
        return 0;
    }
    struct FilePackage unpack(SSL* ssl, struct FilePackage item){



    }




int main(int argc, char* argv[]){

}
