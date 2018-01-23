#ifndef LOGIN_SERVER_H_INCLUDED
int check_authentication(char *usrname, char *password);
int receive_login_request(SSL *cSSL);

#define LOGIN_SERVER_H_INCLUDED



#endif // LOGIN_SERVER_H_INCLUDED
