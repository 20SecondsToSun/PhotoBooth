#pragma once

#include "cinder/app/AppNative.h"
#include "cinder/Json.h"
#include "SocialShare.h"
#include "Utils.h"
#include "Curl.h"

#define SERVER_OK "OK"
#define SERVER_ERROR "ERROR"

class Server
{
	public:
		static std::string  sendVkSharePlus();
		static std::string  sendFbSharePlus();
		static std::string  sendToServerPrintInfo();

};