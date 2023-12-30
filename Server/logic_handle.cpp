#include <iostream>
#include <cstring>

#include "logic_handle.h"

using namespace std;

int signup(char *usename, char *password)
{
    FILE *fr = fopen("login_details.txt", "r");
    char line[1024], check_name[30];
    while (fgets(line, 1024, fr) != NULL)
    {
        sscanf(line, "%s", check_name);
        if (!strcmp(usename, check_name))
        {
            return 1;
        }
    }
    fclose(fr);
    cout<<"dmm ok chua\n";
    FILE *fa = fopen("login_details.txt", "a");
    fprintf(fa, "\n%s %s", usename, password);
    fclose(fa);
    return 0;
}

/**/
int login(char* usename, char* password, int* login_state, char *user){
    if(*login_state)
        return 3;
    FILE *fr = fopen("login_details.txt", "r");
    char line[1024], check_name[30], check_pass[30];
    while (fgets(line, 1024, fr) != NULL)
    {
        sscanf(line, "%s %s", check_name, check_pass);
        if (!strcmp(usename, check_name))
        {   
            cout<<password<<"|"<<check_pass<<"\n";
            if(!strcmp(password, check_pass)){
                fclose(fr);
                *login_state = 1; 
                memset(user, '\0', 30);
                strcpy(user, usename);
                return 0;
            }
            fclose(fr);
            return 1;
        }
    }
    fclose(fr);
    return 2;
}