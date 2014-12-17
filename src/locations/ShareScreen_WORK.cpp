#include "ShareScreen.h"
#include "SocialShare.h"
#include "Instruction.h"

ShareScreen ShareScreen::ShareScreenState;
std::string ShareScreen::FILE_NAME_TO_PRINT		= "template.bmp";
std::string ShareScreen::FILE_NAME_TO_MAIL		= "mail.jpg";
std::string ShareScreen::SOCIAL_POPUP			= "NONE";
bool ShareScreen::CONNECTION_PROBLEM			= false;
bool ShareScreen::PhotoUploadReady				= false;
bool ShareScreen::saveImage						= false;
bool ShareScreen::PHOTO_POLOSA_READY_TO_SHOW	= false;

bool ShareScreen::EMAIL_ENABLE			= false;
bool ShareScreen::VKONTAKTE_ENABLE		= false;
bool ShareScreen::FACEBOOK_ENABLE		= false;


void ShareScreen::Setup()
{
	fon3				= gl::Texture( loadImage( loadAsset( "locations/05/05_bg-1.jpg" ) )) ;
	endText				= gl::Texture(loadImage(getAssetPath("locations/05/05_15sec.png")));
	shadowPhoto			= gl::Texture(loadImage(getAssetPath("locations/05/05_shadow.png")));
	cracks				= gl::Texture(loadImage(getAssetPath("locations/05/05_bg-2.png")));
	socialBottomBlock	= gl::Texture(loadImage(getAssetPath("locations/05/05_bg-3.png")));

	emailBottom	= gl::Texture(loadImage(getAssetPath("bot.png")));

	gl::Texture  preloader = gl::Texture( loadImage( loadAsset( "locations/05/05_preloader.png" ) ) );
	PreloaderCircle::loadTexture(preloader);

	text1				= gl::Texture(loadImage(getAssetPath("locations/05/05_15sec.png")));
	text2				= gl::Texture(loadImage(getAssetPath("locations/05/05_finish.png")));
	textShow = text1;

	startAgain			= gl::Texture(loadImage(getAssetPath("locations/05/05_repeat.png")));
	printAgainEnable	= gl::Texture(loadImage(getAssetPath("locations/05/05_print.png")));
	printAgainDisable	= gl::Texture(loadImage(getAssetPath("locations/05/05_print_start.png")));

	printAgain = printAgainEnable;	
	
	telpl				= gl::Texture(loadImage(getAssetPath("telpl.png")));
	emailpl				= gl::Texture(loadImage(getAssetPath("emailpl.png")));
	socialpl			= gl::Texture(loadImage(getAssetPath("socialpl.png")));

	vkbtn				= gl::Texture(loadImage(getAssetPath("vkbtn.png")));
	facebookBtn			= gl::Texture(loadImage(getAssetPath("facebookBtn.png")));
	emailBtn			= gl::Texture(loadImage(getAssetPath("emailBtn.png")));

	photoTemplate		= gl::Texture(loadImage(getAssetPath("print/print_toyota.png")));
	photoTemplateSurface= Surface(photoTemplate);
	photoTemplateTemp	= gl::Texture(loadImage(getAssetPath("print/top.png")));

	monthJulyTex		= gl::Texture(loadImage(getAssetPath("print/july.png")));
	monthAugTex			= gl::Texture(loadImage(getAssetPath("print/aug.png")));
	monthSeptTex		= gl::Texture(loadImage(getAssetPath("print/sept.png")));

	dayFont				= Font( loadFile(getAssetPath("fonts/ToyotaDisplay_Bd.ttf")), 80 );
	
	qrCodeBlock.setup();
	touchKeyboard.setup(getWindow());

	touchKeyboard.buttonTouched.connect( 
		boost::bind(&ShareScreen::virtualKeyBoardTouch, this) 
		);

	touchKeyboard.threadIn.connect( 
		boost::bind(&ShareScreen::threadInHandler, this) 
		);
	
	touchKeyboard.threadOut.connect( 
		boost::bind(&ShareScreen::threadOutHandler, this) 
		);

	socialShare::SocialShare::setup();

	printerWnd::Printer::printerLetterSize = DMPAPER_BOOTH_TWO_PHOTOS;
	int status = printerWnd::Printer::applySettings();
}

void ShareScreen::threadInHandler()
{
	THREAD_FREE = false;
	console()<<"THREAD_FREE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<<THREAD_FREE<<std::endl;
}

void ShareScreen::threadOutHandler()
{
	THREAD_FREE = true;
	console()<<"THREAD_FREE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<<THREAD_FREE<<std::endl;
}

void ShareScreen::virtualKeyBoardTouch()
{
	if(touchKeyboard.getLastCode() == "closeEmail")
		SOCIAL_POPUP = "NONE";
}

void ShareScreen::Init( LocationEngine* game)
{
	_game = game;
	_game->animationRunning(true);

	PHOTO_POLOSA_READY_TO_SHOW = false;

	EMAIL_ENABLE		= false;
	VKONTAKTE_ENABLE	= false;
	FACEBOOK_ENABLE		= false;


	textShow = text1;

	THREAD_FREE		= true;
	saveImage		= false;
	isNowPrinting	= false;
	SOCIAL_POPUP	= "NONE";

	returnTimer.start();
	printerTimer.stop();	

	resetAnimationParams();		

	time_t     now = time(0);
	struct tm  tstruct;
	char       buf[80];
	tstruct = *localtime(&now);

	current_day = tstruct.tm_mday;
	current_month = tstruct.tm_mon;		

	if (current_month  == 6)
	{
		monthInfo.tex  = monthJulyTex;
		monthInfo.coord1 = Vec2f(93, 213);
		monthInfo.coord2 = Vec2f(697, 213);
	}
	else if (current_month  == 7)
	{
		monthInfo.tex  = monthAugTex;
		monthInfo.coord1 = Vec2f(73, 213);
		monthInfo.coord2 = Vec2f(677, 213);

	}
	else if (current_month  == 8)
	{
		monthInfo.tex  = monthSeptTex;
		monthInfo.coord1 = Vec2f(63, 213);
		monthInfo.coord2 = Vec2f(667, 213);
	}

	PhotoUploadReady = false;
	qrCodeBlock.isReady = false;
	qrCodeBlock.isRender = false;
	CONNECTION_PROBLEM = false;
	
	state = 1;

	fbButtonCoord			= Rectf ( 666, 586, 666 + facebookBtn.getWidth(),586+facebookBtn.getHeight() );
	vkButtonCoord			= Rectf ( 666, 586+142, 666 + vkbtn.getWidth(),586+142+vkbtn.getHeight() );
	emailButtonCoord		= Rectf ( 1070, 586, 1070 + emailBtn.getWidth(),586+emailBtn.getHeight() );


	startAgainButtonCoord	= Rectf ( 1168, 827, 1168 + startAgain.getWidth(),827 + startAgain.getHeight() );
    repeatPrintButtonCoord  = Rectf ( 158, 830, 158 + printAgain.getWidth(),830 + printAgain.getHeight() );

	Params::sessionId = "";

}

void ShareScreen::resetAnimationParams()
{
	float __delay = 0.0;
	fonAlpha = ColorA(1.0f, 1.0f, 1.0f, 0.0f);
	timeline().apply( &fonAlpha,ColorA(1.0f, 1.0f, 1.0f, 1.0f), 0.5f, EaseOutQuart()).delay(__delay);	

	cracksCoords = Vec2f(-cracks.getWidth(), 0);
	timeline().apply( &cracksCoords, Vec2f(0, 0), 0.6f, EaseOutQuart()).delay(__delay);	

	float _height = 420;
	socialCoords  = Vec2f(608, HEIGHT);
	timeline().apply( &socialCoords, Vec2f(606, _height), 0.6f, EaseOutQuart()).delay(0.5 +__delay);	

	emailCoords  = Vec2f(1012, HEIGHT);
	timeline().apply( &emailCoords, Vec2f(1010, _height), 0.6f, EaseOutQuart()).delay(0.7 + __delay);	
		
	telCoords  = Vec2f(1416, HEIGHT);
	timeline().apply( &telCoords, Vec2f(1414, _height), 0.6f, EaseOutQuart()).delay(0.9 +__delay).finishFn( bind( &ShareScreen::StartanimationFinish, this ) );

	textAlpha= ColorA(1.0f, 1.0f, 1.0f, 0.0f);	
	
	socialBottomBlockCoord= Vec2f(0, HEIGHT);
	timeline().apply( &socialBottomBlockCoord, Vec2f(0, HEIGHT- socialBottomBlock.getHeight()), 0.6f, EaseOutQuart());
	
	photoReadyCoords = Vec2f(-(602-12), 68);

	shadowAlpha = ColorA(0.0f, 0.0f, 0.0f, 0.0f);
	colorBtns = ColorA(1.0f, 1.0f, 1.0f, 0.0f);
}

void ShareScreen::saveImageToMail(Surface screenshot)
{	
	fs::path path = getAppPath();
	writeImage( path / fs::path( FILE_NAME_TO_MAIL ), screenshot);		
}

void ShareScreen::saveImageToPrint(Surface screenshot)
{	
	fs::path path = getAppPath();	
	writeImage( path / fs::path( FILE_NAME_TO_PRINT ), screenshot);		
}

Surface ShareScreen::resizeScreenshot(Surface screenshot, float width, float height)
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



void ShareScreen::MouseEvents( LocationEngine* game )
{
	returnTimer.stop();
	returnTimer.start();

	if (THREAD_FREE == false)	return;
	
	MouseEvent event = game->getMouseEvent();
	
	if(_game->isAnimationRunning()) return;	

	if (SOCIAL_POPUP != "NONE") return; 

	if( event.isLeftDown() )
	{
		if (CONNECTION_PROBLEM == false && PhotoUploadReady && fbButtonCoord.contains(event.getPos()))
		{
			console()<<"  FACEBOOOOOOOOOOK  "<<std::endl;
			SOCIAL_POPUP = "FACEBOOK";
			touchKeyboard.connect();
			touchKeyboard.showFacebook();
		}
		else if (CONNECTION_PROBLEM == false && PhotoUploadReady && vkButtonCoord.contains(event.getPos()))
		{
			console()<<"  VKKKKKKKKKKK  "<<std::endl;
			SOCIAL_POPUP = "VK";
			touchKeyboard.connect();
			touchKeyboard.showVkontakte();			
		}
		else if (CONNECTION_PROBLEM == false && PhotoUploadReady && emailButtonCoord.contains(event.getPos()))
		{
			console()<<"  EMAAAAIl  "<<std::endl;
			SOCIAL_POPUP = "EMAIL";
			touchKeyboard.connect();
			touchKeyboard.show();
		}
		else if (startAgainButtonCoord.contains(event.getPos()) && state == 2 && THREAD_FREE)
		{
			console()<<"  startAgainButtonCoord  "<<THREAD_FREE<<std::endl;
			_game->ChangeState(Instruction::Instance());
		}
		else if (repeatPrintButtonCoord.contains(event.getPos())&& state == 2 && THREAD_FREE)
		{
			if (isNowPrinting == false)
			{
				printerTimer.stop();
				printerTimer.start();
				SendToServerPrintInfo();
				printHandler();
			}
			console()<<"  repeatPrintButtonCoord  "<<  isNowPrinting<<std::endl;
		}
	}

	event.setHandled(true );
}

void ShareScreen::prepareToSaveAndPrintImage()
{
		int resize_width  = 591;
		int resize_height = 377;			
		
		photoCollage = photoTemplateSurface;		
		photoCollage.copyFrom(_topSurface, Area(0,0,_topSurface.getWidth(),_topSurface.getHeight()));
		
		for (int i = 0; i < Params::photosStorage.size(); i++)
		{	
			Surface s = resizeScreenshot(Params::photosStorage[i], resize_width, resize_height);
			Vec2f offset = Vec2f(17, (resize_height + 4) * i + 367);
			photoCollage.copyFrom(s, Area(0, 0, resize_width, resize_height), offset);	
		}

		for (int i = 0; i < Params::photosStorage.size(); i++)
		{	
			Surface s = resizeScreenshot(Params::photosStorage[i], resize_width, resize_height);
			Vec2f offset = Vec2f(620,(resize_height+4)*i+ 367);
			photoCollage.copyFrom(s, Area(0,0,resize_width,resize_height),offset);
		}

		sendToMailPhoto = Surface(BIG_PHOTO_WIDTH, BIG_PHOTO_HEIGHT*3, true);

		Surface emailBottomsurf = Surface(emailBottom);

		for (int i = 0; i < Params::photosStorage.size(); i++)
		{
			Vec2f offset = Vec2f(0, BIG_PHOTO_HEIGHT*i);
			sendToMailPhoto.copyFrom(Params::photosStorage[i], Area(0, 0, BIG_PHOTO_WIDTH, BIG_PHOTO_HEIGHT), offset);
			sendToMailPhoto.copyFrom(emailBottomsurf, Area(0, 0, emailBottomsurf.getWidth(), emailBottomsurf.getHeight()), offset + Vec2f(0,BIG_PHOTO_HEIGHT-emailBottomsurf.getHeight()));
		}

		console()<<"PHOTO PREVIEW READ TO SHOW!!!"<<std::endl;

		photoReadySurf = Surface(602-12,1797-28, true);////photoCollage.getWidth()*0.5,photoCollage.getHeight(),true);

		console()<<"PHOTO PREVIEW INIT!!"<<std::endl;
		

		photoReadySurf.copyFrom(photoCollage, Area(12, 28, 602,1797),-Vec2f(12,28));	

	
}

void ShareScreen::HandleEvents(LocationEngine* game)
{

}

void ShareScreen::Update(LocationEngine* game) 
{
	double sec = int(printerTimer.getSeconds());
	
	if (sec >= Params::printerJobSeconds && state == 1 && printerTimer.isStopped() == false)
	{
		state = 2;
		
		_game->animationRunning(true);

		float _height = 300;	
		timeline().apply( &socialCoords, Vec2f(606, _height), 0.6f, EaseOutQuart()).delay(0.2);			
		timeline().apply( &emailCoords,  Vec2f(1010, _height), 0.6f, EaseOutQuart()).delay(0.3);			
		timeline().apply( &telCoords,    Vec2f(1414, _height), 0.6f, EaseOutQuart()).delay(0.4).finishFn( bind( &ShareScreen::animationFinish, this ) );

		fbButtonCoord			= Rectf ( 666, 586-120, 666 + facebookBtn.getWidth(),     586+facebookBtn.getHeight()-120 );
		vkButtonCoord			= Rectf ( 666, 586+142- 120, 666 + vkbtn.getWidth(),        586+142+vkbtn.getHeight()- 120 );
		emailButtonCoord		= Rectf ( 1070, 586- 120, 1070 + emailBtn.getWidth(),  586+emailBtn.getHeight() - 120);

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

	if (SOCIAL_POPUP == "VK" || SOCIAL_POPUP == "FACEBOOK")
	{		
		touchKeyboard.update();
	}
}

void ShareScreen::savePrintServer() 
{
	THREAD_FREE = false;	
	console()<<"savePrintServerThread!!!!!!!!!!!! "<<std::endl;
	try{
		savePrintServerThread = shared_ptr<thread>( new thread( bind( &ShareScreen::savePrintServerThreadHandler, this ) ) );
	}
	catch(...)
	{
		console()<<"================================================================THREAD EXEPTION!!!!!!!!!!!! "<<std::endl;
	}
}

void ShareScreen::saveAndPrintImage() 
{
	prepareToSaveAndPrintImage();
	saveImageToMail(sendToMailPhoto);		
	saveImageToPrint(photoCollage);	

	PHOTO_POLOSA_READY_TO_SHOW = true;
	photoReadyCoords = Vec2f(-(602-12), 68);
	timeline().apply( &photoReadyCoords, Vec2f(213, 68), 0.5f, EaseOutQuart()).delay(1.3);	
	shadowAlpha = ColorA(0.0f, 0.0f, 0.0f, 0.0f);
	timeline().apply( &shadowAlpha,ColorA(0.0f, 0.0f, 0.0f, 1.0f), 0.5f, EaseOutQuart()).delay(1.3);	
}

bool ShareScreen::savePrintServerThreadHandler() 
{
	console()<<"INSIDE THREAD!!!!!!!!!!!! "<<std::endl;
	ci::ThreadSetup threadSetup; // instantiate this if you're talking to Cinder from a secondary thread	

	console()<<"Go THREAD!!!!!!!!!!!! "<<std::endl;
	
	console()<<"printHandler    :::"<< std::endl;
	//saveAndPrintImage() ;
	try{
		printHandler();	
	}
	catch(...)
	{
		console()<<"=============printHandler EXEPTION!!!!!!!!!!!! "<<std::endl;
	}
	console()<<"sendToServer    :::"<< std::endl;
	try{
		sendToServer("init");
	}
	catch(...)
	{
		console()<<"=============sendToServer EXEPTION!!!!!!!!!!!! "<<std::endl;
	}
	console()<<"TRY DEATTACH !!!!!!!!!!!! "<<std::endl;
	savePrintServerThread->detach();
	console()<<"DEATTACHED !!!!!!!!!!!! "<<std::endl;
	THREAD_FREE = true;
	return true;
}

void ShareScreen::printHandler() 
{
	console()<<"START PRINTER    :::"<< std::endl;

	isNowPrinting = true;	
	int status	  = printerWnd::Printer::print();

	console()<<"PRINTER STATUS   :::"<<status<< std::endl;
}

void ShareScreen::preRenderOneFrame() 
{
		gl::enableAlphaBlending();
		gl::clear( Color( 0, 0, 0 ) );	
		gl::color(Color( 1, 1,  1) );
		
		gl::draw(photoTemplateTemp);

		gl::pushMatrices();
			gl::translate(monthInfo.coord1);
			gl::draw(monthInfo.tex);
		gl::popMatrices();

		gl::pushMatrices();
			gl::translate(monthInfo.coord2);
			gl::draw(monthInfo.tex);
		gl::popMatrices();

				
		TextLayout _text;
		_text.setFont( dayFont );
		_text.setColor( Color::hex(0x5b6c46));
		_text.addLine(to_string(current_day));
		textTexture = gl::Texture( _text.render( true, true ) );

		float offset = 0;	
		gl::pushMatrices();		
		if (textTexture.getWidth() <100) offset = (176-textTexture.getWidth())*0.25;
		gl::translate(73+offset, 88);
		gl::draw( textTexture);
		gl::popMatrices();

		gl::pushMatrices();
		if (textTexture.getWidth() <100) offset = (176-textTexture.getWidth())*0.25;
		gl::translate(677+offset, 88);
		gl::draw( textTexture);
		gl::popMatrices();

		_topSurface = copyWindowSurface(Area(0,0, 1280,350));		

		gl::disableAlphaBlending();
}
void ShareScreen::StartanimationFinish()
{
	console()<<"StartanimationFinish ::::::::::::::::::::::::::"<< std::endl;
	_game->animationRunning(false);	

	saveAndPrintImage();
	savePrintServer();

	printerTimer.stop();
	printerTimer.start();

	SendToServerPrintInfo();
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
		gl::clear( Color( 0, 0, 0 ) );
		saveImage = true;		
		return;
	}	

	if (SOCIAL_POPUP == "VK")
	{
		touchKeyboard.drawVkontakte();
		return;
	}
	if (SOCIAL_POPUP == "FACEBOOK")
	{
		touchKeyboard.drawFacebook();
		return;
	}
	
	gl::clear( Color( 0, 0, 0 ) );
	gl::enableAlphaBlending();

	gl::color(fonAlpha);			
	gl::draw( fon3);

	gl::pushMatrices();			
	gl::translate(Vec2f(613,116));
		gl::draw( textShow);
	gl::popMatrices();	
	
	gl::color(Color::white());
	gl::pushMatrices();			
		gl::translate(cracksCoords);
		gl::draw( cracks);	
	gl::popMatrices();	

	gl::pushMatrices();			
		gl::translate(socialBottomBlockCoord);
		gl::draw( socialBottomBlock);	
	gl::popMatrices();	
	
	gl::pushMatrices();	
		gl::color(textAlpha);	
		gl::translate(Vec2f(634, 146));	
		gl::draw( endText);
	gl::popMatrices();	

	gl::color(Color::white());

	gl::pushMatrices();	
		gl::translate(socialCoords);	
		gl::draw(socialpl);

		gl::translate(Vec2f(60, 166));		
		gl::draw(facebookBtn);
		gl::translate(Vec2f(0, 142));
		gl::draw(vkbtn);	
	gl::popMatrices();	

	gl::pushMatrices();	
		gl::translate(emailCoords);	
		gl::draw(emailpl);
		gl::translate(Vec2f(60, 166));	
		gl::draw(emailBtn);
	gl::popMatrices();	

	gl::pushMatrices();	
		gl::translate(telCoords);	
		gl::draw( telpl);	

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
		gl::translate(Vec2f(168, 90));	
		gl::color(shadowAlpha);	
		gl::draw( shadowPhoto);
	gl::popMatrices();

	//console()<<"PHOTO_POLOSA_READY_TO_SHOW "<<PHOTO_POLOSA_READY_TO_SHOW<<std::endl;
	if (PHOTO_POLOSA_READY_TO_SHOW)
	{
		//console()<<"GO"<<std::endl;
		gl::color(Color::white());
		gl::pushMatrices();	
			gl::translate(photoReadyCoords);
			gl::scale(Vec2f(0.43,0.43));
			gl::rotate(2.0f);
			gl::draw( photoReadySurf);
			gl::translate(Vec2f(30, 10) );	
		gl::popMatrices();
	}

	gl::color(colorBtns);	
	gl::pushMatrices();	
		gl::translate(Vec2f(1168, 827));		
		gl::draw( startAgain);
	gl::popMatrices();

	gl::pushMatrices();	
		gl::translate(Vec2f(158, 830));	
		if (isNowPrinting && state == 2)
		{		
			printAgain = printAgainDisable;
		}
		else printAgain = printAgainEnable;
		gl::draw( printAgain);
	gl::popMatrices();

	gl::color(Color::white());
	socialDraw();
}

void ShareScreen::socialDraw() 
{
	if (SOCIAL_POPUP == "EMAIL") touchKeyboard.draw();
}

void ShareScreen::sendToServer(string _name )
{
	Params::link = "none";

	fs::path path = getAppPath();
	DataSourceRef urlRequest =	loadFile( path / fs::path( FILE_NAME_TO_MAIL ));	

	//DataSourceRef urlRequest =  loadFile(FILE_NAME);

	Buffer bf = Buffer(urlRequest);
	string strj = toBase64(bf) + to_string(BIG_PHOTO_HEIGHT);// add height 3 characters for server!!!!!!!!!!!!!!!!!!!!!!!
	const char * send_img = strj.c_str();

	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
	{
		// cout << "WSAStartup failed.\n";	
		console()<<"!!!!!!!!!!!!!!!!!!!!!!!!!! CONNECTION PROBLEM !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<<std::endl;
		CONNECTION_PROBLEM = true;
		return;
	}	

	SOCKET Socket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	struct hostent *host;
	host = gethostbyname("92.63.102.99");
	
	SOCKADDR_IN SockAddr;	

	if (host)
	{
		SockAddr.sin_port   = htons(82);
		SockAddr.sin_family = AF_INET;		
		SockAddr.sin_addr.s_addr = *((unsigned long*)host->h_addr);		
	}
	else
	{
		console()<<"!!!!!!!!!!!!!!!!!!!!!!!!!! CONNECTION PROBLEM !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<<std::endl;
		CONNECTION_PROBLEM = true;
		return;
	}
  
	console()<<"Connecting...\n"<<std::endl;
	
   if(connect(Socket,(SOCKADDR*)(&SockAddr),sizeof(SockAddr)) != 0)
   {
	   console()<<"!!!!!!!!!!!!!!!!!!!!!!!!!! CONNECTION PROBLEM !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<<std::endl;
	   CONNECTION_PROBLEM = true;
	   return;
   }
   
   std::stringstream ss;
   ss << strj.length();
   std::string _bytes = ss.str();
   
    string header = string("POST /save.php HTTP/1.1\r\nHost: www.leto.toyota.ru\r\nConnection: close\r\nContent-Length:")+_bytes;//+"\r\n\r\n";
   header+="\r\n\r\n";
   const char* s = header.c_str();

   send(Socket,s, strlen(	s),0);
   send(Socket, send_img, strlen(send_img), 0);

   char buffer[10000];
   int nDataLength;

	while ((nDataLength = recv(Socket,buffer,10000,0)) > 0)
	{        
		 int i = 0;
		 while (buffer[i] >= 32 || buffer[i] == '\n' || buffer[i] == '\r')
		 {
			   i += 1;
		 }		
	 }

	 closesocket(Socket);
	 WSACleanup();	
	 bool start = false;
	 string jSonstr="";

	 for (int i = 0; i < 10000; i++)
	 {
		 if (buffer[i] == '{')
		 {
			 start = true;			
		 }

		 if(start)
		 {
			jSonstr+= buffer[i];
		 }

		 if (buffer[i] == '}')
		 {
			 break;
		 }
	 }	 
	
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
				//console()<<"jTree.getChild(link).getValue()"<<jTree.getChild("link").getValue()<<std::endl;	
				//console()<<"id"<<jTree.getChild("id").getValue()<<std::endl;			

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
	 
	// SERVER_ERROR = true;
}
void ShareScreen::SendToServerPrintInfo()
{
	std::map<string,string> strings;
	strings.insert(pair<string, string>( "action" , "print_count"));
	strings.insert(pair<string, string>( "cnt" ,  "1"));	
	string request =  Curl::post( "92.63.102.99:82/save.php", strings);

	JsonTree jTree;

	try 
	{
		jTree = JsonTree(request);
		console()<<"PHOTO NUMS   "<< jTree.getChild("cnt").getValue()<<std::endl;
	}
	catch(...)
	{

	}
}