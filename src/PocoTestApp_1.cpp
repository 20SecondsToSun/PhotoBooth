#include "Curl.h"
#include "CinderAwesomium.h"

#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/ImageIO.h"
#include "cinder/gl/Texture.h"
#include "cinder/Json.h"

#include <shellapi.h>
#include <winspool.h>

#include "Printer.h"
#include "SocialShare.h"
#include "TouchKeyboard.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace mndl::curl;

class PocoTestApp : public AppNative {
  public:
	//void prepareSettings( Settings *settings );
	
	void setup();
	//void shutdown();
	void update();
	void draw();
	
	void resize();
	
	void mouseMove( MouseEvent event );	
	void mouseDown( MouseEvent event );	
	void mouseDrag( MouseEvent event );	
	void mouseUp( MouseEvent event );	
	void mouseWheel( MouseEvent event );	
	
	void keyDown( KeyEvent event );
	void keyUp( KeyEvent event );


	void				vkontaktePost();
	void				facebookPost();
	void				emailSend();
	void				printSettings(int paperSize = 501); 
	DWORD				PrintJob(LPTSTR szPrinterName);

private:
	Awesomium::WebCore*		mWebCorePtr;
	Awesomium::WebView*		mWebViewPtr;
	
	gl::Texture				mWebTexture;
	gl::Texture				mLoadingTexture;

	Font					mFont;

	
	Rectf button1Photo;
	Rectf button2Photo;	

	Rectf vkButton, facebookButton, emailButton, printButton;

	bool touchDisable;
	bool socialCall ;
	bool loginClick;
	Rectf allowRect;

	
	void virtualKeyBoardTouch();
	TouchKeyboard touchKeyboard;
};



void PocoTestApp::setup()
{
	setFullScreen(false);
	setWindowSize(1920,1080);
	// set Awesomium logging to verbose
	Awesomium::WebConfig cnf;
	cnf.log_level = Awesomium::kLogLevel_Verbose;

#if defined( CINDER_MAC )
	std::string frameworkPath = ( getAppPath() / "Contents" / "MacOS" ).string();
	cnf.package_path = Awesomium::WebString::CreateFromUTF8( frameworkPath.c_str(), frameworkPath.size() );
#endif

	// initialize the Awesomium web engine
	mWebCorePtr = Awesomium::WebCore::Initialize( cnf );

	// create a webview
	mWebViewPtr = mWebCorePtr->CreateWebView( getWindowWidth(), getWindowHeight() );
	//mWebViewPtr->LoadURL( Awesomium::WebURL( Awesomium::WSLit( "https://oauth.vk.com/authorize?client_id=4452502&redirect_uri=https://oauth.vk.com/blank.html&scope=status,photos,wall,questions&display=page&response_type=token" ) ) );
	//mWebViewPtr->LoadURL( Awesomium::WebURL( Awesomium::WSLit( "https://www.facebook.com/dialog/oauth?client_id=1473762739532829&redirect_uri=https://www.facebook.com/connect/login_success.html" ) ) );
	//mWebViewPtr->Focus();

	// load and create a "loading" icon
	try { mLoadingTexture = gl::Texture( loadImage( loadAsset( "loading.png" ) ) ); }
	catch( const std::exception &e ) { console() << "Error loading asset: " << e.what() << std::endl; }


	//vkontaktePost();
	///facebookPost();
	//emailSend();

	button1Photo = Rectf(0,0,80,30);
	button2Photo = Rectf(0,200,80,230);

	touchDisable = false;
	socialCall = false;
	loginClick = false;
	touchKeyboard.setup(getWindow());

	touchKeyboard.buttonTouched.connect( 
		boost::bind(&PocoTestApp::virtualKeyBoardTouch, this) 
		);



	vkButton        = Rectf(100,100,300,300);
	facebookButton  = Rectf(400,100,600,300);
	emailButton     = Rectf(700,100,1000,300);

	//printerWnd::Printer::printerLetterSize = DMPAPER_BOOTH_TWO_PHOTOS;
	//int status = printerWnd::Printer::applySettings();
	//status	  = printerWnd::Printer::print();
	
	//printSettings(501);
	//PrintJob(L"MITSUBISHI CP70D Series(USB)");
}
void PocoTestApp::virtualKeyBoardTouch() 
{
		console()<<"TOUCHED!!!!!!!!!!!!  "<<touchKeyboard.getLastCode()<<std::endl;
}

void PocoTestApp::printSettings(int paperSize) 
{
	LPWSTR printerName(L"MITSUBISHI CP70D Series(USB)");
	HANDLE print_handle;
	PRINTER_DEFAULTS pd;
	BOOL bFlag;	
	DWORD dwNeeded = 0;
	DEVMODE *pDevMode = NULL;	
	LONG lFlag;


	ZeroMemory(&pd, sizeof(pd));
	pd.DesiredAccess = PRINTER_ALL_ACCESS;	

	bFlag = OpenPrinter(printerName, &print_handle, &pd);

	if (!bFlag || (print_handle == NULL))
	{
		console()<< "::::::::::::::::  OpenPrinter ERROR ::::::::::::::  "<<std::endl;
		return;
	}
	
	bFlag = GetPrinter(print_handle, 2, 0, 0, &dwNeeded);
	

	 if ((!bFlag) && (GetLastError() != ERROR_INSUFFICIENT_BUFFER) || 
         (dwNeeded == 0))
	{
		ClosePrinter(print_handle);
		console()<< ":::::::::::::::  SOMETHING GONE WRONG ::::::::::::::  "<<std::endl;
		return;
	 }

	PRINTER_INFO_2 *pi2 = (PRINTER_INFO_2  *)::GlobalAlloc(GPTR,dwNeeded);

	if (pi2 == NULL)
	{
		ClosePrinter(print_handle);	
		console()<< " :::::::::::::::::: NOT MEMORY ALLOCATE :::::::::::::::::  "<<std::endl;
		return;
	}

	bFlag = GetPrinter(print_handle, 2, (LPBYTE)pi2, dwNeeded, &dwNeeded);
	if (!bFlag)
	{
		GlobalFree(pi2);
		ClosePrinter(print_handle);
		console()<< ":::::::::::::::  SOMETHING GONE WRONG ::::::::::::::  "<<std::endl;
		return;
	}
	
	if (pi2->pDevMode == NULL)
	{
		dwNeeded = DocumentProperties(NULL, print_handle,
						printerName,
						NULL, NULL, 0);
		if (dwNeeded <= 0)
		{
			GlobalFree(pi2);
			ClosePrinter(print_handle);
			return;
		}

		pDevMode = (DEVMODE *)GlobalAlloc(GPTR, dwNeeded);
		if (pDevMode == NULL)
		{
			GlobalFree(pi2);
			ClosePrinter(print_handle);
			return;
		}

		lFlag = DocumentProperties(NULL, print_handle,
						printerName,
						pDevMode, NULL,
						DM_OUT_BUFFER);

		if (lFlag != IDOK || pDevMode == NULL)
		{
			GlobalFree(pDevMode);
			GlobalFree(pi2);
			ClosePrinter(print_handle);
			return;
		}

		pi2->pDevMode = pDevMode;
	}

	 // check that the driver supports the changes 
    if(pi2->pDevMode->dmFields & DM_PAPERSIZE )
    {
		console()<< "  pi2->pDevMode->dmPaperSize  "<< pi2->pDevMode->dmPaperSize<<std::endl;
		// define the page size as A3
         pi2->pDevMode->dmPaperSize = paperSize;
		// define, which field was changed
         pi2->pDevMode->dmFields |= DM_PAPERSIZE;      
	 }
	console()<< " pi2->pDevMode->dmPaperSize   changed ::  "<< pi2->pDevMode->dmPaperSize<<std::endl;

	/*
	*	Make changes to the DevMode which are supported.
	*/
	console()<< " pi2->pDevMode->dmOrientation  ::  "<< pi2->pDevMode->dmOrientation<<std::endl;
	if (pi2->pDevMode->dmFields & DM_ORIENTATION)
	{
	   /* If the printer supports paper orientation, set it.*/
	   pi2->pDevMode->dmOrientation = DMORIENT_PORTRAIT;// DMORIENT_LANDSCAPE;
	   pi2->pDevMode->dmFields |= DM_ORIENTATION;     
	   
	}
	if (pi2->pDevMode->dmCopies & DM_COPIES)
	{
		pi2->pDevMode->dmCopies = 1;
		pi2->pDevMode->dmCopies |= DM_COPIES;  
	}
	/*console()<< " pi2->pDevMode->dmOrientation changed ::  "<< pi2->pDevMode->dmOrientation<<std::endl;
	console()<< " pi2->pDevMode->dmScale ::  "<< pi2->pDevMode->dmScale<<std::endl;
	console()<< " pi2->pDevMode->dmPanningHeight ::  "<< pi2->pDevMode->dmPanningHeight<<std::endl;
	console()<< " pi2->pDevMode->dmCopies ::  "<< pi2->pDevMode->dmCopies<<std::endl;

	console()<< " pi2->pDevMode->dmPaperLength ::  "<< pi2->pDevMode->dmPaperLength<<std::endl;
	console()<< " pi2->pDevMode->dmPaperWidth ::  "<< pi2->pDevMode->dmPaperWidth<<std::endl;
	console()<< " pi2->pDevMode->dmPaperSize ::  "<< pi2->pDevMode->dmPaperSize<<std::endl;

	console()<< " pi2->pDevMode->dmPelsWidth ::  "<< pi2->pDevMode->dmPelsWidth<<std::endl;
	console()<< " pi2->pDevMode->dmPelsHeight ::  "<< pi2->pDevMode->dmPelsHeight<<std::endl;*/

	
	
//lpMargins = (LPDWORD)( (LPBYTE) pi2->pDevMode +  pi2->pDevMode->dmSize + sizeof( DWORD ) );
//*lpMargins = MAKELONG( 30, 30 ); // this will set the margins to 3 mm


	// Do not attempt to set security descriptor...
	pi2->pSecurityDescriptor = NULL;


	// Make sure the driver-dependent part of devmode is updated...
	 lFlag = DocumentProperties  (NULL, print_handle, printerName,
        		pi2->pDevMode, pi2->pDevMode,
        		DM_IN_BUFFER | DM_OUT_BUFFER);

	if (lFlag != IDOK)
	{
		console()<< "  ERROR!!! not updated !!!!!!!!!!!!!!!!!!!!! ::  "<<std::endl;
		GlobalFree(pi2);
		ClosePrinter(print_handle);
		if (pDevMode)
			GlobalFree(pDevMode);
		return;
	}	

	// Update printer information...
	bFlag = SetPrinter(print_handle, 2, (LPBYTE)pi2, 0);

	if (!bFlag)
	// The driver doesn't support, or it is unable to make the change...
	{
		console()<< "  ERROR!!! not updated !!!!!!!!!!!!!!!!!!!!! ::  "<<std::endl;
		GlobalFree(pi2);
		ClosePrinter(print_handle);
		if (pDevMode)
			GlobalFree(pDevMode);
		return ;
	}

	console()<< "  SetPrinter updated !!!!!!!!!!!!!!!!!!!!! ::  "<<bFlag<<std::endl;

	// Tell other apps that there was a change...
	SendMessageTimeout(HWND_BROADCAST, WM_DEVMODECHANGE, 0L,
			  (LPARAM)(LPCSTR)printerName,
			  SMTO_NORMAL, 1000, NULL);	
}





DWORD PocoTestApp::PrintJob(LPTSTR szPrinterName)//szPrinterName is path of the printer in the network
{
	DWORD	 dwError = 0;

	TCHAR	 szJobName[128] = {0};
	HDC	 hDC = 0;
	DOCINFO	 DocInfo;

	BITMAPINFO	 BmpInfo;
	int	 nRet = 0;

	HBITMAP	 hBmp24 = 0;
	BITMAP	 Bmp24 = {0};

	LOGFONT	 LogFont = {0};
	HFONT	 hFont = 0,
	hDefFont = 0;	
	
	ZeroMemory(&hBmp24, sizeof(HBITMAP));	
	hBmp24 = (HBITMAP)LoadImage(NULL, L"Z:\\1.bmp", IMAGE_BITMAP,0,0,LR_CREATEDIBSECTION|LR_DEFAULTSIZE|LR_LOADFROMFILE|LR_VGACOLOR);

		if( !hBmp24 )
	{
		console()<<"hBmp  ERROR  "<<dwError<<std::endl;
		return 1;
	}	
	
	dwError = GetLastError();
	GetObject(hBmp24, sizeof(BITMAP), &Bmp24);

	hDC = CreateDC(L"WINSPOOL", szPrinterName, NULL, 0);
	
	//4. StartDoc
	memset(&DocInfo, 0, sizeof(DOCINFO));
	DocInfo.cbSize = sizeof(DOCINFO);

	lstrcpy(szJobName, L"Test");
	DocInfo.lpszDocName = szJobName;

	int nJobId = ::StartDoc(hDC, &DocInfo);

	//5. StartPage
	nRet = ::StartPage(hDC);

	//6.b Draw anything you want on page between ::StartPage and ::EndPage
	if ( Bmp24.bmBits )
	{
		memset(&BmpInfo, 0, sizeof(BITMAPINFO));
		BmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		BmpInfo.bmiHeader.biWidth = Bmp24.bmWidth;
		BmpInfo.bmiHeader.biHeight = Bmp24.bmHeight;
		BmpInfo.bmiHeader.biPlanes = Bmp24.bmPlanes;
		BmpInfo.bmiHeader.biBitCount = Bmp24.bmBitsPixel * Bmp24.bmPlanes;
		BmpInfo.bmiHeader.biCompression = BI_RGB;

		nRet = StretchDIBits(hDC, 0, 0, Bmp24.bmWidth, Bmp24.bmHeight, 0, 0, Bmp24.bmWidth, Bmp24.bmHeight, Bmp24.bmBits, &BmpInfo, DIB_RGB_COLORS, SRCCOPY);
	}

	//7. If page drawing is finished, set to end page
	nRet = ::EndPage(hDC);

	//8. If job done, set end of the job
	nRet = ::EndDoc(hDC);
	//release image object
	if ( hBmp24 )
	DeleteObject(hBmp24);

	DeleteDC(hDC);

	return 0;
}

void PocoTestApp::emailSend()
{
	Curl::sendMail();
}

void PocoTestApp::vkontaktePost()
{
	string access_token = "3368715a2e89dfde53e041faee5701da618ba8310946d87730432159ef80da8d059baf5e0721b1fd11567" ;

	std::map<string,string> strings;
	strings.insert(pair<string, string>( "access_token" , access_token));

	string vkRequest =  Curl::post( "https://api.vk.com/method/photos.getWallUploadServer", strings);

	console()<< "getWallUploadServer===============================: " <<vkRequest<<std::endl;

	JsonTree jTree = JsonTree(vkRequest);
	string upload_url = jTree.getChild("response").getChild("upload_url").getValue();
	console()<< " upload_url ======================================= " <<upload_url<<std::endl;
	

	FILE *fileToUpload;
	const char*  filePath = "Z:\\star.png";
 
	fileToUpload= fopen(filePath, "rb");
	if(fileToUpload == NULL)
	{ 
		console()<< " file doesn't exist ((((((" <<std::endl;
		return;
	}
	 

	vkRequest =  Curl::postUpload( upload_url, strings, filePath);

	console()<< " photo uploaded answer ======================================= " <<vkRequest<<std::endl;

	jTree = JsonTree(vkRequest);
	strings.clear();
	strings.insert(pair<string, string>( "server" , jTree.getChild("server").getValue() ));
	strings.insert(pair<string, string>( "photo" , jTree.getChild("photo").getValue() ));
	strings.insert(pair<string, string>( "hash" , jTree.getChild("hash").getValue() ));
	strings.insert(pair<string, string>( "access_token" , access_token ));
	
	vkRequest =  Curl::post( "https://api.vk.com/method/photos.saveWallPhoto",strings);
	ci::app::console()<<"--------------------------photos.saveWallPhoto================   "<<vkRequest<<std::endl;	


	jTree = JsonTree(vkRequest);
	
	JsonTree owner = jTree.getChild( "response" );
        for( JsonTree::ConstIter item = owner.begin(); item != owner.end(); ++item ) {
                console() << "Node: " << item->getKey() << " Value: " << item->getValue<string>() << endl;
        }

   ci::app::console()<<"====================================   "<<owner[0]<<std::endl;	
   ci::app::console()<<"====================================   "<<owner[0].getChild( "id" ).getValue()<<std::endl;	

   string photo_id = owner[0].getChild( "id" ).getValue();

	strings.insert(pair<string, string>( "attachments" , photo_id ));
	strings.insert(pair<string, string>( "access_token" , access_token));
	vkRequest =  Curl::post( "https://api.vk.com/method/wall.post",strings);

	ci::app::console()<<"--------------------------wall.post ================   "<<vkRequest<<"  ___"<<std::endl;	
}


void PocoTestApp::facebookPost()
{
}





void PocoTestApp::update()
{
	// update the Awesomium engine
	mWebCorePtr->Update();

	// create or update our OpenGL Texture from the webview
	if( ph::awesomium::isDirty( mWebViewPtr ) ) 
	{
		try {
			// set texture filter to NEAREST if you don't intend to transform (scale, rotate) it
			gl::Texture::Format fmt; 
			fmt.setMagFilter( GL_NEAREST );

			// get the texture using a handy conversion function
			mWebTexture = ph::awesomium::toTexture( mWebViewPtr, fmt );
		}
		catch( const std::exception &e ) {
			console() << e.what() << std::endl;
		}

		// update the window title to reflect the loaded content
		char title[1024];
		mWebViewPtr->title().ToUTF8( title, 1024 );

		

		//console()<<"UL!!!!!!!!!!!!!!!"<<mWebViewPtr->url().scheme()<<std::endl;
		//console()<<"anchor!!!!!!!!!!!!!!!"<<mWebViewPtr->url().anchor()<<std::endl;

		app::getWindow()->setTitle( title );
	}
}

void PocoTestApp::draw()
{
	gl::clear();
	gl::enableAlphaBlending();	

	gl::color( Color::hex(0xff0000) );
	gl::drawSolidRect(vkButton);	
	gl::color( Color::hex(0x2962FF) );
	gl::drawSolidRect(facebookButton);

	gl::color( Color::hex(0xffff00) );
	gl::drawSolidRect(emailButton);


	if( mWebTexture )
	{
		gl::color( Color::white() );
		gl::pushMatrices();
		//gl::translate(0, -500);
		gl::draw( mWebTexture );
		gl::popMatrices();
	}

	// show spinner while loading 
	if( mLoadingTexture && mWebViewPtr && mWebViewPtr->IsLoading() )
	{
		gl::pushModelView();

		gl::translate( 0.5f * Vec2f( getWindowSize() ) );
		gl::scale( 0.5f, 0.5f );
		gl::rotate( 180.0f * float( getElapsedSeconds() ) );
		gl::translate( -0.5f * Vec2f( mLoadingTexture.getSize() ) );
		
		gl::color( Color::white() );
		gl::enableAlphaBlending();
		gl::draw( mLoadingTexture );
		gl::disableAlphaBlending();

		gl::popModelView();
	}
	else if (socialCall)		
	{	

		if (loginClick)
		{
			char title[1024];
			mWebViewPtr->url().query().ToUTF8( title, 1024 );
			string url = socialShare::SocialShare::urlDecode(title);
			console()<<"query!!!!!!!!   "<< url<<std::endl;
			loginClick = false;
		}
		
		//console()<<"LOADED  FB"<<std::endl;
		allowRect = socialShare::SocialShare::getAllowRec();
		


	}

	if (socialShare::SocialShare::socialType != SOCIAL_NONE)
	{
		gl::color( ColorA(0.0f,1.0f,0.0f,0.5f) );
		gl::drawSolidRect( allowRect );
	}

	touchKeyboard.draw();
	
}


void PocoTestApp::resize()
{
	// resize webview if window resizes
	//if( mWebViewPtr )
	//	mWebViewPtr->Resize( getWindowWidth(), getWindowHeight() );
}

void PocoTestApp::mouseMove( MouseEvent event )
{
	// send mouse events to Awesomium
	ph::awesomium::handleMouseMove( mWebViewPtr, event );
}

void PocoTestApp::mouseDown( MouseEvent event )
{
	if (socialShare::SocialShare::socialType != SOCIAL_NONE)
	{
		allowRect = socialShare::SocialShare::getAllowRec();
		if (allowRect.contains(event.getPos()))
		{
			loginClick = true;
			ph::awesomium::handleMouseDown( mWebViewPtr, event );
		}
		return;
	}

	if (touchDisable) return;

	ph::awesomium::handleMouseDown( mWebViewPtr, event );

	if (vkButton.contains(event.getPos()))
	{		
		console()<<"VK CALL"<<std::endl;
		socialShare::SocialShare::socialType = SOCIAL_VKONTAKTE;
		mWebViewPtr->LoadURL( Awesomium::WebURL( Awesomium::WSLit( socialShare::SocialShare::vkontakteAuthURL ) ) );
		mWebViewPtr->Focus();
		touchDisable = true;
		socialCall = true;
	}
	else if (facebookButton.contains(event.getPos()))
	{		
		console()<<"facebookButton CALL"<<std::endl;
		socialShare::SocialShare::socialType = SOCIAL_FACEBOOK;
		//allowRect = socialShare::SocialShare::getAllowRec();
		mWebViewPtr->LoadURL( Awesomium::WebURL( Awesomium::WSLit(  socialShare::SocialShare::facebookAuthURL ) ) );
		mWebViewPtr->Focus();
		touchDisable = true;
		socialCall = true;
	}
	else if (emailButton.contains(event.getPos()))
	{
		socialShare::SocialShare::socialType = SOCIAL_EMAIL;
		console()<<"emailButton CALL"<<std::endl;
		touchDisable = true;
		socialCall = true;
	}

	//system("rundll32.exe c:\\Windows\\System32\\shimgvw.dll, ImageView_PrintTo z:\\1.jpg \"MITSUBISHI CP70D Series(USB)\"");
	//int callbaxk = 0;//system("mspaint /p z:\\1.jpg /pt \"MITSUBISHI CP70D Series(USB)\"");	
	//console()<<callbaxk<< " callbaxk "<<std::endl;

	//system("rundll32.exe  c:\\Windows\\System32\\mspaint.exe, /p z:\\1.jpg /pt \"MITSUBISHI CP70D Series(USB)\"");
	
	// send mouse events to Awesomium
	console()<<event.getX()<< "  "<<event.getY()<<std::endl;
	////console()<< event.mInitiator<<" - "<<std::endl;
	
}

void PocoTestApp::mouseDrag( MouseEvent event )
{
	// send mouse events to Awesomium
	ph::awesomium::handleMouseDrag( mWebViewPtr, event );
}

void PocoTestApp::mouseUp( MouseEvent event )
{
	// send mouse events to Awesomium
	ph::awesomium::handleMouseUp( mWebViewPtr, event );
}

void PocoTestApp::mouseWheel( MouseEvent event )
{
	// send mouse events to Awesomium
	ph::awesomium::handleMouseWheel( mWebViewPtr, event );
}

void PocoTestApp::keyDown( KeyEvent event )
{
	// send key events to Awesomium
	
	ph::awesomium::handleKeyDown( mWebViewPtr, event );
}

void PocoTestApp::keyUp( KeyEvent event )
{
	// send key events to Awesomium
	ph::awesomium::handleKeyUp( mWebViewPtr, event );
	//console()<<"  KEY UP  "<<std::endl;
	/*unsigned int k = 1;
	float r = 1.0f;
	uint32_t t = 1;
	MouseEvent m(getWindow(),1, 286 , 206, k,  r,  t);

	ph::awesomium::handleMouseDown( mWebViewPtr, m );

	console()<<m.getX()<< "  "<<m.getY()<<std::endl;*/

}

CINDER_APP_NATIVE( PocoTestApp, RendererGl )
