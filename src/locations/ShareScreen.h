#pragma once

#include "Location.h"
#include "cinder/ip/Resize.h"
#include "cinder/app/AppNative.h"
#include "cinder/Timeline.h"
#include "cinder/gl/Texture.h"
#include "cinder/ImageIo.h"
#include "cinder/Text.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Fbo.h"

#include <boost/algorithm/string.hpp>

#include "Params.h"
#include "PhotoMaker.h"
#include "Printer.h"
#include "QRcode.h"
#include "PreloaderCircle.h"

#include "cinder/Json.h"
#include "cinder/Base64.h"
#include "Curl.h"
#include "Utils.h"

#include "EmailForm.h"
#include "SocialForm.h"


using namespace ci;
using namespace ci::app;
using namespace std;
using namespace gl;
using namespace mndl::curl;

class ShareScreen : public Location
{
public:
	void Setup();
	void Init( LocationEngine* game);
	void Cleanup();

	void Pause();
	void Resume();

	void HandleEvents(LocationEngine* game);
	void Update(LocationEngine* game);
	void Draw(LocationEngine* game);

	void MouseEvents( LocationEngine* game );

	static ShareScreen* Instance() {
		return &ShareScreenState;
	}

	static bool CONNECTION_PROBLEM;
	static bool PhotoUploadReady;

	Surface dateSurf;

	static int SOCIAL_POPUP;

	static bool   PHOTO_POLOSA_READY_TO_SHOW;


	static bool EMAIL_ENABLE			;
	static bool VKONTAKTE_ENABLE		;
	static bool FACEBOOK_ENABLE			;

	void				PREPARE_IMAGES_TO_SERVER();

	void StartanimationFinish();


protected:
	ShareScreen() { }
	
private:
	
	LocationEngine*			_game;
	static ShareScreen		ShareScreenState;
	int						state;

	int					current_day, current_month;
	Font				dayFont, dayFont2;

	
	Texture			*fon3, *cracks, *logo, *photoTemplate, *socialBottomBlock, *printAgainEnable, *printAgainDisable, *emailBottom, *focus_err;
	Texture			*endText, *shadowPhoto, *telpl, *emailpl, *socialpl, *vkbtn, *facebookBtn, *emailBtn, textTexture;
	Texture			photoTemplateTemp, monthJulyTex, monthAugTex, monthSeptTex;
	Texture			*startAgain, *printAgain, *text1, *text2, *textShow;

	Anim<ColorA>	fonAlpha, shadowAlpha, textAlpha, colorBtns;
	Anim<Vec2f>		cracksCoords, photoReadyCoords, logoCoord, socialBottomBlockCoord;
	Anim<Vec2f>     emailCoords, socialCoords, telCoords, textCoords;

	Font				counterFont, qrCodeFont;	



	Surface				resizeScreenshot(ci::Surface screenshot, int32_t width, int32_t height);
	Surface				photoCollage, photoReadySurf, sendToMailPhoto;
	Surface				photoTemplateSurface, _monthSurf, _daySurf, _topSurface;

	

	static bool				saveImage;

	void					saveAndPrintImage();
	void					animationFinish();
	//void					StartanimationFinish();
	void					resetAnimationParams();
	void					socialDraw();
	void					savePrintServer() ;
	void					saveImageToFile(Surface screenshot, string name);
	bool					savePrintServerThreadHandler() ;	
	void					SendToServerImage(std::string _name );
	void					preRenderOneFrame();
	
	void					printHandler();
	bool					isNowPrinting;

	void					SendToServerPrintInfo();

	void					threadInHandler();
	void					threadOutHandler();
	void					backGroundHandlers();
	void					prepareToSaveAndPrintImage();	


	QRcode					qrCodeBlock;
	
	std::shared_ptr<std::thread> savePrintServerThread;
	
	struct monthStruct
	{
		ci::gl::Texture tex;
		ci::Vec2f coord1;
		ci::Vec2f coord2;
	} monthInfo;
	
	Timer				printerTimer, returnTimer;		
	
	Rectf fbButtonCoord, vkButtonCoord, emailButtonCoord, startAgainButtonCoord, repeatPrintButtonCoord;

	bool THREAD_FREE;	

	int photoPrintMode;
	int photoPrintOffsetX;

	void				SEND_IMAGE_TO_SERVER();
	void				PREPARE_IMAGES_TO_PRINT();
	void				PREPARE_IMAGES_TO_PRINT_FAKE();
	void				SEND_IMAGE_TO_PRINTER();
	void				LOAD_FAKE_PHOTOS_BECAUSE_BREAK_FBO();

	void				drawToFBO(Surface img);
	Fbo					mFbo;
	ci::gl::GlslProg	mShaderNoColor, mShaderColor;


	bool				focusWatermark;

	EmailForm			emailForm;
	SocialForm			socialForm;
	void				formClosed();


	
};