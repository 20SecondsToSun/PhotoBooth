#pragma once

#include "cinder/app/AppNative.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/gl.h"
#include "cinder/Text.h"
#include "PreloaderCircle.h"
#include "AssetsManager.h"
#include "Utils.h"

using namespace std;

class QRcode
{
	public:				
		void draw();	
		void setup();
		void drawError();

		bool isReady, isRender;		

		void setTexture(ci::gl::Texture qrCodeTexture);
		void setLink(std::string link, ci::Font qrCodeFont);
		void setTextureString(std::string str);

	private:
		ci::gl::Texture			qrCodeTextTexture;
		ci::gl::Texture			qrCodeTexture, *errorTexture;	
		ci::Font				qrCodeFont;
		std::string				url;
		std::string				stringQrcode;		

		gl::Texture				loadImageFromString(std::string value);
};