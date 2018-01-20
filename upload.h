//upload.h

struct FilePackage pack(char tCmd,int tFilesize,int tAck, char *uname, char *tFilename,char *tBuf, int count);
int request_upload_file(SSL* cSSL,char *filename,char *usrname,int filesize);
int file_size(char *filename);
int check_request(SSL* cSSL);
int upload_file(SSL *cSSL, char *filename,int filesize,char *usrname);
int upload(SSL *cSSL, char* usrname);
