#include "packet_ws.h"
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <iostream>
#include "LogManager.h"
#include<sstream>

using namespace std;

bool WSTool::TestWSHandShake(const char * data){
    TRACEEPOLL(LOG_LEVEL_INFO,"TestWSHandShake:%s", data);
    if(strncmp(data,"GET",3) != 0){
        return false;
    }
    std::istringstream s(data);
    std::string request;

    std::getline(s, request);
    if (request[request.size()-1] == '\r') {
        request.erase(request.end()-1);
    } else {
        return false;
    }

    std::string header;
    std::string::size_type end;

    while (std::getline(s, header) && header != "\r") {
        if (header[header.size()-1] != '\r') {
            continue; //end
        } else {
            header.erase(header.end()-1);    //remove last char
        }

        end = header.find(": ",0);
        if (end != std::string::npos) {
            std::string key = header.substr(0,end);
            std::string value = header.substr(end+2);
            TRACEEPOLL(LOG_LEVEL_INFO,"ket:%s value:%s", key.c_str(), value.c_str());
        }
    }

    return false;
}