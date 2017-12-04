#ifndef INIT_H
#define INIT_H

#include <iostream>
#include <unistd.h>

using namespace std;

string logo = "\x1b[1;33m    ________________ \n   / ____/_  __/ __ \\\n  / /_    / / / /_/ /\n / __/   / / / ____/ \n/_/     /_/ /_/\x1b[37mv0.4-alpha\n\x1b[35;1m	__  ____/__  __ \\ \\/ /\n	_  /    __  /_/ /_  / \n	/ /___  _  _, _/_  /  \n	\\____/  /_/ |_| /_/\n\n\x1b[00m";

void init(){
    std::ios_base::sync_with_stdio(false);
    cout << logo;
}

#endif
