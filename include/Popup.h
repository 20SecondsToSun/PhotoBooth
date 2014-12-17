#pragma once

#include "cinder/app/AppNative.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/gl.h"
#include "cinder/Text.h"
#include "Params.h"
#include "PreloaderCircle.h"
#include "AssetsManager.h"
#include "Utils.h"


class PopupMail
{
	public:		
		static void draw(std::string mode, std::string _msg = "");
		static void setup();	
		static ci::gl::Texture *errorPopupBg, *hintTextMail, *hintTextTouch, *serverFonPopup, *hintTextTouchAllSend;	
		static ci::gl::Texture *serverErrorTex, *emptyErrorTex;	
		static ci::gl::Texture *hintTextTouchAllSendFb, *hintTextTouchAllSendVk;	
		static ci::gl::Texture *fbErrorTex, *vkErrorTex;	


		static Font  msgFont;
};
