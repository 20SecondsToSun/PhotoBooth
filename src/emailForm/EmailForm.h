#pragma once

#include "cinder/app/AppNative.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/gl.h"
#include "cinder/ImageIO.h"
#include "cinder/Timeline.h"
#include "cinder/Json.h"

#include "Popup.h"
#include "Curl.h"
#include "Params.h"
#include "Utils.h"
#include "AssetsManager.h"
#include "Button.h"
#include "VirtualKeyboard.h"

class EmailForm
{
	public:
		
		void setup( ci::app::WindowRef window);
		void show();
		void hide();
		void draw();
		void update();

		boost::signals2::signal<void(void)> formClosed;

	

	private :
		ci::app::WindowRef mainWindow;

		ci::Anim<ci::Vec2f>  bgPosition;

		ci::Anim<ci::ColorA> errorAlpha, bgColor;

		ci::gl::Texture		*keyBoardMainBgTex, *emailLineTex;

		VirtualKeyboard		touchKeyBoard;

		ci::gl::Texture		addEmailTex, deleteAllTex, closeEmailTex;

		ci::Font				mFont, emailInputFont, emailAddFont;

		Button deleteAll, closeEmail, addEmail, shift;	

		void KeyDown( ci::app::KeyEvent event  );
		void MouseDown( ci::app::MouseEvent &event );
		void MouseUp( ci::app::MouseEvent &event );

		void initHandlers();
		void removeHandlers();
		void closedHandler();
		void keyboardTouchSignal();
		void drawEmailInput();
		void drawAdditionEmails();
		void addCurrentEmail(std::string _email);

		ci::signals::connection KeyDownCon, MouseDownCon, MouseUpCon;

		std::string currentEmail;
		std::vector<std::string> emailVector;

		bool  isShowPopupError;
		std::string errorMode;

		bool isSendingToServer;
		void startSending();
		void sendToServerThread();
		std::shared_ptr<std::thread> serverThread;

		std::string SERVER_STATUS;

};