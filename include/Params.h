#pragma once

#include "cinder/app/AppNative.h"
#include "cinder/Utilities.h"

#define SERVER "92.63.102.99:82"
#define FILE_NAME_TO_PRINT "template.bmp"
#define FILE_NAME_TO_MAIL "mail.jpg"
#define PHOTO_SAVE_NAME(ind) "PHOTO_"+ind+".jpg"
#define FAKE_PHOTO_SAVE_NAME(ind) "FAKE_PHOTO_"+ind+".jpg"
#define FOCUS_OUT_PHOTO_SAVE_NAME(ind) "FOCUS_OUT_"+ind+".jpg"

#define SHOT_NUMS 3
#define SHOT_SECONDS 5


const float WIDTH		 = 1920.0f; 
const float HEIGHT		 = 1080.0f;
const int   BIG_PHOTO_WIDTH  = 1000;
const int   BIG_PHOTO_HEIGHT   = 638;

const bool  RELEASE_VER   = true;

using namespace std;
using namespace ci;
using namespace ci::app;

class Params
{ 
	public:
	   static bool				FULL_SCREEN;
	   static float				scalingFactor;
	   static bool				isDesaturate;
	   static vector<Surface>   photosStorage;
	   static vector<string>    photosNameStorage;

	   static string			sessionId;
	   static int				printerJobSeconds;

	  static int				lastScreenReturnSeconds;
	  static int				returnFromCameraChooseSeconds;

	  static string				ShareVkontakteText;
	  static string				link;	  

	  static LPWSTR				PrinterImageURL;

	  static fs::path			downloadedDirectoryForCanonPhotos;
	  static fs::path			getPhotosStorageDirectory();


	  static bool				isFboCrash;
	  static bool				isPhotoFake;
	  static bool				focusError;
	  
};