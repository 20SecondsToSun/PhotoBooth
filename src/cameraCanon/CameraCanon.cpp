#include "CameraCanon.h"
#include "Params.h"

bool CameraCanon::setup(bool & _isConnect)
{
	isConnect = &_isConnect;

    mFlashAlpha = 0.0f;
    mPhotoDownloadFolder = Params::getPhotosStorageDirectory();
    if (!fs::exists(mPhotoDownloadFolder)) fs::create_directories(mPhotoDownloadFolder);
    
    bool status = mCamera.setup(this);
	
	*isConnect = status;

	if (status == false) return false;

    mCamera.startLiveView();

	return true;
}

void CameraCanon::update()
{
   mCamera.update();

   if(mCamera.checkNewFrame())
    {
		// process image here with mCamera.getLiveSurface();
    }

	if (!restartLiveViewTimer.isStopped())
	{
		if((int)restartLiveViewTimer.getSeconds() == 1)
		{
			photoCameraReadyLiveView();
		}
	}
}

void CameraCanon::draw()
{
	 if(mCamera.isLiveViewing())
	{		
		gl::draw( gl::Texture( mCamera.getLiveSurface() ));	
	}
}

void CameraCanon::takePhoto(bool & _userPhotoIsDownloaded, string& path, string& error)
{
	userPhotoIsDownloaded = &_userPhotoIsDownloaded;
	userPhotoFileName = &path;
	photoCameraErrorMsg = &error;
    mCamera.endLiveView();
    mCamera.takePicture(this);	
}

void CameraCanon::restartLiveView()
{
	 mCamera.startLiveView();
}

//void CameraCanon::keyDown( KeyEvent event )
//{
   /* switch (event.getChar())
    {
        case 'l': mCamera.toggleLiveView(); break;
        case 'f': setFullScreen( !isFullScreen() ); break;
        case ' ': takePhoto(); break;
        case event.KEY_ESCAPE: quit(); break;
    }*/
//}

void CameraCanon::shutdown()
{
	 mCamera.shutdown();
}

//#pragma mark
//#pragma mark Callbacks from the canon SDK
//-----------------------------------------------------------------------------------
void CameraCanon::photoCameraError( EdsError err)
{
	//string temp = (c_str, strnlen(c_str, max_length));
	*photoCameraErrorMsg = CanonErrorToString(err);
	console()<<" ==   photoCameraError  = "<<CanonErrorToString(err)<<std::endl;
}

void CameraCanon::photoTaken(EdsDirectoryItemRef directoryItem, EdsError error)
{
	console()<<" ==   photoTaken  = "<<std::endl;
    mCamera.downloadImage(directoryItem, this);
}

void CameraCanon::photoDownloaded(const std::string & downloadPath, EdsError error)
{
	//console()<<" ==   downloadedImage  = "<<mCamera.getCannon().getFileName()<<std::endl;	
	*userPhotoFileName = mCamera.getCannon().getFileName();
   *userPhotoIsDownloaded = true;
	
}

// Delegate method to tell the canon where to download the full-res image.
std::string CameraCanon::photoDownloadDirectory()
{
    return mPhotoDownloadFolder.generic_string();
}

// Delegate method when camera ready
std::string CameraCanon::photoCameraReadyLiveView()
{
	if (*isConnect == false)	 return "ERROR";

	if (mCamera.isLiveViewing()) return "START";

	//if (mCamera.isBusy())		 return "ERROR";

	console()<<"START LIVE VIEW!!!!!!"<<std::endl;

	console()<<"isBusy? "<<mCamera.isBusy()<<std::endl;

	if (mCamera.isBusy())
	{
		//restartLiveViewTimer.start();
		//return "ERROR";
	}

	EdsError err = mCamera.startLiveView();

	if (err != EDS_ERR_OK)
	{
		restartLiveViewTimer.start();
	}
	else
	{
		restartLiveViewTimer.stop();
	}
	console()<<"isBusy? "<<mCamera.isBusy()<<std::endl;
	
	console()<<"START LIVE VIEW??  "<< CanonErrorToString(err)<<std::endl;

	return "START";
}
void CameraCanon::handleStateEvent(EdsUInt32 inEvent)
{
	console()<<"STSWITCH OFF??  "<< std::endl;

	if (kEdsStateEvent_Shutdown == inEvent)
	{
		 mCamera.endLiveView();
		*isConnect = false;
	}
	
}


