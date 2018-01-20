//
//  upload_server.h
//  SFSS
//
//  Created by Liushirley on 2018-01-16.
//  Copyright © 2018 Liushirley. All rights reserved.
//

#ifndef upload_server_h
struct FilePackage pack(char tCmd,int tFilesize,int tAck, char *uname, char *tFilename,char *tBuf, int count);
int check_freespace(int filesize);
int reply_space_check(SSL* cSSL, int filesize, char *usrname,char *filename );
int check_client_read(SSL *cSSL);
int uploading(SSL *cSSL);
int uploading_done(SSL *cSSL,int filesize, char *usrname, char *filename);
#define upload_server_h
#endif /* upload_server_h */
