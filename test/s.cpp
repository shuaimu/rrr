/*************************************************************************
 > File Name: s.cpp
 > Author: Mengxing Liu
 > Created Time: 2015-03-19
 ************************************************************************/

#include <iostream>
#include "rrr.hpp"
 
using namespace std;

int main(int argc, char **argv) {
    if (argc != 3)
        return -1;
    unsigned int time = atoi(argv[2]);
    
    rrr::PollMgr *pm = new rrr::PollMgr(1);
    base::ThreadPool *tp = new base::ThreadPool(1);
    rrr::Server *server = new rrr::Server(pm, tp);
    
    cout << "server starts..\n";
    
    server->start((std::string("0.0.0.0:") + argv[1]).c_str());
    pm->release();
    tp->release();

    while (1)
        sleep(10000);
    cout << "server terminates..\n";
    delete server;

}

