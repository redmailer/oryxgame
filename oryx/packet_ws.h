#ifndef __PACKET_WS__
#define __PACKET_WS__

#include "common.h"
#include <string>


enum WS_FrameType
{
    WS_CONTIUE_FRAME = 0x00,

	WS_TEXT_FRAME = 0x01,
	WS_BINARY_FRAME = 0x02,

    WS_CLOSING_FRAME = 0x08,

	WS_PING_FRAME = 0x09,
	WS_PONG_FRAME = 0x0A,
};

using namespace std;

class PacketWS {
public:
    static PacketWS * DecodeWsPacket(const char * data, INT32 len, INT32& error);
    static PacketWS * EncodeWsPacket(const char * data, INT32 len);
private:
    PacketWS();    
public:
    ~PacketWS();

    init();

    void fetchFin(INT32 &len);
    void fetchOpcode(INT32 &len);
    void fetchMask(INT32 &len);
    void fetchPayLoadLen(INT32 &len);
    void fetchMaskingKey(INT32 &len);
    void fetchPayLoadData(INT32 &len);
    void decodeLoadData();
    
    BYTE fin;
    BYTE opcode;
    BYTE mask;
    INT32 payLoadLength;
    char masking_key[4];

    char * payLoadData;

    INT32 wspacket_len;
    char * wspacket_data;
};


class WSTool {
public:
    static bool TestWSHandShake(const char * request, std::string & response);
    static void WSDecode();
    static void WSEncode();
};

#endif