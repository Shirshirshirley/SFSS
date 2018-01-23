#include <sqlite3.h>
#include <stdio.h>


int main(void){
    sqlite3 *db;
    char *err_msg=0;


    //create a database and a table attached to the database
    int rc=sqlite3_open("Authentication.db", &db);
    if(rc!=SQLITE_OK){
        fprintf(stderr, "cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return -1;
}
    char *sql="DROP TABLE IF EXISTS ACCOUNTS;"
        "CREATE TABLE ACCOUNTS(Id INT, Name TEXT, Password TEXT);"
        "INSERT INTO ACCOUNTS VALUES(1, 'usr1', 3597);"
        "INSERT INTO ACCOUNTS VALUES(2, 'usr2', 34697);"
        "INSERT INTO ACCOUNTS VALUES(3, 'usr3', 3397);"
        "INSERT INTO ACCOUNTS VALUES(4, 'usr4', 35357);"
        "INSERT INTO ACCOUNTS VALUES(5, 'usr5', 359732);";
    rc=sqlite3_exec(db, sql, 0,0,&err_msg);
    if(rc!=SQLITE_OK){
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return 1;
    }
    sqlite3_close(db);
    return 0;
// retrieving data
}
