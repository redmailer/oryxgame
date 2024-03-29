#ifndef __PACKET__
#define __PACKET__

#include "common.h"

#define PACKET_VERSION 1

#define PACKET_HEAD_LEN 14

#define OFFSET_MESSLEN 0
#define OFFSET_VERSION 4
#define OFFSET_OPERCODE 6
#define OFFSET_ERRCODE 10

#define PACKET_MIN_SIZE 256

class Packet
{
private:
    Packet(INT32 opercode, INT32 errcode);
    Packet(byte *data, INT32 len);

public:
    ~Packet();

    static Packet *NewPacket();
    static Packet *NewPacket(INT32 opercode, INT32 errcode, byte *data = NULL, INT32 len = 0);

    static Packet *NewPacketFromBytes(byte *data, INT32 len);

    Packet *Pack();
    Packet *WriteHead();

    byte *GetRealData(INT32 &len);
    void PrintData();

    Packet *Append(void *data, INT32 len);

private:
    Packet *WriteData(void *data, INT32 len, INT32 pos);
    Packet *Write(INT32 data, INT32 pos);
    Packet *Write(UINT32 data, INT32 pos);
    Packet *Write(INT16 data, INT32 pos);
    Packet *Write(UINT16 data, INT32 pos);

    INT32 Read(INT32 &data, INT32 pos);
    UINT32 Read(UINT32 &data, INT32 pos);
    INT16 Read(INT16 &data, INT32 pos);
    UINT16 Read(UINT16 &data, INT32 pos);

    void CheckLen(int len);

    void Init();

public:
    INT32 message_len;
    INT32 version;
    INT32 operatecode;
    INT32 errorcode;
    byte *data;

    INT32 dataMaxLen; // ��������С
    INT32 dataNowLen; // ��ǰʹ�õĻ�������С
};

#endif