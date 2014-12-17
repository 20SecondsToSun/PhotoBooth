#include "SocialForm.h"
#include <ctype.h>
#include <boost/algorithm/string.hpp>

using namespace ci;
using namespace ci::app;
using namespace std;

Awesomium::WebCore*		 SocialForm::mWebCorePtr;
Awesomium::WebView*		 SocialForm::mWebViewPtr;
Awesomium::WebSession*   SocialForm::session;

void SocialForm::setup( ci::app::WindowRef window)
{
	mainWindow = window;

	keyBoardMainBgTex		= AssetManager::getInstance()->getTexture(  "locations/06/06_podl.jpg" );
	closeEmailTex			= gl::Texture( loadImage( loadAsset("keyboard/closeEmail.png"  )));

	touchKeyBoard.setup( mainWindow, Vec2f(360.0f, HEIGHT - 504.0f));
	touchKeyBoard.keyboardTouchSignal.connect( 
		boost::bind(&SocialForm::keyboardTouchSignal, this) 
		);

	Font mFont;
	closePopup.setup(closeEmailTex, mFont, "closePopup", false);
	closePopup.setScreenField( Vec2f(1778.0f, 88.0f));

	// set Awesomium logging to verbose
	Awesomium::WebConfig cnf;
	//cnf.log_level = Awesomium::kLogLevel_Verbose;
	Awesomium::WebPreferences pref;	
	// initialize the Awesomium web engine
	mWebCorePtr		= Awesomium::WebCore::Initialize( cnf );	
	session			= mWebCorePtr->CreateWebSession(Awesomium::WSLit("soc"), pref);
	mWebViewPtr		= mWebCorePtr->CreateWebView( getWindowWidth(), getWindowHeight(), session );	

	_vkontakteOffset			= Vec2f(0.0f, HEIGHT - 1754.0f+674.0f);
	vkontaktePopupAvailableArea = Area(0,  170, getWindowWidth(), 550);
	facebookPopupAvailableArea  = Area(746, 92, getWindowWidth(), 750);

	_facebookWindowHeight = 500;	
	_facebookWindowWidth  = 700;

	photoURLs.clear();

	for (int i = 1; i < 4; i++)
	{
		string num_s = to_string(i);

		string photo_url = "";
		string path = Params::getPhotosStorageDirectory().string();	
	
		path = path + "\\PHOTO_"+ num_s+ ".jpg";		
		
		for (size_t i = 0; i < path.size(); i++)
		{
			photo_url.push_back( path[i]);
			if (path[i] == '\\' ) photo_url.push_back( path[i]);
		}

		photoURLs.push_back(photo_url);		
	}
}

void SocialForm::show(int mode)
{
	isTryFocusInLoginTextField = false;
	popupMode = mode;
	popupAnimationState = POPUP_ANIMATION_STATE;
	socialServerStatus  = SERVER_STATUS_NONE;	
	session->ClearCookies();
	
	timeline().apply( &bgPosition, Vec2f(0.0f, 0.0f), Vec2f(0.0f, HEIGHT - 1754.0f), 0.6f, EaseOutQuart()).delay(0.4f).finishFn( bind( &SocialForm::initHandlers, this ) );
	timeline().apply( &bgColor, ColorA(1.0f, 1.0f, 1.0f, 0.0f), ColorA(1.0f, 1.0f, 1.0f, 0.98f), 0.5f, EaseOutQuart());	

	if (popupMode == SOCIAL_VKONTAKTE)
	{		
		social = new Vkontakte();		
		social->clear_token();	
		socialServerSignalCon = social->serverHandler.connect( 
			boost::bind(&SocialForm::socialServerSignal, this) 
		);		

		postingWaitingText = "Отправляем фотографии во ВКонтакте..";

		touchKeyBoard.setPosition( Vec2f(360.0f, HEIGHT - 484.0f));

		if( mWebViewPtr )
			mWebViewPtr->Resize(getWindowWidth(), 550 );
	
		console()<<" auth url :: "<<social->getAuthUrl()<<std::endl;
		mWebViewPtr->LoadURL( Awesomium::WebURL( Awesomium::WSLit( social->getAuthUrl()) ) );
		mWebViewPtr->Focus();
	}
	else if (popupMode == SOCIAL_FACEBOOK)
	{		
		social = new Facebook();	
		social->clear_token();
		socialServerSignalCon = social->serverHandler.connect( 
			boost::bind(&SocialForm::socialServerSignal, this) 
		);
	
		postingWaitingText = "Отправляем фотографии в Facebook..";

		touchKeyBoard.setPosition( Vec2f(360.0f, HEIGHT - 484.0f));

		if( mWebViewPtr )
			mWebViewPtr->Resize(_facebookWindowWidth, _facebookWindowHeight );

		_facebookOffset = Vec2f(0.5f * (getWindowWidth() - _facebookWindowWidth), HEIGHT - 1754.0f+674.0f+20.0f);
	
		mWebViewPtr->LoadURL( Awesomium::WebURL( Awesomium::WSLit( social->getAuthUrl()) ) );
		mWebViewPtr->Focus();
	}
	else if (popupMode == SOCIAL_TWITTER)
	{	
		social = new Twitter();			
		social->clear_token();
		socialServerSignalCon = social->serverHandler.connect( 
			boost::bind(&SocialForm::socialServerSignal, this) 
		);

		/*string login = "metalcorehero", password = "Metalcorehero88";
		string message = "твитератест1";
		social->setLoginPassword(login, password);
		//social->postStatus(message);
		string url = "Z:\\GENERATIVE\\Cinder\\cinder_0.8.5_vc2012\\cinder_0.8.5_vc2012\\apps\\PocoTest\\vc11\\Release\\Documents\\Canon Pictures\\PHOTO_1.jpg";
		social->postPhoto(url, message);
		*/	
	

	}
}
void SocialForm::hide()
{
	timeline().apply( &bgPosition,Vec2f(0.0f, 0.0f), 0.6f, EaseInQuart()).finishFn( bind( &SocialForm::closedHandler, this ) );		
	timeline().apply( &bgColor, ColorA(1.0f, 1.0f, 1.0f, 0.4f), 0.5f, EaseInQuart()).delay(0.4f);
}

void SocialForm::socialServerSignal()
{
	if (social->getResponse() == "OK")
		socialServerStatus = SERVER_STATUS_POST_READY;
	else 
		socialServerStatus = SERVER_STATUS_POST_ERROR;
}

void SocialForm::keyboardTouchSignal()
{
	if (touchKeyBoard.isBackCode())
	{	
		// if focus not in textInput return else means goint to previous web page
		if(mWebViewPtr->focused_element_type()!=3 && 
		   mWebViewPtr->focused_element_type()!=4 ) return;

		KeyEvent key = VirtualKeyboard::imitate_BACKSPACE_KEY_EVENT();
		ph::awesomium::handleKeyDown( mWebViewPtr, key );
	}
	else if (touchKeyBoard.isMailCode())
	{
		string lastCode = touchKeyBoard.getLastCode();

		for (size_t i = 0; i < lastCode.size(); i++)
		{
			char chr = lastCode[i];		
			chr = tolower(chr);
			KeyEvent key(getWindow(),1,chr,chr, 1,chr);
			ph::awesomium::handleKeyDown( mWebViewPtr, key );
		}		
	}
	else if (touchKeyBoard.isSendCode())
	{
		KeyEvent key = VirtualKeyboard::imitate_ENTER_KEY_EVENT();
		ph::awesomium::handleKeyDown( mWebViewPtr, key );		
	}
	else
	{
		string lastCode = touchKeyBoard.getLastCode();
		char chr = lastCode[0];			
		KeyEvent key(getWindow(),1,chr,chr, 1,chr);
		ph::awesomium::handleKeyDown( mWebViewPtr, key );
	}
}

void SocialForm::initHandlers()
{
	popupAnimationState = POPUP_READY_STATE;
	touchKeyBoard.initKeyboard();

	MouseDownCon   = mainWindow->getSignalMouseDown().connect( std::bind( &SocialForm::MouseDown, this, std::placeholders::_1 ) );
	MouseUpCon	   = mainWindow->getSignalMouseUp().connect(   std::bind( &SocialForm::MouseUp,   this, std::placeholders::_1 ) );
	KeyDownCon	   = mainWindow->getSignalKeyDown().connect(   std::bind( &SocialForm::KeyDown,	  this, std::placeholders::_1 ) );		
}

void SocialForm::removeHandlers()
{
	socialServerSignalCon.disconnect();
	delete social;

	touchKeyBoard.removeHandlers();	

	MouseUpCon.disconnect( );
	MouseDownCon.disconnect();
	KeyDownCon.disconnect();
}

void SocialForm::closedHandler()
{
	formClosed();
}

void SocialForm::MouseDown( MouseEvent &event )
{
	if (SERVER_STATUS_POSTING == socialServerStatus)
	{
		return;
	}
	else if(closePopup.contains(event.getPos()) ||
			socialServerStatus  ==  SERVER_STATUS_POST_READY|| 
			socialServerStatus == SERVER_STATUS_POST_ERROR)
	{
		removeHandlers();
		hide();
	}
	else if (popupMode ==  SOCIAL_VKONTAKTE && 
			 vkontaktePopupAvailableArea.contains(event.getPos()))
	{			
		MouseEvent mEvent = VirtualKeyboard::inititateMouseEvent(event.getPos() -_vkontakteOffset);
		ph::awesomium::handleMouseDown( mWebViewPtr, mEvent );		
	}
	else if (popupMode ==  SOCIAL_FACEBOOK && 
			 facebookPopupAvailableArea.contains(event.getPos()))
	{	
		MouseEvent mEvent = VirtualKeyboard::inititateMouseEvent(event.getPos() -_facebookOffset);
		ph::awesomium::handleMouseDown( mWebViewPtr, mEvent );	
	}

	console()<< "  coord  :: "<< event.getPos()<<endl;
}

void SocialForm::MouseUp( MouseEvent &event )
{
	if (popupMode ==  SOCIAL_VKONTAKTE && 
		vkontaktePopupAvailableArea.contains(event.getPos()))
	{					
		MouseEvent mEvent = VirtualKeyboard::inititateMouseEvent(event.getPos() -_vkontakteOffset);
		ph::awesomium::handleMouseUp( mWebViewPtr, mEvent );	
	}
	else if (popupMode ==  SOCIAL_FACEBOOK && 
			facebookPopupAvailableArea.contains(event.getPos()))
	{		
		MouseEvent mEvent = VirtualKeyboard::inititateMouseEvent(event.getPos() -_facebookOffset);
		ph::awesomium::handleMouseUp( mWebViewPtr, mEvent );		
	}
}
void SocialForm::KeyDown( KeyEvent event )
{	
	ph::awesomium::handleKeyDown( mWebViewPtr, event );
}

void SocialForm::update()
{
	if (socialServerStatus == SERVER_STATUS_USER_REJECT) return;

	// update the Awesomium engine
	if(mWebCorePtr)	mWebCorePtr->Update();

	// create or update our OpenGL Texture from the webview
	if(mWebViewPtr && ph::awesomium::isDirty( mWebViewPtr ) ) 
	{
		try {
			// set texture filter to NEAREST if you don't intend to transform (scale, rotate) it
			gl::Texture::Format fmt; 
			fmt.setMagFilter( GL_NEAREST );

			// get the texture using a handy conversion function
			mWebTexture = ph::awesomium::toTexture( mWebViewPtr, fmt );
			//console()<<"UPDATE TEXTURE!!"<<std::endl;
		}
		catch( const std::exception &e ) {
			console() << e.what() << std::endl;
		}

		// update the window title to reflect the loaded content
		char title[1024];
		mWebViewPtr->title().ToUTF8( title, 1024 );
	}

	if(mWebViewPtr)
	{
		if (popupMode ==  SOCIAL_FACEBOOK)
		{		
			char anchr[1024];
			mWebViewPtr->url().anchor().ToUTF8( anchr, 1024 );
			std::string anchString( anchr );
			std::size_t pos = anchString.find("access_token"); 			

			if (pos == 0 && socialServerStatus == SERVER_STATUS_NONE) 	
			{	
				std::string delimiter		= "&";
				std::string token			= anchString.substr(0, anchString.find(delimiter)); 
				social->access_token		=  token.substr(13);				
				socialServerStatus			= SERVER_STATUS_POSTING;		
			
				social->postPhoto(photoURLs, "#ТойотаНастроение");
				//social->postStatus( "#ТойотаНастроение");
			}	
			else
			{
				char query[1024];
				mWebViewPtr->url().query().ToUTF8( query, 1024 );
				std::string queryString( query );
				std::size_t pos_denied = queryString.find("error_reason=user_denied"); 
				if (pos_denied <1000)
				{
					socialServerStatus = SERVER_STATUS_USER_REJECT;
					removeHandlers();
					hide();				
				}
			}			
		}	
		else if (popupMode ==  SOCIAL_VKONTAKTE)
		{
			char anchr[1024];
			mWebViewPtr->url().anchor().ToUTF8( anchr, 1024 );
			std::string anchString( anchr );
			std::size_t pos = anchString.find("access"); 
			
			if (pos==0 && socialServerStatus == SERVER_STATUS_NONE) 	
			{	
				std::string delimiter = "&";
				std::string token = anchString.substr(0, anchString.find(delimiter));
				social->access_token = token.substr(13);
				socialServerStatus  = SERVER_STATUS_POSTING;

				//social->postStatus( "#ТойотаНастроение тест");
				social->postPhoto(photoURLs, "#ТойотаНастроение");
			}
			else 
			{
				std::size_t pos_denied = anchString.find("User denied your request"); 
				if (pos_denied <1000)
				{
					socialServerStatus = SERVER_STATUS_USER_REJECT;
					removeHandlers();
					hide();
				}			
			}
		}
		else if (popupMode ==  SOCIAL_TWITTER)
		{
			
			//console()<<" path :: "<<mWebViewPtr->url().path()<<std::endl;
			//console()<<" query :: "<<mWebViewPtr->url().query()<<std::endl;
			//console()<<" spec :: "<<mWebViewPtr->url().spec()<<std::endl;
		}
	}



	if (!isTryFocusInLoginTextField)
	{
		if( mWebTexture&& mWebViewPtr->IsLoading() == false )
		{
			MouseEvent mEvent;
			Vec2f coords;

			if (popupMode ==  SOCIAL_VKONTAKTE)
			{
				coords = Vec2f(932.0f, 246.0f);
				mEvent = VirtualKeyboard::inititateMouseEvent(coords);

			}
			else if (popupMode ==  SOCIAL_FACEBOOK)
			{
				coords = Vec2f(886.0f, 113.0f);
				mEvent = VirtualKeyboard::inititateMouseEvent(coords);
			}

			isTryFocusInLoginTextField = true;
			ph::awesomium::handleMouseDown( mWebViewPtr, mEvent );
			ph::awesomium::handleMouseUp( mWebViewPtr, mEvent );			
		}
	}
}

void SocialForm::draw()
{
	
	if (SERVER_STATUS_POSTING == socialServerStatus)
	{		
		PopupMail::draw("SERVER_LOADING", postingWaitingText);
		return;
	}
	else if (SERVER_STATUS_POST_READY == socialServerStatus)
	{
		if (popupMode ==  SOCIAL_VKONTAKTE)
			PopupMail::draw("SERVER_OK_VK");
		else if (popupMode ==  SOCIAL_FACEBOOK)
			PopupMail::draw("SERVER_OK_FB");
		return;
	}
	else if (SERVER_STATUS_POST_ERROR == socialServerStatus)
	{
		
		if (popupMode ==  SOCIAL_VKONTAKTE)
			PopupMail::draw("SERVER_ERROR_VK");
		else  if (popupMode ==  SOCIAL_FACEBOOK)
			PopupMail::draw("SERVER_ERROR_FB");		
		return;
	}

	gl::pushMatrices();
		gl::translate(bgPosition);
		gl::color(bgColor);
		gl::draw(*keyBoardMainBgTex);
		gl::pushMatrices();
			if (popupMode ==  SOCIAL_VKONTAKTE || popupMode ==  SOCIAL_FACEBOOK || popupMode ==  SOCIAL_TWITTER)
			{
				gl::translate(360.0f, 1270.0f);
			}		
			touchKeyBoard.draw();
		gl::popMatrices();	
	gl::popMatrices();

	if( popupAnimationState == POPUP_READY_STATE && mWebTexture && mWebViewPtr->IsLoading() == false )
	{		
		gl::pushMatrices();			
			gl::translate(bgPosition);
			gl::translate(0.0f, 674.0f);	
			if (popupMode ==  SOCIAL_FACEBOOK)
			{
				gl::color( Color::hex(0x4e4e4e) );
				gl::drawSolidRect(Rectf(0.0f, 0.0f, getWindowWidth(), _facebookWindowHeight + 40.0f));	
				gl::translate(0.5f*(getWindowWidth() - mWebTexture.getWidth()), 20.0f);				
			}
			gl::color(bgColor);
			gl::draw( mWebTexture );
		gl::popMatrices();		
	}
	else
	{
		gl::pushMatrices();
			gl::color(bgColor);
			gl::translate(bgPosition);
			gl::translate(950.0f, 674.0f+150.0f);
			PreloaderCircle::draw();
		gl::popMatrices();
	}

	gl::pushMatrices();
		gl::translate(bgPosition);		
		gl::translate(Vec2f(0.0f, 674.0f));	
		gl::color(ColorA(1.0f, 1.0f, 1.0f, 1.0f));
		closePopup.draw();
	gl::popMatrices();
}