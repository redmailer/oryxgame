#include "packet_ws.h"
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <iostream>
#include "LogManager.h"
#include <sstream>

#include "sha1.h"
#include "base64.h"

using namespace std;


PacketWS* PacketWS::DecodeWsPacket(const char * data, INT32 len, INT32& error){
    error = 0;
    if(len < 2){
        return NULL;
    }
    PacketWS* wsp = new PacketWS();
    wsp->wspacket_data = new char[len];
    wsp->wspacket_len = len;
    memcpy(wsp->wspacket_data, data, len);
    INT32 readLen = 0;
    wsp->fetchFin(readLen);
    wsp->fetchOpcode(readLen);
    wsp->fetchMask(readLen);
    wsp->fetchPayLoadLen(readLen);

    if(wsp->payLoadLength < 0){
        error = -1;
        delete wsp;
        return NULL;
    }

    INT32 checkLen = readLen + wsp->payLoadLength ;
    if(wsp->mask == 1){
        checkLen += sizeof(wsp->masking_key);
    }

    if(checkLen > len){
        delete wsp;
        return NULL;
    }
    wsp->wspacket_len = checkLen;

    wsp->fetchMaskingKey(readLen);
    wsp->fetchPayLoadData(readLen);
    return wsp;
}

PacketWS::PacketWS(){
    this->init();
}

PacketWS::~PacketWS(){
    if(this->wspacket_data){
        delete[] this->wspacket_data;
        this->wspacket_data = NULL;
    }
    if(this->payLoadData){
        delete[] this->payLoadData;
        this->payLoadData = NULL;
    }
}

void PacketWS::fetchFin(INT32 &len){
    this->fin = (unsigned char)this->wspacket_data[len] >> 7;
}

void PacketWS::fetchOpcode(INT32 &len){
    this->opcode = this->wspacket_data[len] & 0x0f;
    len += 1;
}

void PacketWS::fetchMask(INT32 &len){
    this->mask = (unsigned char)this->wspacket_data[len] >> 7;
}

void PacketWS::fetchPayLoadLen(INT32 &len){
    this->payLoadLength = this->wspacket_data[len] & 0x7f;
    len += 1;
    if(this->payLoadLength == 126){
        UINT16 * length = (UINT16 *)(this->wspacket_data + len);
        this->payLoadLength = ntohs(*length);
        len += 1;
    }else if (this->payLoadLength == 127){
        //不解析，数据量过大
        this->payLoadLength = -1;
        return;
    }
}

void PacketWS::fetchMaskingKey(INT32 &len){
    if(this->mask != 1){
        return;
    }
    memcpy(this->masking_key, this->wspacket_data+len, sizeof(this->masking_key));
    len += sizeof(this->masking_key);
}

void PacketWS::fetchPayLoadData(INT32 &len){
    if(this->payLoadLength <= 0){
        return;
    }
    if(this->payLoadData){
        delete[] this->payLoadData;
        this->payLoadData = NULL;
    }
    this->payLoadData = new char[this->payLoadLength + 1];
    memset(this->payLoadData, 0 , this->payLoadLength + 1);
    memcpy(this->payLoadData, this->wspacket_data+len , this->payLoadLength);
    len += this->payLoadLength;
    this->decodeLoadData();
}

void PacketWS::decodeLoadData(){
    if(this->mask != 1){
        return;
    }
    for(int i = 0 ; i < this->payLoadLength; i++){
        this->payLoadData[i] = this->masking_key[i % 4] ^ this->payLoadData[i];
    }
}

void PacketWS::init(){    
    this->fin = 0;
    this->opcode = 0;
    this->mask = 0;
    this->payLoadLength = 0;
    memset(this->masking_key, 0 ,sizeof(this->masking_key)) ;
    this->payLoadData = NULL;

    this->wspacket_len = 0;
    this->wspacket_data = NULL;
}

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
    //TRACEEPOLL(LOG_LEVEL_INFO,"response:%s, size:%d", response.c_str(),response.size());
    return true;
}