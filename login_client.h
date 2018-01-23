#ifndef LOGIN_CLIENT_H_INCLUDED
int send_login_request(SSL *cSSL);
int login_request_reply(SSL *cSSL);
#define LOGIN_CLIENT_H_INCLUDED



#endif // LOGIN_CLIENT_H_INCLUDED
