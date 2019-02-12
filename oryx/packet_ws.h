#ifndef __PACKET_WS__
#define __PACKET_WS__

#include "common.h"
#include <string>

using namespace std;

class PacketWS {
public:
   
};


class WSTool {
public:
    static bool TestWSHandShake(const char * request, std::string & response);
};

#endif