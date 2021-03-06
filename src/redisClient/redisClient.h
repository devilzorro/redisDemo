#ifndef _REDISCLIENT_H_
#define _REDISCLIENT_H_
 
#include <iostream>
#include <string.h>
#include <string>
#include <stdio.h>

#ifdef WIN32
#define NO_QFORKIMPL
#include "hiredis-win/hiredis.h"
#else
#include "hiredis/hiredis.h"
#endif
 
class CRedisClient
{
public:
    CRedisClient();
	
    ~CRedisClient();
	
    bool Connect(const std::string &host, int port);
	
	bool Connect(const std::string &host, int port, int timeout);
	
	void Disconnect();

	bool HSet(const std::string &hkey,const std::string &key,const std::string &value);

	bool Set(const std::string &key, const std::string &value);
	
    bool Get(const std::string &key, std::string &value);
	
	bool HGet(const std::string &key, const std::string &hkey, std::string &value);
 
	bool CheckStatus();
private:
    redisContext* m_connect;
    redisReply* m_reply;
	
	void FreeConnect();
	void FreeReply();
};
 
#endif //_REDISCLIENT_H_