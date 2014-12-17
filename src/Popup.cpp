#include "Popup.h"

using namespace ci;
using namespace gl;
using namespace ci::app;
using namespace std;

Texture* PopupMail::errorPopupBg;
Texture* PopupMail::hintTextMail;
Texture* PopupMail::hintTextTouch;
Texture* PopupMail::serverFonPopup;
Texture* PopupMail::hintTextTouchAllSend;
Texture* PopupMail::hintTextTouchAllSendFb;
Texture* PopupMail::hintTextTouchAllSendVk;

Texture* PopupMail::serverErrorTex;
Texture* PopupMail::emptyErrorTex;

Texture* PopupMail::fbErrorTex;
Texture* PopupMail::vkErrorTex;



Font  PopupMail::msgFont;

void PopupMail::setup()
{
	errorPopupBg				= AssetManager::getInstance()->getTexture(  "locations/06/06_errorbg.jpg" );
	hintTextMail				= AssetManager::getInstance()->getTexture(  "locations/06/06_errorbad.png"  );
	hintTextTouch				= AssetManager::getInstance()->getTexture(  "locations/06/06_touch.png" );
	hintTextTouchAllSend		= AssetManager::getInstance()->getTexture(  "locations/06/06_send.png" );

	hintTextTouchAllSendFb		= AssetManager::getInstance()->getTexture(  "popups/fb.png" );
	hintTextTouchAllSendVk		= AssetManager::getInstance()->getTexture(  "popups/vk.png" );


	serverFonPopup				= AssetManager::getInstance()->getTexture(  "locations/06/06_message.jpg"  );
	emptyErrorTex				= AssetManager::getInstance()->getTexture(  "locations/06/06_errorempty.png"  );
	serverErrorTex				= AssetManager::getInstance()->getTexture(  "locations/06/06_errorinet.png"  );

	fbErrorTex				= AssetManager::getInstance()->getTexture(  "popups/errfb.png"  );
	vkErrorTex				= AssetManager::getInstance()->getTexture(  "popups/errvk.png"  );


	msgFont					= Font( loadFile(getAssetPath("fonts/Helvetica Neue Light.ttf")), 62 );
}

void PopupMail::draw(string mode, string _msg)
{	
	if (mode == "MAIL_FORMAT_ERROR")
	{
		gl::color(ColorA(1.0f, 1.0f , 1.0f, 0.96f));
		gl::draw(*errorPopupBg);

		gl::color(Color::white());

		gl::pushMatrices();
		gl::translate(Vec2f(0.5f*(WIDTH- hintTextMail->getWidth()),370.0f));
		gl::draw(*hintTextMail);
		gl::popMatrices();

		gl::color(Color::white());

		gl::pushMatrices();
		gl::translate(Vec2f(0.5f*(WIDTH- hintTextTouch->getWidth()),700.0f));
		gl::draw(*hintTextTouch);
		gl::popMatrices();
	}
	else if (mode == "EMPTY_MAIL_ERROR")
	{
		gl::color(ColorA(1.0f,1.0f,1.0f,0.96f));
		gl::draw(*errorPopupBg);

		gl::color(Color::white());

		gl::pushMatrices();
		gl::translate(Vec2f(0.5f*(WIDTH- hintTextMail->getWidth()),370.0f));
		gl::draw(*emptyErrorTex);
		gl::popMatrices();

		gl::color(Color::white());

		gl::pushMatrices();
		gl::translate(Vec2f(0.5f*(WIDTH- hintTextTouch->getWidth()),700.0f));
		gl::draw(*hintTextTouch);
		gl::popMatrices();

	}
	else if (mode == "SERVER_ERROR")
	{
		gl::color(ColorA(1.0f,1.0f,1.0f,0.96f));
		gl::draw(*errorPopupBg);

		gl::color(Color::white());

		gl::pushMatrices();
		gl::translate(Vec2f(0.5f*(WIDTH- hintTextMail->getWidth()),250.0f));
		gl::draw(*serverErrorTex);
		gl::popMatrices();

		gl::color(Color::white());

		gl::pushMatrices();
		gl::translate(Vec2f(0.5f*(WIDTH- hintTextTouch->getWidth()),700.0f));
		gl::draw(*hintTextTouch);
		gl::popMatrices();
	}
	else if (mode == "SERVER_ERROR_VK")
	{
		gl::color(ColorA(1.0f,1.0f,1.0f,0.96f));
		gl::draw(*errorPopupBg);

		gl::color(Color::white());

		gl::pushMatrices();
		gl::translate(Vec2f(0.5f*(WIDTH- hintTextMail->getWidth()),250.0f));
		gl::draw(*vkErrorTex);
		gl::popMatrices();

		gl::color(Color::white());
	}
	else if (mode == "SERVER_ERROR_FB")
	{
		gl::color(ColorA(1.0f,1.0f,1.0f,0.96f));
		gl::draw(*errorPopupBg);

		gl::color(Color::white());

		gl::pushMatrices();
		gl::translate(Vec2f(0.5f*(WIDTH- hintTextMail->getWidth()),250.0f));
		gl::draw(*fbErrorTex);
		gl::popMatrices();

		gl::color(Color::white());
	}
	else if (mode == "SERVER_LOADING")
	{
		gl::color(ColorA(1.0f,1.0f,1.0f,0.96f));
		gl::draw(*serverFonPopup);
		gl::color(Color::white());

		gl::pushMatrices();
			gl::color(Color::black());
			gl::translate(910.0f, 540.0f);
			PreloaderCircle::draw();
		gl::popMatrices();

		gl::pushMatrices();
			TextLayout countL;
			countL.setFont( msgFont );
			countL.setColor( Color::hex(0xffffff) );
			countL.addLine(Utils::cp1251_to_utf8(_msg.c_str()));
			gl::Texture drawTxt = gl::Texture( countL.render( true, false ) );
			gl::translate(400.0f, 0.5f*(getWindowWidth() - drawTxt.getWidth()));
			gl::draw(drawTxt);
		gl::popMatrices();
	}
	else if (mode == "SERVER_OK")
	{
		gl::color(ColorA(1.0f,1.0f,1.0f,0.96f));
		gl::draw(*serverFonPopup);
		gl::color(Color::white());

		gl::pushMatrices();
		gl::translate(Vec2f(0.5f*(WIDTH- hintTextTouchAllSend->getWidth()),370.0f));
		gl::draw(*hintTextTouchAllSend);
		gl::popMatrices();

		gl::color(Color::white());

		gl::pushMatrices();
		gl::translate(Vec2f(0.5f*(WIDTH- hintTextTouch->getWidth()),700.0f));
		gl::draw(*hintTextTouch);
		gl::popMatrices();		
	}
	else if (mode == "SERVER_OK_VK")
	{
		gl::color(ColorA(1.0f,1.0f,1.0f,0.96f));
		gl::draw(*serverFonPopup);
		gl::color(Color::white());

		gl::pushMatrices();
		gl::translate(Vec2f(0.5f*(WIDTH- hintTextTouchAllSend->getWidth()),370.0f));
		gl::draw(*hintTextTouchAllSendVk);
		gl::popMatrices();

		gl::color(Color::white());

	}
	else if (mode == "SERVER_OK_FB")
	{
		gl::color(ColorA(1.0f,1.0f,1.0f,0.96f));
		gl::draw(*serverFonPopup);
		gl::color(Color::white());

		gl::pushMatrices();
		gl::translate(Vec2f(0.5f*(WIDTH- hintTextTouchAllSend->getWidth()),370.0f));
		gl::draw(*hintTextTouchAllSendFb);
		gl::popMatrices();

		gl::color(Color::white());
	
	}
}