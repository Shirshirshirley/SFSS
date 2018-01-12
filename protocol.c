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
    int pack(struct Filepackage *item,char tCmd,int tFilesize,int tAck, char *uname, char *tFilename,char *tBuf, int count){
        item->cmd=tCmd;
        item->filesize=tFilesize;
        item->ack=tAck;
        item->usrname=uname;
        item->filename=tFilename;
        memcpy(item->buf,tBuf,count);
        return 0;
    }




int main(int argc, char* argv[]){

}
