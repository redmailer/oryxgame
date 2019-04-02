#include "packet.h"
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <iostream>
using namespace std;

Packet::Packet(INT32 opercode, INT32 errcode)
{
	this->Init();
	this->operatecode = opercode;
	this->errorcode = errcode;
	this->WriteHead();
}


Packet::Packet(byte * data, INT32 len)
{
	this->Init();
	if (len >= PACKET_HEAD_LEN) {
		CheckLen(len);

		memcpy(this->data, data, len);
		this->dataNowLen = len;

		this->Read(this->message_len, OFFSET_MESSLEN);
		this->Read(this->version, OFFSET_VERSION);
		this->Read(this->operatecode, OFFSET_OPERCODE);
		this->Read(this->errorcode, OFFSET_ERRCODE);
		
	}
}

Packet::~Packet()
{
	if (this->data != NULL) {
		delete[] this->data;
	}
}

Packet* Packet::Pack()
{
	this->WriteHead();
	return this;
}

Packet* Packet::WriteData(void * data, INT32 len, INT32 pos)
{
	if (data == NULL || len <= 0 || pos < 0) {
		return this;
	}
	CheckLen(len + pos);
	memcpy(this->data + pos, data, len);
	return this;
}

Packet* Packet::Write(INT32 data, INT32 pos)
{
	UINT32 temp = htonl(data);
	this->WriteData(&temp, sizeof(temp), pos);
	return this;
}

Packet* Packet::Write(INT16 data, INT32 pos)
{
	UINT16 temp = htons(data);
	this->WriteData(&temp, sizeof(temp), pos);
	return this;
}

Packet* Packet::Write(UINT32 data, INT32 pos)
{
	UINT32 temp = htonl(data);
	this->WriteData(&temp, sizeof(temp), pos);
	return this;
}

Packet* Packet::Write(UINT16 data, INT32 pos)
{
	UINT16 temp = htons(data);
	this->WriteData(&temp, sizeof(temp), pos);
	return this;
}

INT32 Packet::Read(INT32 & data, INT32 pos)
{
	if (pos + (INT32)sizeof(data) > this->dataNowLen) {
		data = 0;
	}
	else {
		INT32* temp = (INT32 *)(this->data + pos);
		data = ntohl(*temp);
	}
	return data;
}

UINT32 Packet::Read(UINT32 & data, INT32 pos)
{
	if (pos + (INT32)sizeof(data) > this->dataNowLen) {
		data = 0;
	}
	else {
		UINT32* temp = (UINT32 *)(this->data + pos);
		data = ntohl(*temp);
	}
	return data;
}

INT16 Packet::Read(INT16 & data, INT32 pos)
{
	if (pos + (INT32)sizeof(data) > this->dataNowLen) {
		data = 0;
	}
	else {
		INT16* temp = (INT16 *)(this->data + pos);
		data = ntohs(*temp);
	}
	return data;
}

UINT16 Packet::Read(UINT16 & data, INT32 pos)
{
	if (pos + (INT32)sizeof(data) > this->dataNowLen) {
		data = 0;
	}
	else {
		UINT16* temp = (UINT16 *)(this->data + pos);
		data = ntohs(*temp);
	}
	return data;
}

Packet* Packet::WriteHead()
{
	this->CheckLen(this->message_len);
	this->Write(this->message_len, OFFSET_MESSLEN);
	this->Write(this->version, OFFSET_VERSION);
	this->Write(this->operatecode, OFFSET_OPERCODE);
	this->Write(this->errorcode, OFFSET_ERRCODE);
	this->Write(this->message_idx, OFFSET_MESSIDX);
	return this;

}

Packet* Packet::Append(void * data, INT32 len)
{
	if (data != NULL && len > 0) {
		this->WriteData(data, len, this->dataNowLen);
		this->dataNowLen += len;
		this->message_len += len;
		this->WriteHead();
	}
	return this;
}

byte* Packet::GetRealData()
{
	if (this->dataNowLen - PACKET_HEAD_LEN > 0) {
		return this->data + PACKET_HEAD_LEN;
	}
	return NULL;
}

INT32 Packet::GetRealDataLen(){
	return this->dataNowLen - PACKET_HEAD_LEN;
}

void Packet::PrintData()
{
	if (this->dataNowLen - PACKET_HEAD_LEN > 0) {

		this->data[this->dataNowLen] = 0;
		//cout << "packet data :" << this->data + PACKET_HEAD_LEN << endl;
	}
	else {
		//cout << "PrintData NULL" << endl;
	}
}

void Packet::CheckLen(INT32 len)
{
	if (len < PACKET_HEAD_LEN) {
		len = PACKET_HEAD_LEN;
	}
	if (this->dataMaxLen >= len) {
		return;
	}

	while (this->dataMaxLen < len) {
		if (this->dataMaxLen < PACKET_MIN_SIZE) {
			this->dataMaxLen = PACKET_MIN_SIZE;
		}
		else {
			this->dataMaxLen *= 2;
		}
	}

	char* buff = new char[this->dataMaxLen];
	memset(buff, 0 ,this->dataMaxLen);
	if (this->dataNowLen > 0) {
		memcpy(buff, this->data, this->dataNowLen);
	}

	if (this->data != NULL) {
		delete[] this->data;
		this->data = NULL;
	}

	this->data = buff;
}

void Packet::setMessageIdx(UINT32 idx)
{
	this->message_idx = idx;
	this->Write(this->message_idx, OFFSET_MESSIDX);
}

void Packet::Init()
{
	this->message_len = PACKET_HEAD_LEN;
	this->errorcode = 0;
	this->operatecode = 0;
	this->message_idx = 0;
	this->version = PACKET_VERSION;
	this->dataMaxLen = 0;
	this->dataNowLen = PACKET_HEAD_LEN;
	this->data = NULL;
	this->WriteHead();
}


Packet* Packet::NewPacket()
{
	Packet* packet = ORYX_NEW(Packet,0,0);
	return packet;
}

Packet* Packet::NewPacket(INT32 opercode, INT32 errcode, byte * data, INT32 len)
{
	Packet* packet = ORYX_NEW(Packet, opercode, errcode);
	if (data != NULL && len > 0) {
		packet->Append(data, len);
	}
	return packet;
}

Packet* Packet::NewPacketFromBytes(byte * data, INT32 len)
{
	if (len < PACKET_HEAD_LEN || data == NULL) {
		return NULL;
	}
	Packet* packet = ORYX_NEW(Packet, data, len);
	return packet;
}
