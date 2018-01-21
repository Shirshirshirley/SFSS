//
//  download_client.h
//  SFSS
//
//  Created by Liushirley on 2018-01-16.
//  Copyright © 2018 Liushirley. All rights reserved.
//

#ifndef download_client_h
int download_request(SSL *cSSL, char *usrname, char *filename);
int downloading(SSL *cSSL);
int downloaded(SSL *cSSL, char *usrname, char *filename);
#define download_client_h


#endif /* download_client_h */
