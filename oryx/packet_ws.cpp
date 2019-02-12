#include "packet_ws.h"
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <iostream>
#include "LogManager.h"

using namespace std;

bool WSTool::TestWSHandShake(const char * data){
    TRACEEPOLL("TestWSHandShake:%s", data);
    return false;
}