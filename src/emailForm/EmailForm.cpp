#include "EmailForm.h"
#include <ctype.h>
#include <boost/algorithm/string.hpp>

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace mndl::curl;

void EmailForm::setup( ci::app::WindowRef window)
{
	mainWindow = window;

	keyBoardMainBgTex		= AssetManager::getInstance()->getTexture(  "locations/06/06_podl.jpg" );
	emailLineTex			= AssetManager::getInstance()->getTexture( "keyboard/emailLine.png"  );
	closeEmailTex			= gl::Texture( loadImage( loadAsset("keyboard/closeEmail.png"  )));
	deleteAllTex			= gl::Texture( loadImage(getAssetPath("keyboard/deleteAll.png"  )));
	addEmailTex				= gl::Texture( loadImage( loadAsset( "locations/06/06_add.png"  )));	

	mFont					= Font( loadFile(getAssetPath("fonts/Helvetica Neue Light.ttf")), 32 );
	emailInputFont			= Font( loadFile(getAssetPath("fonts/ToyotaDisplay_Bd.ttf")), 70 );
	emailAddFont			= Font( loadFile(getAssetPath("fonts/ToyotaDisplay_Bd.ttf")), 20 );

	addEmail.setup(addEmailTex, mFont, "addEmail", false);
	addEmail.setScreenField(Vec2f(389.0f, 260.0f));	

	deleteAll.setup(deleteAllTex, mFont, "deleteAll", false);	
	deleteAll.setScreenField(Vec2f(1463.0f, 135.0f));	

	closeEmail.setup(closeEmailTex, mFont, "closeEmail", false);
	closeEmail.setScreenField( Vec2f(1778.0f, 88.0f));	
		
	touchKeyBoard.setup( mainWindow, Vec2f(360.0f, HEIGHT - 504.0f));

	touchKeyBoard.keyboardTouchSignal.connect( 
		boost::bind(&EmailForm::keyboardTouchSignal, this) 
		);

	PopupMail::setup();	
}

void EmailForm::show()
{
	touchKeyBoard.setPosition( Vec2f(360.0f, HEIGHT - 504.0f));

	currentEmail = "";
	emailVector.clear();

	isSendingToServer = false;
	isShowPopupError  = false;	
	SERVER_STATUS = "NONE";

	timeline().apply( &bgPosition, Vec2f(0.0f, 0.0f), Vec2f(0.0f, HEIGHT - 1754.0f), 0.6f, EaseOutQuart()).delay(0.4f).finishFn( bind( &EmailForm::initHandlers, this ) );
	timeline().apply( &bgColor, ColorA(1.0f, 1.0f, 1.0f, 0.0f), ColorA(1.0f, 1.0f, 1.0f, 0.98f), 0.5f, EaseOutQuart());
}

void EmailForm::hide()
{
	timeline().apply( &bgPosition,Vec2f(0.0f, 0.0f), 0.6f, EaseInQuart()).finishFn( bind( &EmailForm::closedHandler, this ) );		
	timeline().apply( &bgColor, ColorA(1.0f, 1.0f, 1.0f, 0.4f), 0.5f, EaseInQuart()).delay(0.4f);
}

void EmailForm::closedHandler()
{
	formClosed();
}

void EmailForm::initHandlers()
{
	touchKeyBoard.initKeyboard();

	MouseDownCon   = mainWindow->getSignalMouseDown().connect( std::bind( &EmailForm::MouseDown, this, std::placeholders::_1 ) );
	MouseUpCon	   = mainWindow->getSignalMouseUp().connect(   std::bind( &EmailForm::MouseUp,   this, std::placeholders::_1 ) );
	KeyDownCon	   = mainWindow->getSignalKeyDown().connect(	  std::bind( &EmailForm::KeyDown,	  this, std::placeholders::_1 ) );		
}

void EmailForm::removeHandlers()
{
	touchKeyBoard.removeHandlers();

	MouseUpCon.disconnect( );
	MouseDownCon.disconnect();
	KeyDownCon.disconnect();
}

void EmailForm::keyboardTouchSignal()
{
	string lastCode = touchKeyBoard.getLastCode();

	if (currentEmail.size() > 30)   return;

	if (touchKeyBoard.isBackCode())
	{
		currentEmail = currentEmail.substr(0, currentEmail.size() - 1);	
		return;
	}
	else if (touchKeyBoard.isMailCode())
	{
		if (currentEmail.size() == 0) return;	
				
		if(currentEmail[currentEmail.size() - 1] =='@')
		{
			currentEmail = currentEmail + lastCode;
			boost::to_upper(currentEmail);
		}
		else
		{
			currentEmail = currentEmail + "@" + lastCode;
			boost::to_upper(currentEmail);
		}
		return;
	}
	else if (touchKeyBoard.isSendCode() && isSendingToServer == false)
	{		
		if (currentEmail.size() == 0 && emailVector.size() == 0)
		{			
			errorMode = "EMPTY_MAIL_ERROR";
			isShowPopupError = true;
			touchKeyBoard.removeHandlers();
		}
		else if (currentEmail.size() != 0 &&  Utils::isValidEmail(currentEmail) == false)
		{
			errorMode = "MAIL_FORMAT_ERROR";
			isShowPopupError = true;
			touchKeyBoard.removeHandlers();
		}
		else
		{
			startSending();
		}
		return;
	}

	currentEmail = currentEmail + lastCode;
	boost::to_upper(currentEmail);
}

void EmailForm::drawEmailInput()
{
	TextLayout _text;
	_text.setFont( emailInputFont );
	_text.setColor( Color::white());	
	_text.addLine(currentEmail);	
	 gl::Texture textTexture = gl::Texture( _text.render( true, false ) );

	 Vec2f _scale = Vec2f(1.0f,1.0f);
	
	 if (textTexture.getWidth() > 1050.0f)
	{
		float sc = 1050.0f / textTexture.getWidth();
		_scale= Vec2f(sc, sc);		
	}

	float coordy = deleteAll.getY() - _scale.x*textTexture.getHeight()*0.5f;

	gl::color(ColorA(1.0f, 1.0f, 1.0f, 1.0f));
	gl::pushMatrices();		
		gl::translate(Vec2f(391.0f, coordy));
		gl::scale(_scale);
		gl::draw(textTexture);
		gl::translate(Vec2f(textTexture.getWidth(),0.0f));
		if (getElapsedFrames() % 20 == 0 )
		gl::drawSolidRect(Rectf(0.0f, 0.0f, 10.0f, textTexture.getHeight()));
	gl::popMatrices();
}

void EmailForm::drawAdditionEmails()
{
	if (emailVector.size() == 0 ) return;

	for (size_t i = 0, ilen = emailVector.size(); i < ilen; i++)
	{
		TextLayout simple;
		simple.setFont( emailAddFont );
		simple.setColor( Color::white());
		simple.addLine(emailVector[i]);	

		gl::Texture textTexture = gl::Texture( simple.render( true, false ) );

		gl::color(Color::white());

		gl::pushMatrices();
				gl::translate(Vec2f(393.0f + 1108.0f -textTexture.getWidth() ,246.0f + i *(textTexture.getHeight() + 3.0f)));				
				gl::draw(textTexture);
		gl::popMatrices();				
	}
}

void EmailForm::startSending()
{
	if (currentEmail.size() != 0)
	{		
		emailVector.push_back(currentEmail);
		currentEmail = "";
	}
	
	serverThread = shared_ptr<thread>( new thread( bind( &EmailForm::sendToServerThread, this ) ) );	
}

void EmailForm::sendToServerThread()
{
	ci::ThreadSetup threadSetup; // instantiate this if you're talking to Cinder from a secondary thread		
	isSendingToServer = true;

	string allEmails = "";

	for (size_t i = 0; i < emailVector.size(); i++)
	{
		if ( i != emailVector.size()-1)
		{
			allEmails +=emailVector[i] +",";
		}
		else allEmails +=emailVector[i];		
	}

	console()<<"SEND TO EMAILS::  "<<allEmails<<std::endl;	

	std::map<string,string> strings;
	strings.insert(pair<string, string>( "action" , "mail"));
	strings.insert(pair<string, string>( "id" ,  Params::sessionId));
	strings.insert(pair<string, string>( "email" ,  allEmails));

	string request =  Curl::post( SERVER"/save.php", strings);
	JsonTree jTree;

	console()<<"SERVER ANSWER TO SEND MAILS "<<request<<std::endl;

	try 
	{
		jTree = JsonTree(request);
		
		string success = jTree.getChild("success").getValue();		
	
		if (success == "1")
		{
			SERVER_STATUS = "OK";
		}
		else
		{
			SERVER_STATUS = "ERROR";
			touchKeyBoard.removeHandlers();
		}
	}
	catch (... )
	{
		SERVER_STATUS = "ERROR";
		touchKeyBoard.removeHandlers();
	}

	isSendingToServer = false;	
	serverThread->detach();
}

void EmailForm::MouseDown( MouseEvent &event )
{
	if (isShowPopupError == true)
	{
		isShowPopupError = false;
		touchKeyBoard.initKeyboard();
		return;
	}

	if (SERVER_STATUS == "OK")
	{
		removeHandlers();
		formClosed();		
		return;
	}
	else if (SERVER_STATUS == "ERROR")
	{
		SERVER_STATUS ="NONE";
		isShowPopupError = false;
		return;
	}

	if(deleteAll.contains(event.getPos()))
	{
		currentEmail = "";		
	}	
	else if(addEmail.contains(event.getPos()))
	{
		if (currentEmail.size() == 0)
		{
			errorMode = "EMPTY_MAIL_ERROR";			
			touchKeyBoard.removeHandlers();
			isShowPopupError = true;
		}
		else if (Utils::isValidEmail(currentEmail) == false)
		{
			errorMode = "MAIL_FORMAT_ERROR";			
			touchKeyBoard.removeHandlers();
			isShowPopupError = true;
		}
		else
		{
			addCurrentEmail(currentEmail);
		}		
	}
	else if(closeEmail.contains(event.getPos()))
	{
		removeHandlers();
		hide();
	}	
}

void EmailForm::addCurrentEmail(string _email)
{
	if (emailVector.size() >=3) return;//noMoreAdd = true;

	emailVector.push_back(_email);
	currentEmail = "";	
}

void EmailForm::MouseUp( MouseEvent &event )
{

}

void EmailForm::KeyDown( KeyEvent event  )
{

}


void EmailForm::draw()
{
	gl::color(Color(1.0f, 1.0f, 1.0f));

	if (SERVER_STATUS == "OK")
	{
		PopupMail::draw("SERVER_OK");
		return;
	}


	gl::pushMatrices();
		gl::translate(bgPosition);
		gl::color(bgColor);
		gl::draw(*keyBoardMainBgTex);

		gl::pushMatrices();
			gl::translate(360.0f, 1250.0f);
			touchKeyBoard.draw();
		gl::popMatrices();	

		gl::translate(0.0f, 1754.0f - 1080.0f);
		drawEmailInput();
		drawAdditionEmails();

	gl::popMatrices();

	gl::pushMatrices();

		gl::translate(bgPosition);	
		gl::translate(Vec2f(0.0f, 674.0f));	
		gl::color(ColorA(1.0f, 1.0f, 1.0f, 1.0f));
		addEmail.draw();
		
		gl::color(Color::white());
		gl::pushMatrices();
				gl::translate(Vec2f(393.0f, 206.0f));
				gl::draw(*emailLineTex);
		gl::popMatrices();				

		gl::color(ColorA(1.0f, 1.0f, 1.0f, 1.0f));

		if (currentEmail != "")
			deleteAll.draw();

		closeEmail.draw();

	gl::popMatrices();


	if (isSendingToServer)
	{			
		PopupMail::draw("SERVER_LOADING", "Отправляем фотографии на email..");
	}
	
	if (SERVER_STATUS == "ERROR")
	{
		PopupMail::draw("SERVER_ERROR");
	}
	
	if (isShowPopupError)
	{
		PopupMail::draw(errorMode);
	}
	
}