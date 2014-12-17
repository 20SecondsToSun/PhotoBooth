#include "PhotoMaker.h"

PhotoMaker PhotoMaker::PhotoMakerState;

void PhotoMaker::Setup()
{
	mainPhoto			= AssetManager::getInstance()->getTexture("locations/04/04_bg.jpg");	
	numFont				= Font( loadFile(getAssetPath("fonts/ToyotaDisplay_Bd.ttf")), 55 );
	countFont			= Font( loadFile(getAssetPath("fonts/ToyotaDisplay_Bd.ttf")), 400 );	

	fakePhoto = Utils::readFlag("config/fake_photo_config.txt");
}

void PhotoMaker::Init( LocationEngine* game)
{
	_game				= game;
	state				= 0;	
	allSeconds			= SHOT_SECONDS;

	needToSaveFocusOut  = false;

	fakePhoto			= Params::isPhotoFake;	

	CameraAdapter::Instance()->userPhotoIsDownloaded		= false;
	CameraAdapter::Instance()->tryToTakePhoto				= false;
	CameraAdapter::Instance()->photoCameraErrorMsg			= "NONE";
	CameraAdapter::Instance()->live();

	Params::photosNameStorage.clear();

	_scaleRectAnim				= 3.0f;
	_mainPhotoAlphaAnim			= 0.0f;	

	_startShiftNum1				= Vec2f(300.0f  , 0.0f);
	_startShiftNum2				= Vec2f(-300.0f , 0.0f);

	timeline().apply( &_scaleRectAnim, 1.0f, 0.3f, EaseOutCubic()).finishFn( bind( &PhotoMaker::endFirstAnim, this ) );	
}

void PhotoMaker::endFirstAnim()
{
	timeline().apply( &_mainPhotoAlphaAnim,  1.0f, 0.3f, EaseOutCubic());
	timeline().apply( &_startShiftNum1, Vec2f(0.0f, 0.0f), 0.5f, EaseOutQuart() ).finishFn( bind( &PhotoMaker::startPhotoSaver, this ) );
	timeline().apply( &_startShiftNum2, Vec2f(0.0f, 0.0f), 0.5f, EaseOutQuart() );
}

void PhotoMaker::startPhotoSaver()
{		
	if (state == SHOT_NUMS)
	{
		if (needToSaveFocusOut)
		{
			saveFocusErrorPhoto();
		}

		_game->ChangeState(ShareScreen::Instance());		
	}
	else
	{
		allSeconds = SHOT_SECONDS;	
		photoTimer.start();	
	}	
}

void PhotoMaker::Cleanup()
{
	console()<< "CLEAN :: "<<std::endl;
	photoTimer.stop();
}
void PhotoMaker::Pause()
{
	console()<< "PAUSE :: "<<std::endl;
}
void PhotoMaker::Resume()
{
	console()<< "RESUME :: "<<std::endl;
}
void PhotoMaker::MouseEvents( LocationEngine* game )
{

}
void PhotoMaker::HandleEvents(LocationEngine* game)
{

}

void PhotoMaker::Update(LocationEngine* game) 
{	
	CameraAdapter::Instance()->update();

	if (!photoTimer.isStopped())
	{	
		allSeconds = SHOT_SECONDS - (int)photoTimer.getSeconds();

		if (allSeconds <= 0)
		{
			photoTimer.stop();				
			saveFakeScreenPhoto();

			_alphaFlashAnim =  1.0f;

			if (fakePhoto == false)
			{
				CameraAdapter::Instance()->takePhoto();
				timeline().apply(&_alphaFlashAnim,  0.0f, 2.0f );		
			}
			else
			{	
				string fileName = FAKE_PHOTO_SAVE_NAME(to_string(state+1));	
				Params::photosNameStorage.push_back(fileName);
				timeline().apply(&_alphaFlashAnim, 0.0f, 2.0f ).finishFn( bind( &PhotoMaker::flashFinished, this ) );		
			}
		}
	}
	else
	{
		if (CameraAdapter::Instance()->checkIfDownloaded() == true)
		{
			state++;	
			CameraAdapter::Instance()->tryToTakePhoto = false;
			Params::photosNameStorage.push_back(CameraAdapter::Instance()->getpathToDownloadedPhoto());	
			startPhotoSaver();
		}
		else if (CameraAdapter::Instance()->checkIfError())
		{
			console()<<"========================== FOCUS ERROR BUT CONTINUE FUN ======================"<<std::endl;
			
			needToSaveFocusOut = true;
			state++;
			CameraAdapter::Instance()->tryToTakePhoto = false;
			startPhotoSaver();
		}
	}
}

void PhotoMaker::flashFinished()
{
	state++;
	startPhotoSaver();
}

void PhotoMaker::Draw(LocationEngine* game) 
{
	float _width = 1289.0f;
	gl::color( Color::white() );
	gl::draw(*mainPhoto);

	TextLayout countL;
	countL.setFont( countFont );
	countL.setColor( Color::hex(0xffffff) );
	if (allSeconds != 0)
		countL.addLine(to_string(allSeconds));
	else
	  countL.addLine(" ");
	textTexture = gl::Texture( countL.render( true, false ) );

	if (state!=3)
	{		
		gl::pushMatrices();		
			gl::translate( (312.0f - textTexture.getWidth())*0.5f, 200.0f);
			gl::translate(_startShiftNum1);
			gl::draw( textTexture);
		gl::popMatrices();

		gl::pushMatrices();		
			gl::translate(312.0f+_width+ (312.0f - textTexture.getWidth())*0.5f, 200.0f);
			gl::translate(_startShiftNum2);
			gl::draw( textTexture);
		gl::popMatrices();
	}

	gl::pushMatrices();	
			gl::translate(Vec2f(312.0f, 121.0f));
			gl::translate(-Vec2f((_width*_scaleRectAnim - 1289.0f)*0.5f, (832.0f*_scaleRectAnim - 832.0f)*0.5f));
			gl::scale(_scaleRectAnim, _scaleRectAnim);			
			gl::drawSolidRect(Rectf(0.0f, 0.0f, _width, 832));
	gl::popMatrices();

	TextLayout simple1;
	simple1.setFont( numFont );
	simple1.setColor( Color::hex(0xffffff) );
	simple1.addLine("3");
	textTexture = gl::Texture( simple1.render( true, false ) );

	gl::pushMatrices();			
	gl::translate(0.5f*(WIDTH-textTexture.getWidth()) + 60.0f, 958.0f);
	gl::draw( textTexture);
	gl::popMatrices();

	TextLayout simple2;
	simple2.setFont( numFont );
	simple2.setColor(Color::hex(0xffffff) );
	int stateToShow = state + 1;
	if (stateToShow>3) stateToShow= 3;
	simple2.addLine(to_string(stateToShow));
	textTexture = gl::Texture( simple2.render( true, false ) );

	gl::pushMatrices();			
	gl::translate(0.5f*(WIDTH - textTexture.getWidth()) - 30.0f, 958.0f);
	gl::draw( textTexture);
	gl::popMatrices();	
	
	gl::pushMatrices();	

	float _scale = 1268.0f/CameraAdapter::Instance()->getWidth();
	
	gl::color(ColorA(1.0f, 1.0f, 1.0f, _mainPhotoAlphaAnim ));	
	gl::translate(Vec2f(312.0f, 121.0f ));//+ 15));

	gl::pushMatrices();	

	gl::translate(Vec2f( 11.0f, -5.0f ));		
	gl::scale(_scale,_scale);

	if (Params::isDesaturate)
	{
		CameraAdapter::Instance()->drawDesaturate();
	}
	else
	{
		CameraAdapter::Instance()->draw();
	}

	gl::popMatrices();
		
	gl::translate(0.0f, -10.0f);
	gl::drawSolidRect(Rectf(0.0f, 0.0f, _width, 10.0f));

	gl::translate(0.0f, 842.0f);
	gl::drawSolidRect(Rectf(0.0f, 0.0f, _width, 10.0f));
	
	gl::popMatrices();	

	if (needToSaveFocusOut == false)
	{
		gl::color(ColorA(1.0f, 1.0f, 1.0f, _alphaFlashAnim ));	
		gl::drawSolidRect(Rectf(0.0f, 0.0f, (float)getWindowWidth(), (float)getWindowHeight()));
	}
	else
	{
		saveFocusErrorPhoto();
	}
}

void PhotoMaker::saveFocusErrorPhoto() 
{
		needToSaveFocusOut = false;
		Surface screenshot = copyWindowSurface( Area(323, 121, 323 +1268, 121 + 832));
		fs::path path = Params::getPhotosStorageDirectory();
		string fileName = FOCUS_OUT_PHOTO_SAVE_NAME(to_string(state));
		writeImage( path / fs::path( fileName ), screenshot);
		Params::photosNameStorage.push_back(fileName);
}

void PhotoMaker::saveFakeScreenPhoto() 
{
	Surface screenshot = copyWindowSurface( Area(323, 121, 323 +1268, 121 + 832));
	fs::path path = Params::getPhotosStorageDirectory();
	string fileName = FAKE_PHOTO_SAVE_NAME(to_string(state+1));	
	writeImage( path / fs::path( fileName ), screenshot);	
}