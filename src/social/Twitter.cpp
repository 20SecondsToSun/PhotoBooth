#include "Twitter.h"

using namespace twDefaults;
using namespace twParams;

bool Twitter::init(string login, string password)
{
	std::string userName( login );
    std::string passWord( password );	
   
    std::string tmpStr, tmpStr2;
    std::string replyMsg;
    char tmpBuf[1024];

    /* Set twitter username and password */
    twitterObj.setTwitterUsername( userName );
    twitterObj.setTwitterPassword( passWord );

	 /* OAuth flow begins */

    /* Step 0: Set OAuth related params. These are got by registering your app at twitter.com */
    twitterObj.getOAuth().setConsumerKey( std::string( TW_API_KEY ) );
    twitterObj.getOAuth().setConsumerSecret( std::string( TW_API_SECRET ) );

	  /* Step 1: Check if we alredy have OAuth access token from a previous run */
    std::string myOAuthAccessTokenKey("");
    std::string myOAuthAccessTokenSecret("");
    std::ifstream oAuthTokenKeyIn;
    std::ifstream oAuthTokenSecretIn;

    oAuthTokenKeyIn.open( TOKEN_KEY_COOKIE );
    oAuthTokenSecretIn.open( TOKEN_SECRET_COOKIE );

    memset( tmpBuf, 0, 1024 );
    oAuthTokenKeyIn >> tmpBuf;
    myOAuthAccessTokenKey = tmpBuf;

    memset( tmpBuf, 0, 1024 );
    oAuthTokenSecretIn >> tmpBuf;
    myOAuthAccessTokenSecret = tmpBuf;

    oAuthTokenKeyIn.close();
    oAuthTokenSecretIn.close();

	if( myOAuthAccessTokenKey.size() && myOAuthAccessTokenSecret.size() )
    {
        /* If we already have these keys, then no need to go through auth again */
        console()<< "Using:  Key:  "<<myOAuthAccessTokenKey.c_str()<< " Secret "<< myOAuthAccessTokenSecret.c_str()<<std::endl;
        twitterObj.getOAuth().setOAuthTokenKey( myOAuthAccessTokenKey );
        twitterObj.getOAuth().setOAuthTokenSecret( myOAuthAccessTokenSecret );
    }
    else
    {
        /* Step 2: Get request token key and secret */
        std::string authUrl;
        twitterObj.oAuthRequestToken( authUrl );
		console()<<" oath ---------------------------"<<authUrl<<std::endl;
        /* Step 3: Get PIN  
        memset( tmpBuf, 0, 1024 );
        printf( "\nDo you want to visit twitter.com for PIN (0 for no; 1 for yes): " );
        gets( tmpBuf );
        tmpStr = tmpBuf;
        if( std::string::npos != tmpStr.find( "1" ) )
        {
            // Ask user to visit twitter.com auth page and get PIN 
            memset( tmpBuf, 0, 1024 );
            printf( "\nPlease visit this link in web browser and authorize this application:\n%s", authUrl.c_str() );
            printf( "\nEnter the PIN provided by twitter: " );
            gets( tmpBuf );
            tmpStr = tmpBuf;
            twitterObj.getOAuth().setOAuthPin( tmpStr );
        }
        else
        {
            // Else, pass auth url to twitCurl and get it via twitCurl PIN handling 
            twitterObj.oAuthHandlePIN( authUrl );
        }*/

		 twitterObj.oAuthHandlePIN( authUrl );//!!!!!!!!!!!!!!!

        /* Step 4: Exchange request token with access token */
        twitterObj.oAuthAccessToken();

        /* Step 5: Now, save this access token key and secret for future use without PIN */
        twitterObj.getOAuth().getOAuthTokenKey( myOAuthAccessTokenKey );
        twitterObj.getOAuth().getOAuthTokenSecret( myOAuthAccessTokenSecret );

        /* Step 6: Save these keys in a file or wherever */
        std::ofstream oAuthTokenKeyOut;
        std::ofstream oAuthTokenSecretOut;

        oAuthTokenKeyOut.open( TOKEN_KEY_COOKIE );
        oAuthTokenSecretOut.open( TOKEN_SECRET_COOKIE );

        oAuthTokenKeyOut.clear();
        oAuthTokenSecretOut.clear();

        oAuthTokenKeyOut << myOAuthAccessTokenKey.c_str();
        oAuthTokenSecretOut << myOAuthAccessTokenSecret.c_str();

        oAuthTokenKeyOut.close();
        oAuthTokenSecretOut.close();
    }
    /* OAuth flow ends */

	 /* Account credentials verification */
    if( twitterObj.accountVerifyCredGet() )
    {
        twitterObj.getLastWebResponse( replyMsg );
		console()<<"MESSAGE  ::"<<replyMsg.c_str()<<std::endl;   
		isAuthFlowComplete = true;
		return true;
    }
    else
    {
        twitterObj.getLastCurlError( replyMsg );
		console()<<"ERROR  ::"<<replyMsg.c_str()<<std::endl;
		return false;       
    }
}
bool Twitter::postTextTweet(string status)
{
	if (!isAuthFlowComplete) return false;

	std::string replyMsg = "";

	if( twitterObj.statusUpdate(  Utils::cp1251_to_utf8(status.c_str())  ) )
    {
        twitterObj.getLastWebResponse( replyMsg );
        console()<<"twitterClient:: twitCurl::statusUpdate web response: "<< replyMsg.c_str()<<std::endl;
		return true;
    }
    else
    {
        twitterObj.getLastCurlError( replyMsg );
        console()<<"twitterClient:: twitCurl1111::statusUpdate error: "<< replyMsg.c_str()<<std::endl;
		return false;
    }
}
bool Twitter::postPhotoTweet(string status, vector<string> filesPath)
{
	if (!isAuthFlowComplete) return false;

	int max_media_per_upload;
	std::string replyMsg = "";

	if( twitterObj.getTwitterConfiguration())
    {
        twitterObj.getLastWebResponse( replyMsg );
		try 
		{
			JsonTree jTree = JsonTree(replyMsg);
			max_media_per_upload = atoi(jTree.getChild("max_media_per_upload").getValue().c_str());	
		}
		catch(...){};
	}
	else
    {
        twitterObj.getLastCurlError( replyMsg );
        console()<<"twitterClient:: twitCurl::statusUpdate error: "<< replyMsg.c_str()<<std::endl;
		return false;
    }
	
	vector<string> filelinks;

	for (int i = 0; i < max_media_per_upload; i++)	
	{
		console()<<"path ::  "<<filesPath[i]<<std::endl;
		filelinks.push_back(filesPath[i]);	
	}
	
	if( twitterObj.uploadPictureFromFile(filelinks,  Utils::cp1251_to_utf8(status.c_str()) ))
    {
        twitterObj.getLastWebResponse( replyMsg );
        console()<<"twitterClient:: twitCurl:: statusUpdate web response: "<< replyMsg.c_str()<<std::endl;
		return true;
    }
	else
    {
        twitterObj.getLastCurlError( replyMsg );
        console()<<"twitterClient:: twitCurl:: statusUpdate error: "<< replyMsg.c_str()<<std::endl;
		return false;
    }
}
bool Twitter::postPhotoTweetBase64(string status, string filesPath)
{
	if (!isAuthFlowComplete) return false;
	std::string replyMsg = "";

	
	DataSourceRef urlRequest =	loadFile( filesPath);	
	Buffer bf = Buffer(urlRequest);
	string strj = toBase64(bf) ;

	if( twitterObj.uploadPictureBase64(strj,  Utils::cp1251_to_utf8(status.c_str()) ) )
    {
        twitterObj.getLastWebResponse( replyMsg );
        console()<<"twitterClient:: twitCurl::statusUpdate web response: "<< replyMsg.c_str()<<std::endl;
		return true;
    }
	else
    {
        twitterObj.getLastCurlError( replyMsg );
        console()<<"twitterClient:: twitCurl::statusUpdate error: "<< replyMsg.c_str()<<std::endl;
		return false;
    }
}


void Twitter::post()
{
	serverThread = shared_ptr<thread>( new thread( bind( &Twitter::twitterPostThread, this ) ) );	
}
void Twitter::twitterPostThread()
{
	ci::ThreadSetup threadSetup; // instantiate this if you're talking to Cinder from a secondary thread	

	bool status;

	if (isAuthFlowComplete == false)
	{
		status = init(login, password);

		if (status == false) 
		{
			response = SocialPostError;
			serverHandler();
			return;
		}
	}

	status =  false;

	switch (type)
	{
		case TEXT_STATUS:
			status = postTextTweet(textStatus);			
		break;

		case PHOTO_STATUS:
			status =  postPhotoTweet(textStatus, photosVector);			
		break;

		default:
		break;
	}

	if (status)
	{
		response = SocialPostOk;	
	}
	else
	{
		response = SocialPostError;
	}

	serverThread->detach();	

	// notificate main thread that twitter finished
	serverHandler();
}
const char * Twitter::getAuthUrl()
{	
	return twitterAuthURL.c_str();
}

void Twitter::logOut()
{
	isAuthFlowComplete = false;
	login = password = "";	
}

string  Twitter::getDefaultStatus()
{
	return STATUS_DEFAULT;
}