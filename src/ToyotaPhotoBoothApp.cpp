#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"

#include "LocationEngine.h"
#include "Instruction.h"
#include "PhotoMaker.h"
#include "ShareScreen.h"

#include "AssetsManager.h"
#include "VirtualKeyboard.h"

#include "CameraAdapter.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace gl;


class ToyotaPhotoBoothApp : public AppNative {
  public:

	//void prepareSettings( Settings *settings );	
	void setup();
	void shutdown();
	void update();
	void draw();

	void keyDown( KeyEvent event );

private:

	Texture					*mainBg;
	LocationEngine			 game;
};

void ToyotaPhotoBoothApp::setup()
{
	Params::FULL_SCREEN = RELEASE_VER;
	setFullScreen(Params::FULL_SCREEN);
	setWindowSize((int)WIDTH,(int)HEIGHT);

	//if (RELEASE_VER) ShowCursor(false);
	
	mainBg = AssetManager::getInstance()->getTexture( "locations/01/01_bg.jpg" );


	// ====================================		PRINTER INITIALIZATION   ====================================
	printerWnd::Printer::printerLetterSize = DMPAPER_BOOTH_TWO_PHOTOS;
	int status							   = printerWnd::Printer::applySettings();
	
	string path							  = Params::getPhotosStorageDirectory().string()  + "\\template.bmp";		
	string photo_url					  = Utils::twiceSymbol(path, '\\') ;

	char* cstr = _strdup(photo_url.c_str());
	printerWnd::Printer::imagePath_c = cstr;

	//======================================	LOCATION INITIALIZATION  =====================================


	PreloaderCircle::setup();
	CameraAdapter::Instance()->loadResource();
	CameraAdapter::Instance()->setup();

	PhotoMaker::Instance()->Setup();
	Instruction::Instance()->Setup();
	ShareScreen::Instance()->Setup();

	game.Init("init",getWindow());
	game.ChangeState(ShareScreen::Instance());	
}


void ToyotaPhotoBoothApp::update()
{
	game.Update();
	AssetManager::getInstance()->update();
}

void ToyotaPhotoBoothApp::draw()
{
	gl::enableAlphaBlending();
		gl::clear( Color( 0, 0, 0 ) ); 
		gl::color( Color::white() );
		gl::draw(*mainBg);

		game.Draw();
	gl::disableAlphaBlending();
}


void ToyotaPhotoBoothApp::keyDown( KeyEvent event )
{
    switch (event.getChar())
    {
       // case 'l': mCamera.toggleLiveView(); break;
	case '9':
		//Params::isFboCrash = !Params::isFboCrash;
		//game.ChangeState(PhotoMaker::Instance());
	break;//setFullScreen( !Params::FULL_SCREEN ); break;
	case '1':
		//Params::isPhotoFake = !Params::isPhotoFake;
		//game.ChangeState(PhotoMaker::Instance());
	break;//setFullScreen( !Params::FULL_SCREEN ); break;
	case '2':
		//Params::focusError = !Params::focusError;
		//game.ChangeState(PhotoMaker::Instance());
	break;//setFullScreen( !Params::FULL_SCREEN ); break;
		//case ' ': PhotoCamera::takePhoto(); break;
     case event.KEY_ESCAPE:
	 case 'q':	 
		 quit(); break;
    }
}
void ToyotaPhotoBoothApp::shutdown()
{
	CameraAdapter::Instance()->shutdown();
}

CINDER_APP_NATIVE( ToyotaPhotoBoothApp, RendererGl )
