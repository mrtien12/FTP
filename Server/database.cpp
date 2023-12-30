#include "../sqlite/sqlite3.h"
#include <iostream>

using namespace std;


int main(){
    sqlite3 *db;
    string sql = "create table permission("
        "username varchar(40) not null,"
        "path varchar(100) not null,"
        "permission varchar(10) );" ; 
    
    int exit = 0 ;
    exit = sqlite3_open("user.db", &db);
    char* messagesError;

    exit = sqlite3_exec(db, sql.c_str(), NULL, 0, &messagesError);  

    if (exit != SQLITE_OK) { 
        cerr << "Error Create Table" << endl;
        sqlite3_free(messagesError);
    }
    else
        cout << "Table created Successfully" << endl;
    

    sqlite3_close(db);
    return 0;

}
