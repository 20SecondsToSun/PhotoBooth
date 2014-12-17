#include "QRcode.h"

using namespace ci;
using namespace ci::app;
using namespace std;

void QRcode::setup()
{
	qrCodeFont			    = Font( loadFile(getAssetPath("fonts/ToyotaDisplay_Bd.ttf")), 25 );	
	errorTexture			= AssetManager::getInstance()->getTexture("locations/05/05_serveraerror.png");
}
void QRcode::drawError()
{
	gl::pushMatrices();
			gl::color(Color::white());
			gl::translate(57.0f,257.0f);
			gl::draw(*errorTexture);			
	gl::popMatrices();
}
void QRcode::draw()
{
	if (isReady == false)
	{
		gl::pushMatrices();
			gl::color(Color::black());
			gl::translate(185.0f, 250.0f);
			PreloaderCircle::draw();
		gl::popMatrices();
	}
	else
	{
		if (isRender == false)
		{
			isRender = true;		
			qrCodeTexture = Utils::loadImageFromString(stringQrcode);		
		}

		gl::pushMatrices();			

		gl::pushMatrices();
				gl::translate(73.0f,154.0f);
				gl::color(Color::white());			
				gl::drawSolidRect(Rectf(0.0f, 0.0f, 220.0f,220.0f));
				gl::translate(9.0f, 9.0f);
				gl::scale(202.0f/qrCodeTexture.getWidth(),202.0f/qrCodeTexture.getWidth());
				gl::draw(qrCodeTexture);
		gl::popMatrices();

			gl::translate(0.0f, 125.0f);
			gl::translate(0.0f, 272.0f);		
			
			TextLayout simple;
			simple.setFont( qrCodeFont );
			simple.setColor( Color::hex(0x197ec7) );
			simple.addLine(url);
			qrCodeTextTexture = gl::Texture( simple.render( true, false ) );

			gl::translate(25.0f + (qrCodeTexture.getWidth() - qrCodeTextTexture.getWidth())*0.5f, 0.0f);

			gl::draw(qrCodeTextTexture);

		gl::popMatrices();
	}
}
void QRcode::setTextureString(std::string str)
{
	stringQrcode = str;
}
void QRcode::setTexture(ci::gl::Texture _qrCodeTexture)
{	
	qrCodeTexture = _qrCodeTexture;
}
void QRcode::setLink(std::string link, Font _qrCodeFont)
{	
	url =  link;
	url = url.substr(7);
}