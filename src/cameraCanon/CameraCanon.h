#include "cinder/app/AppNative.h"
#include "cinder/Utilities.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Timeline.h"

#include "CinderCanon.h"
#include "CanonCamera.h"

using namespace std;

class CameraCanon :public canon::PhotoHandler 
{

public:
	
	bool setup(bool & _isConnect);
	void update();
	void draw();
    void shutdown();
	void restartLiveView();

	 //void keyDown( KeyEvent event );
	 // Delegate callbacks from canon::PhotoHandler
    void photoTaken(EdsDirectoryItemRef directoryItem, EdsError error);
    void photoDownloaded(const std::string & downloadPath, EdsError error);
	void photoCameraError( EdsError err);
	void handleStateEvent(EdsUInt32 inEvent);

	std::string photoDownloadDirectory();
	std::string photoCameraReadyLiveView();

	CanonCamera                 mCamera;
    fs::path                    mPhotoDownloadFolder;
    Anim<float>                 mFlashAlpha;
	void takePhoto(bool & userPhotoIsDownloaded, string & path, string& photoCameraErrorMsg);

	bool  *userPhotoIsDownloaded;
	bool *isConnect;
	string *userPhotoFileName;
	string *photoCameraErrorMsg; 

	Timer restartLiveViewTimer;
};