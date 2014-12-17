#include "ShareScreen.h"
#include "Instruction.h"
#include "Server.h"

ShareScreen ShareScreen::ShareScreenState;
int ShareScreen::SOCIAL_POPUP			=  SOCIAL_NONE;
bool ShareScreen::CONNECTION_PROBLEM			= false;
bool ShareScreen::PhotoUploadReady				= false;
bool ShareScreen::saveImage						= false;
bool ShareScreen::PHOTO_POLOSA_READY_TO_SHOW	= false;

bool ShareScreen::EMAIL_ENABLE			= false;
bool ShareScreen::VKONTAKTE_ENABLE		= false;
bool ShareScreen::FACEBOOK_ENABLE		= false;

void ShareScreen::Setup()
{
	
	fon3				= AssetManager::getInstance()->getTexture("locations/05/05_bg-1.jpg" );
	endText				= AssetManager::getInstance()->getTexture("locations/05/05_15sec.png");
	shadowPhoto			= AssetManager::getInstance()->getTexture("locations/05/05_shadow.png");
	cracks				= AssetManager::getInstance()->getTexture("locations/05/05_bg-2.png");
	socialBottomBlock	= AssetManager::getInstance()->getTexture("locations/05/05_bg-3.png");
	emailBottom			= AssetManager::getInstance()->getTexture("bot.png");	

	text1				= AssetManager::getInstance()->getTexture("locations/05/05_15sec.png");
	text2				= AssetManager::getInstance()->getTexture("locations/05/05_finish.png");
	textShow = text1;

	startAgain			= AssetManager::getInstance()->getTexture("locations/05/05_repeat.png");
	printAgainEnable	= AssetManager::getInstance()->getTexture("locations/05/05_print.png");
	printAgainDisable	= AssetManager::getInstance()->getTexture("locations/05/05_print_start.png");
	printAgain			= printAgainEnable;	

	focus_err			= AssetManager::getInstance()->getTexture("locations/05/focus_err.png");	
	
	telpl				= AssetManager::getInstance()->getTexture("telpl.png");
	emailpl				= AssetManager::getInstance()->getTexture("emailpl.png");
	socialpl			= AssetManager::getInstance()->getTexture("socialpl.png");
	vkbtn				= AssetManager::getInstance()->getTexture("vkbtn.png");
	facebookBtn			= AssetManager::getInstance()->getTexture("facebookBtn.png");
	emailBtn			= AssetManager::getInstance()->getTexture("emailBtn.png");

	dayFont				= Font( loadFile(getAssetPath("fonts/ToyotaDisplay_Bd.ttf")), 80 );
	dayFont2			= Font( loadFile(getAssetPath("fonts/ToyotaDisplay_Bd.ttf")), 160 );	
	
	mShaderNoColor		= gl::GlslProg( loadAsset( "shaders/passThru_vert.glsl"),  loadAsset( "shaders/shriner.frag") );		
	mShaderColor		= gl::GlslProg( loadAsset( "shaders/passThru_vert_color.glsl"),  loadAsset( "shaders/shriner_color.frag") );	
	
	focusWatermark = Utils::readFlag("config/focus_config.txt");	
	
	string	folder = "printphoto/small/";//:"printphoto/big/";

	photoTemplateTemp		= gl::Texture( loadImage( getAssetPath(folder + "datepl.jpg")));
	monthJulyTex			= gl::Texture( loadImage( getAssetPath(folder + "jul.png")));
	monthAugTex				= gl::Texture( loadImage( getAssetPath(folder + "avg.png")));
	monthSeptTex			= gl::Texture( loadImage( getAssetPath(folder + "sent.png")));
	//return;
	gl::Texture temp = gl::Texture( loadImage( getAssetPath(folder + "bgphoto.jpg")));
	photoTemplateSurface		= Surface(temp);
	
	mFbo					= gl::Fbo( BIG_PHOTO_WIDTH, BIG_PHOTO_HEIGHT);
	
	qrCodeBlock.setup();
	emailForm.setup(getWindow());
	socialForm.setup(getWindow());

	emailForm.formClosed.connect( 
		boost::bind(&ShareScreen::formClosed, this) 
		);	

	socialForm.formClosed.connect( 
		boost::bind(&ShareScreen::formClosed, this) 
		);		
}

void ShareScreen::formClosed()
{
	SOCIAL_POPUP = SOCIAL_NONE;	
}


void ShareScreen::threadInHandler()
{
	THREAD_FREE = false;
}

void ShareScreen::threadOutHandler()
{
	THREAD_FREE = true;
}

void ShareScreen::Init( LocationEngine* game)
{
	_game = game;
	_game->animationRunning(true);

	

	
	PHOTO_POLOSA_READY_TO_SHOW = false;
	focusWatermark = Params::focusError;



	
	EMAIL_ENABLE		= false;
	VKONTAKTE_ENABLE	= false;
	FACEBOOK_ENABLE		= false;

	textShow = text1;

	THREAD_FREE		= true;
	saveImage		= false;
	isNowPrinting	= false;
	SOCIAL_POPUP	= SOCIAL_NONE;

	returnTimer.start();
	printerTimer.stop();	

	resetAnimationParams();			

	current_day		= Utils::getCurrentTime().tm_mday;
	current_month   = Utils::getCurrentTime().tm_mon;	

	if (current_month  == 6)
	{
		monthInfo.tex  = monthJulyTex;
		monthInfo.coord1 = Vec2f(93.0f, 213.0f);
		monthInfo.coord2 = Vec2f(697.0f, 213.0f);
	}
	else if (current_month  == 7)
	{
		monthInfo.tex  = monthAugTex;
		monthInfo.coord1 = Vec2f(73.0f, 213.0f);
		monthInfo.coord2 = Vec2f(677.0f, 213.0f);
	}
	else if (current_month  == 8)
	{
		monthInfo.tex  = monthSeptTex;
		monthInfo.coord1 = Vec2f(63.0f, 213.0f);
		monthInfo.coord2 = Vec2f(667.0f, 213.0f);
	}

	PhotoUploadReady		= false;
	qrCodeBlock.isReady		= false;
	qrCodeBlock.isRender	= false;
	CONNECTION_PROBLEM		= false;
	
	state = 1;	
	
	Params::sessionId = "";

	fbButtonCoord			= Rectf ( 666.0f, 586.0f, 666.0f + 266.0f,586.0f + 133.0f  );
	vkButtonCoord			= Rectf ( 666.0f, 586.0f + 142.0f, 666.0f + 266.0f, 586.0f + 142.0f + 133.0f );
	emailButtonCoord		= Rectf ( 1070.0f, 586.0f, 1070.0f + 266.0f,586.0f + 276.0f );
	startAgainButtonCoord	= Rectf ( 1168.0f, 827.0f, 1168.0f + 625.0f, 827.0f + 131.0f );
    repeatPrintButtonCoord  = Rectf ( 158.0f, 830.0f,   158.0f + 307.0f,830.0f + 131.0f );
}

void ShareScreen::StartanimationFinish()
{
	
	_game->animationRunning(false);
	
	// ----------- delete -------------------------------------
	if(RELEASE_VER == false)
	{
		Params::photosNameStorage.clear();
		Params::photosNameStorage.push_back("FOCUS_OUT_3.JPG");
		Params::photosNameStorage.push_back("FOCUS_OUT_2.JPG");
		Params::photosNameStorage.push_back("FOCUS_OUT_1.JPG");
	}
	// ----------- delete -------------------------------------
	
	
	try
	{
		if (Params::isFboCrash == false)
		{
			PREPARE_IMAGES_TO_SERVER();
		}
		else
		{
			LOAD_FAKE_PHOTOS_BECAUSE_BREAK_FBO();
		}
	}
	catch(...)
	{
		LOAD_FAKE_PHOTOS_BECAUSE_BREAK_FBO();
	}
	
	

	try
	{
		PREPARE_IMAGES_TO_PRINT();
	}
	catch(...)
	{
		
	}
	

	PHOTO_POLOSA_READY_TO_SHOW		= true;
	photoReadyCoords				= Vec2f(-590.0f, 68.0f);
	shadowAlpha						= ColorA(0.0f, 0.0f, 0.0f, 0.0f);

	timeline().apply( &photoReadyCoords, Vec2f(213.0f, 68.0f), 0.5f, EaseOutQuart()).delay(1.3f);		
	timeline().apply( &shadowAlpha,ColorA(0.0f, 0.0f, 0.0f, 1.0f), 0.5f, EaseOutQuart()).delay(1.3f);	

	printerTimer.stop();
	printerTimer.start();	
	
	if (RELEASE_VER == false)
		  return;

	savePrintServerThread = shared_ptr<thread>( new thread( bind( &ShareScreen::backGroundHandlers, this ) ) );
}

void ShareScreen::backGroundHandlers()
{
	THREAD_FREE = false;	
	ci::ThreadSetup threadSetup; // instantiate this if you're talking to Cinder from a secondary thread	
	
	SEND_IMAGE_TO_PRINTER();
	SEND_IMAGE_TO_SERVER();

	Server::sendToServerPrintInfo();

	savePrintServerThread->detach();
	THREAD_FREE = true;	
}

void ShareScreen::LOAD_FAKE_PHOTOS_BECAUSE_BREAK_FBO()
{
	Utils::writeCrashLog("config/fbolog.txt");
	Params::photosStorage.clear();

	sendToMailPhoto = Surface(BIG_PHOTO_WIDTH, BIG_PHOTO_HEIGHT * 3, true);

	for (size_t  i = 0; i < Params::photosNameStorage.size(); i++)
	{
		string file = "FAKE_PHOTO_"+to_string(i+1)+".jpg";
		fs::path imgPath = Params::getPhotosStorageDirectory()/file;		
		gl::Texture img =  gl::Texture( loadImage( imgPath) );
	
		Surface surf = Surface(img);
		surf = resizeScreenshot(surf, (int32_t)BIG_PHOTO_WIDTH, (int32_t)BIG_PHOTO_HEIGHT);		
	
		saveImageToFile(surf, PHOTO_SAVE_NAME(to_string(i+1)));
		Params::photosStorage.push_back(surf);
		Vec2f offset = Vec2f(0.0f, (float)BIG_PHOTO_HEIGHT*i);
		sendToMailPhoto.copyFrom(surf, Area(0, 0, BIG_PHOTO_WIDTH, BIG_PHOTO_HEIGHT), offset);	
	}

	saveImageToFile(sendToMailPhoto, FILE_NAME_TO_MAIL);
}

void ShareScreen::PREPARE_IMAGES_TO_SERVER()
{
	Params::photosStorage.clear();	

	sendToMailPhoto = Surface(BIG_PHOTO_WIDTH, BIG_PHOTO_HEIGHT * 3, true);

	for (size_t  i = 0; i < Params::photosNameStorage.size(); i++)
	{
		fs::path imgPath = Params::getPhotosStorageDirectory()/Params::photosNameStorage[i];		
		gl::Texture img =  gl::Texture( loadImage( imgPath) );

		Surface surf1 = Surface(img);
		surf1 = resizeScreenshot(surf1, (int32_t)BIG_PHOTO_WIDTH, (int32_t)BIG_PHOTO_HEIGHT);		
		drawToFBO(surf1); 
		
		gl::setViewport( getWindowBounds() );
		mFbo.getTexture().setFlipped(true);
		Surface surf = Surface(mFbo.getTexture());
	
		saveImageToFile(surf, PHOTO_SAVE_NAME(to_string(i+1)));
		Params::photosStorage.push_back(surf);
		Vec2f offset = Vec2f(0.0f, (float)BIG_PHOTO_HEIGHT*i);
		sendToMailPhoto.copyFrom(surf, Area(0, 0, BIG_PHOTO_WIDTH, BIG_PHOTO_HEIGHT), offset);	
	}

	saveImageToFile(sendToMailPhoto, FILE_NAME_TO_MAIL);
}

void ShareScreen::drawToFBO(Surface img)
{
      gl::SaveFramebufferBinding bindingSaver;
      mFbo.bindFramebuffer();
	  Area saveView = getViewport();
      gl::setViewport(mFbo.getBounds() );
	  gl::pushMatrices();
      gl::setMatricesWindow( mFbo.getSize(), false );
      gl::clear( Color( 0, 0, 0 ) );
   
	  if (Params::isDesaturate == false)
	  {
		 mShaderColor.bind();
		 mShaderColor.uniform( "basetex", 2 );
		 gl::draw( img );
		 mShaderColor.unbind();
	  }
	  else
	  {
		 mShaderNoColor.bind();
		 mShaderNoColor.uniform( "basetex", 2 );
		 gl::draw( img );
		 mShaderNoColor.unbind();
	  }
	  gl::popMatrices();
	
	  gl::setViewport(saveView);
}

void ShareScreen::PREPARE_IMAGES_TO_PRINT()
{
	int resize_width  = 591;
	int resize_height = 377;
	Vec2f shift1= Vec2f(61.0f, 102.0f), shift2 = Vec2f(665.0f, 102.0f);

	int	 startX1 = 17, startX2 = 621, startY = 364;	
	int	 shitYBeetwen = 4;
	int cropX1= 12, cropY1 = 28, cropX2 = 602, cropY2 = 1797;
	
	
	photoCollage = photoTemplateSurface;
	
	photoCollage.copyFrom(_topSurface, Area(0,0,_topSurface.getWidth(),_topSurface.getHeight()), shift1);
	photoCollage.copyFrom(_topSurface, Area(0,0,_topSurface.getWidth(),_topSurface.getHeight()), shift2);	

	
			
	for (size_t i = 0, ilen = Params::photosStorage.size(); i < ilen; i++)
	{	
		Surface s = resizeScreenshot(Params::photosStorage[i], (int32_t)resize_width, (int32_t)resize_height);
		Vec2f offset = Vec2f((float)startX1, (float)(resize_height + shitYBeetwen) * i + startY); 
		photoCollage.copyFrom(s, Area(0, 0, resize_width, resize_height), offset);

		Vec2f offset1 = Vec2f((float)startX2,(float)(resize_height+shitYBeetwen)*i+ startY);
		photoCollage.copyFrom(s, Area(0, 0, resize_width, resize_height), offset1);
		
		if (focusWatermark && Params::photosNameStorage[i].substr (0,9) == "FOCUS_OUT")
		{
			Surface ff = Surface(*focus_err);
			photoCollage.copyFrom(ff, Area(0, 0, ff.getWidth(),  ff.getHeight()), offset1 + Vec2i(10,10));
			photoCollage.copyFrom(ff, Area(0, 0, ff.getWidth(),  ff.getHeight()), offset + Vec2i(10,10));
		}		
	}

	photoReadySurf = Surface(cropX2-cropX1,cropY2-cropY1, true);	
	photoReadySurf.copyFrom(photoCollage, Area(cropX1, cropY1, cropX2,cropY2),-Vec2f((float)cropX1, (float)cropY1));
	saveImageToFile(photoCollage, FILE_NAME_TO_PRINT);	
}

void ShareScreen::PREPARE_IMAGES_TO_PRINT_FAKE()
{
	int resize_width  = 591;
	int resize_height = 377;
	Vec2f shift1= Vec2f(61.0f, 102.0f), shift2 = Vec2f(665.0f, 102.0f);

	int	 startX1 = 17, startX2 = 621, startY = 364;	
	int	 shitYBeetwen = 4;
	int cropX1= 12, cropY1 = 28, cropX2 = 602, cropY2 = 1797;

	photoCollage = photoTemplateSurface;	
	photoCollage.copyFrom(_topSurface, Area(0,0,_topSurface.getWidth(),_topSurface.getHeight()), shift1);
	photoCollage.copyFrom(_topSurface, Area(0,0,_topSurface.getWidth(),_topSurface.getHeight()), shift2);	
			
	for (size_t i = 0, ilen = Params::photosStorage.size(); i < ilen; i++)
	{	
		Surface s = resizeScreenshot(Params::photosStorage[i], (int32_t)resize_width, (int32_t)resize_height);
		Vec2f offset = Vec2f((float)startX1, (float)(resize_height + shitYBeetwen) * i + startY); 
		photoCollage.copyFrom(s, Area(0, 0, resize_width, resize_height), offset);

		Vec2f offset1 = Vec2f((float)startX2,(float)(resize_height+shitYBeetwen)*i+ startY);
		photoCollage.copyFrom(s, Area(0, 0, resize_width, resize_height), offset1);

		if (focusWatermark && Params::photosNameStorage[i].substr (0,9) == "FOCUS_OUT")
		{
			Surface ff = Surface(*focus_err);
			photoCollage.copyFrom(ff, Area(0, 0, ff.getWidth(),  ff.getHeight()), offset1 + Vec2i(10,10));
			photoCollage.copyFrom(ff, Area(0, 0, ff.getWidth(),  ff.getHeight()), offset + Vec2i(10,10));
		}
	}

	photoReadySurf = Surface(cropX2-cropX1,cropY2-cropY1, true);	
	photoReadySurf.copyFrom(photoCollage, Area(cropX1, cropY1, cropX2,cropY2),-Vec2f((float)cropX1, (float)cropY1));
	saveImageToFile(photoCollage, FILE_NAME_TO_PRINT);	
}

void ShareScreen::SEND_IMAGE_TO_PRINTER()
{
	try{
		printHandler();	
	}
	catch(...){}
}

void ShareScreen::SEND_IMAGE_TO_SERVER()
{
	try{
		SendToServerImage(FILE_NAME_TO_MAIL);
	}
	catch(...){}
}

void ShareScreen::resetAnimationParams()
{
	float __delay = 0.0f;
	fonAlpha = ColorA(1.0f, 1.0f, 1.0f, 0.0f);
	timeline().apply( &fonAlpha,ColorA(1.0f, 1.0f, 1.0f, 1.0f), 0.5f, EaseOutQuart()).delay(__delay);	

	cracksCoords = Vec2f(-(float)cracks->getWidth(), 0.0f);
	timeline().apply( &cracksCoords, Vec2f(0.0f, 0.0f), 0.6f, EaseOutQuart()).delay(__delay);	

	float _height = 420.0f;
	socialCoords  = Vec2f(608.0f, HEIGHT);
	timeline().apply( &socialCoords, Vec2f(606.0f, _height), 0.6f, EaseOutQuart()).delay(0.5f +__delay);	

	emailCoords  = Vec2f(1012.0f, HEIGHT);
	timeline().apply( &emailCoords, Vec2f(1010.0f, _height), 0.6f, EaseOutQuart()).delay(0.7f + __delay);	
		
	telCoords  = Vec2f(1416.0f, HEIGHT);
	timeline().apply( &telCoords, Vec2f(1414.0f, _height), 0.6f, EaseOutQuart()).delay(0.9f +__delay).finishFn( bind( &ShareScreen::StartanimationFinish, this ) );

	textAlpha= ColorA(1.0f, 1.0f, 1.0f, 0.0f);	
	
	socialBottomBlockCoord= Vec2f(0.0f, HEIGHT);
	timeline().apply( &socialBottomBlockCoord, Vec2f(0.0f, HEIGHT- 81.0f), 0.6f, EaseOutQuart());
	
	photoReadyCoords = Vec2f(-(602.0f - 12.0f), 68.0f);

	shadowAlpha = ColorA(0.0f, 0.0f, 0.0f, 0.0f);
	colorBtns = ColorA(1.0f, 1.0f, 1.0f, 0.0f);
}

void ShareScreen::saveImageToFile(Surface screenshot, string name)
{	
	fs::path path = Params::getPhotosStorageDirectory();
	writeImage( path / fs::path( name ), screenshot);		
}

Surface ShareScreen::resizeScreenshot(Surface screenshot, int32_t width, int32_t height)
{
	Surface screenshot_r = Surface( width, height, false ); 
	ci::ip::resize( screenshot, screenshot.getBounds(), &screenshot_r, screenshot_r.getBounds(), FilterBox() );
	return screenshot_r;
}

void ShareScreen::Cleanup()
{
	console()<< "CLEAN ShareScreen:: "<<std::endl;
	printerTimer.stop();
	returnTimer.stop();
}
void ShareScreen::Pause()
{
	console()<< "PAUSE ShareScreen:: "<<std::endl;
}
void ShareScreen::Resume()
{
	console()<< "RESUME ShareScreen:: "<<std::endl;
}
void ShareScreen::HandleEvents(LocationEngine* game)
{

}

void ShareScreen::MouseEvents( LocationEngine* game )
{
	returnTimer.stop();
	returnTimer.start();

	//if (CONNECTION_PROBLEM) THREAD_FREE = true;
	if(_game->isAnimationRunning() || SOCIAL_POPUP != SOCIAL_NONE || THREAD_FREE == false) return;	

	MouseEvent event = game->getMouseEvent();

	if( event.isLeftDown() )
	{
		if (fbButtonCoord.contains(event.getPos()))
		{
			SOCIAL_POPUP = SOCIAL_FACEBOOK;	
			//SOCIAL_POPUP = SOCIAL_TWITTER;				
			socialForm.show(SOCIAL_POPUP);
		}
		else if (vkButtonCoord.contains(event.getPos()))
		{
			SOCIAL_POPUP = SOCIAL_VKONTAKTE;		
			socialForm.show(SOCIAL_POPUP);
		}
		else if (PhotoUploadReady && emailButtonCoord.contains(event.getPos()))		
		{
			SOCIAL_POPUP = SOCIAL_EMAIL;
			emailForm.show();
		}
		else if (state == 2)
		{
			if (startAgainButtonCoord.contains(event.getPos()))
			{
				_game->ChangeState(Instruction::Instance());
			}
			else if (repeatPrintButtonCoord.contains(event.getPos()))
			{
				if (isNowPrinting == false)
				{
					printerTimer.stop();
					printerTimer.start();
					printHandler();

					if (CONNECTION_PROBLEM == false) 
						Server::sendToServerPrintInfo();	
				}				
			}
		}		
	}
	event.setHandled(true );
}

void ShareScreen::Update(LocationEngine* game) 
{
	
	int sec = int(printerTimer.getSeconds());
	
	if (sec >= Params::printerJobSeconds && state == 1 && printerTimer.isStopped() == false)
	{
		state = 2;
		
		_game->animationRunning(true);

		float _height = 300;	
		timeline().apply( &socialCoords, Vec2f(606.0f, _height), 0.6f, EaseOutQuart()).delay(0.2f);			
		timeline().apply( &emailCoords,  Vec2f(1010.0f, _height), 0.6f, EaseOutQuart()).delay(0.3f);			
		timeline().apply( &telCoords,    Vec2f(1414.0f, _height), 0.6f, EaseOutQuart()).delay(0.4f).finishFn( bind( &ShareScreen::animationFinish, this ) );

		fbButtonCoord			= Rectf ( 666.0f, 586.0f-120.0f, 666.0f + 266.0f,     586.0f+133.0f-120.0f );
		vkButtonCoord			= Rectf ( 666.0f, 586.0f+142.0f- 120.0f, 666.0f + 266.0f, 586.0f+142.0f+133.0f- 120.0f );
		emailButtonCoord		= Rectf ( 1070.0f, 586.0f- 120.0f, 1070.0f + 266.0f,  586.0f+ 276.0f - 120.0f);

		textShow = text2;

		colorBtns = ColorA(1.0f, 1.0f, 1.0f, 0.0f);
		timeline().apply( &colorBtns,ColorA(1.0f, 1.0f, 1.0f, 1.0f), 0.5f, EaseOutQuart());		
	}

	if (sec >= Params::printerJobSeconds && printerTimer.isStopped() == false)
	{
		printerTimer.stop();
		isNowPrinting = false;		
	}

	sec = int(returnTimer.getSeconds());
	if (sec >= Params::lastScreenReturnSeconds)
	{
		if (THREAD_FREE == false) 
		{
			returnTimer.stop();
			returnTimer.start();			
		}
		else _game->ChangeState(Instruction::Instance());
	}

	if (SOCIAL_POPUP == SOCIAL_FACEBOOK || SOCIAL_POPUP == SOCIAL_VKONTAKTE|| SOCIAL_POPUP == SOCIAL_TWITTER)
	{		
		socialForm.update();
	}
}

void ShareScreen::printHandler() 
{
	isNowPrinting = true;	
	int status	  = printerWnd::Printer::print();
	console()<<"PRINTER STATUS   :::"<<status<< std::endl;
}

void ShareScreen::preRenderOneFrame() 
{
	int monthHeight = 111, dayHeight= -15;

	gl::enableAlphaBlending();
	gl::clear( Color( 0.0f, 0.0f, 0.0f ) );	
	gl::color(Color( 1.0f, 1.0f,  1.0f) );		
		
	gl::draw(photoTemplateTemp);
	gl::pushMatrices();
		gl::translate(Vec2f((photoTemplateTemp.getWidth()-monthInfo.tex.getWidth())*0.5f, (float)monthHeight));//monthInfo.coord1);
		gl::draw(monthInfo.tex);
	gl::popMatrices();
			
	TextLayout _text;
	_text.setFont( dayFont );
	_text.setColor( Color::hex(0x313327));
	_text.addLine(to_string(current_day));
	 textTexture = gl::Texture( _text.render( true, true ) );
	
	gl::pushMatrices();		
		gl::translate((photoTemplateTemp.getWidth()- textTexture.getWidth())*0.5f,(float)dayHeight);//73+offset, 88);
		gl::draw( textTexture);
	gl::popMatrices();

	_topSurface = copyWindowSurface(Area(0,0, photoTemplateTemp.getWidth(),photoTemplateTemp.getHeight()));	
	gl::disableAlphaBlending();
}

void ShareScreen::animationFinish()
{
	_game->animationRunning(false);
}

void ShareScreen::Draw(LocationEngine* game) 
{
	
	if (!saveImage)
	{			
		preRenderOneFrame();
		gl::clear( Color::black() );
		saveImage = true;		
		return;
	}
	
	gl::clear( Color::black());
	gl::enableAlphaBlending();

	gl::color(fonAlpha);			
	gl::draw( *fon3);

	gl::pushMatrices();			
	gl::translate(Vec2f(613.0f,116.0f));
		gl::draw(*textShow);
	gl::popMatrices();	
	
	gl::color(Color::white());
	gl::pushMatrices();			
		gl::translate(cracksCoords);
		gl::draw( *cracks);	
	gl::popMatrices();	

	gl::pushMatrices();			
		gl::translate(socialBottomBlockCoord);
		gl::draw(*socialBottomBlock);	
	gl::popMatrices();	
	
	gl::pushMatrices();	
		gl::color(textAlpha);	
		gl::translate(Vec2f(634.0f, 146.0f));	
		gl::draw( *endText);
	gl::popMatrices();	

	gl::color(Color::white());

	gl::pushMatrices();	
		gl::translate(socialCoords);	
		gl::draw(*socialpl);

		gl::translate(Vec2f(60.0f, 166.0f));		
		gl::draw(*facebookBtn);
		gl::translate(Vec2f(0.0f, 142.0f));
		gl::draw(*vkbtn);	
	gl::popMatrices();	

	gl::pushMatrices();	
		gl::translate(emailCoords);	
		gl::draw(*emailpl);
		gl::translate(Vec2f(60.0f, 166.0f));	
		gl::draw(*emailBtn);
	gl::popMatrices();	

	gl::pushMatrices();	
		gl::translate(telCoords);	
		gl::draw( *telpl);	

		if (CONNECTION_PROBLEM == false)
		{
			qrCodeBlock.draw();
		}
		else
		{
			qrCodeBlock.drawError();
		}	
	gl::popMatrices();	

	gl::pushMatrices();	
		gl::translate(Vec2f(168.0f, 90.0f));	
		gl::color(shadowAlpha);	
		gl::draw( *shadowPhoto);
	gl::popMatrices();
	
	if (PHOTO_POLOSA_READY_TO_SHOW)
	{
		gl::color(Color::white());
		gl::pushMatrices();	
			gl::translate(photoReadyCoords);
			gl::scale(Vec2f(0.43f,0.43f));
			gl::rotate(2.0f);
			gl::draw( photoReadySurf);
			gl::translate(Vec2f(30.0f, 10.0f) );	
		gl::popMatrices();
	}

	gl::color(colorBtns);	
	gl::pushMatrices();	
		gl::translate(Vec2f(1168.0f, 827.0f));		
		gl::draw( *startAgain);
	gl::popMatrices();

	gl::pushMatrices();	
		gl::translate(Vec2f(158.0f, 830.0f));	
		if (isNowPrinting && state == 2)
		{		
			printAgain = printAgainDisable;
		}
		else printAgain = printAgainEnable;
		gl::draw( *printAgain);
	gl::popMatrices();

	gl::color(Color::white());

	if (SOCIAL_POPUP == SOCIAL_EMAIL) 
	{
		emailForm.draw();	
	}
	else if (SOCIAL_POPUP == SOCIAL_FACEBOOK || SOCIAL_POPUP == SOCIAL_VKONTAKTE|| SOCIAL_POPUP == SOCIAL_TWITTER)
	{
		socialForm.draw();		
	}
}

void ShareScreen::SendToServerImage(string _name )
{	
	Params::link = "none";

	fs::path path = Params::getPhotosStorageDirectory();
	DataSourceRef urlRequest =	loadFile( path / fs::path( _name ));	

	Buffer bf = Buffer(urlRequest);
	string strj = toBase64(bf) + to_string(BIG_PHOTO_HEIGHT);

	string jSonstr =  Curl::postImage( SERVER"/save.php", strj);
	
	 if (jSonstr.length()>=1)
	 {
		JsonTree jTree;
		try 
		{
			jTree = JsonTree(jSonstr);
			string success = jTree.getChild("success").getValue();
		
			if (success=="1")
			{
				console()<<"SERVER SEND ALL FUCKING DATA"<<std::endl;
			
				Params::sessionId = jTree.getChild("id").getValue();	
				Params::link = jTree.getChild("link").getValue();
			
				Buffer buff = fromBase64(jTree.getChild("data").getValue());
				qrCodeBlock.setTextureString(jTree.getChild("data").getValue());	

				qrCodeBlock.setLink(jTree.getChild("link").getValue(), dayFont);
				qrCodeBlock.isReady = true;
				PhotoUploadReady = true;
				CONNECTION_PROBLEM = false;
				return;
			}
			else  CONNECTION_PROBLEM = true;

		}
		catch (... )
		{
			 CONNECTION_PROBLEM = true;
		}
	 }

	 console()<<"==========================  SERVER ERROR SORRY.....==============================="<<std::endl;
	 CONNECTION_PROBLEM = true;
}
