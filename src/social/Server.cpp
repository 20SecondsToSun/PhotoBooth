#include "Server.h"
#include "Params.h"

using namespace mndl::curl;

std::string   Server::sendVkSharePlus()
{
	if (!RELEASE_VER) return SERVER_OK;

	std::map<string,string> strings;
	strings.insert(pair<string, string>( "action" , "cnt"));
	strings.insert(pair<string, string>( "cnt" ,     "1"));
	strings.insert(pair<string, string>( "type" ,  "vk"));
	string request =  Curl::post( SERVER"/save.php", strings);

	JsonTree jTree;

	try 
	{
		jTree = JsonTree(request);
		console()<<"VKONTAKTE PLUS   "<< jTree.getChild("cnt").getValue()<<std::endl;
		return SERVER_OK;

	}
	catch(...)
	{

	}

	return SERVER_ERROR;
}

std::string   Server::sendFbSharePlus()
{
	if (!RELEASE_VER) return SERVER_OK;

	std::map<string,string> strings;
	strings.insert(pair<string, string>( "action" , "cnt"));
	strings.insert(pair<string, string>( "cnt" ,     "1"));
	strings.insert(pair<string, string>( "type" ,  "fb"));
	string request =  Curl::post( SERVER"/save.php", strings);

	JsonTree jTree;

	try 
	{
		jTree = JsonTree(request);
		console()<<"Facebook PLUS   "<< jTree.getChild("cnt").getValue()<<std::endl;
		return SERVER_OK;
	}
	catch(...)
	{

	}
	return SERVER_ERROR;
}

std::string Server::sendToServerPrintInfo()
{
	if (!RELEASE_VER) return SERVER_OK;

	std::map<string,string> strings;
	strings.insert(pair<string, string>( "action" , "print_count"));
	strings.insert(pair<string, string>( "cnt" ,  "1"));	
	string request =  Curl::post( SERVER"/save.php", strings);

	JsonTree jTree;

	try 
	{
		jTree = JsonTree(request);
		console()<<"PHOTO NUMS   "<< jTree.getChild("cnt").getValue()<<std::endl;
		return SERVER_OK;
	}
	catch(...)
	{

	}

	return SERVER_ERROR;
}