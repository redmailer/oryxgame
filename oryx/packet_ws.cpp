#include "packet_ws.h"
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <iostream>
#include "LogManager.h"
#include <sstream>

#include "sha1.h"
#include "md5.h"
#include "base64.h"

using namespace std;

bool WSTool::TestWSHandShake(const char * request, std::string & response){
    //TRACEEPOLL(LOG_LEVEL_INFO,"TestWSHandShake:%s", data);
    if(strncmp(request,"GET",3) != 0){
        return false;
    }
    std::istringstream s(request);
    std::string tmp;

    std::getline(s, tmp);
    if (tmp[tmp.size()-1] == '\r') {
        tmp.erase(tmp.end()-1);
    } else {
        return false;
    }

    std::string header;
    std::string::size_type end;
    std::string websocketKey;

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
            //TRACEEPOLL(LOG_LEVEL_INFO,"key:%s value:%s", key.c_str(), value.c_str());
            if(strcmp("Sec-WebSocket-Key",key.c_str()) == 0){
                websocketKey = value;
                break;
            }
        }
    }

    if (websocketKey.size() == 0 ){
        return false;
    }

    response = "HTTP/1.1 101 Switching Protocols\r\n";
    response += "Upgrade: websocket\r\n";
    response += "Connection: Upgrade\r\n";
    response += "Sec-WebSocket-Accept: ";


    websocketKey += "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    SHA1 sha;
	unsigned int message_digest[5];
	sha.Reset();
	sha << websocketKey.c_str();
    sha.Result(message_digest);
	for (int i = 0; i < 5; i++) 
	{
		message_digest[i] = htonl(message_digest[i]);
	}
    websocketKey = base64_encode(reinterpret_cast<const unsigned char*>(message_digest),20);
	response += websocketKey + "\r\n\r\n";


    // std::string serverKey = websocketKey + magicKey;



    // char shaHash[32];
    // memset(shaHash, 0, sizeof(shaHash));
    // sha1::calc(serverKey.c_str(), serverKey.size(), (unsigned char *) shaHash);
    // serverKey = base64::base64_encode(std::string(shaHash)) + "\r\n\r\n";
    // string strtmp(serverKey.c_str());
    // response += strtmp;
    TRACEEPOLL(LOG_LEVEL_INFO,"response:%s", response.c_str());
    return true;
}