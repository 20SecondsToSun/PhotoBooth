#include "Params.h"

float		 Params::scalingFactor				= 42.0;
bool		 Params::isDesaturate					= false;
string		 Params::sessionId = "";


bool		 Params::isFboCrash					= false;
bool		 Params::isPhotoFake				= false;
bool		 Params::focusError					= false;



int			 Params::printerJobSeconds             = 15;
int			 Params::lastScreenReturnSeconds       = 60;
int			 Params::returnFromCameraChooseSeconds = 60;

std::vector<ci::Surface>  Params::photosStorage;
std::vector<std::string>  Params::photosNameStorage;

std::string Params::link = "none";

bool Params::FULL_SCREEN;

std::string Params::ShareVkontakteText = "";

LPWSTR Params::PrinterImageURL =  L"Z:\\GENERATIVE\\Cinder\\cinder_0.8.5_vc2012\\cinder_0.8.5_vc2012\\apps\\PocoTest\\vc11\\Debug\\template.bmp";

fs::path  Params::downloadedDirectoryForCanonPhotos = getDocumentsDirectory()/"Documents"/"Canon Pictures";

fs::path		Params::getPhotosStorageDirectory()
{
	return getAppPath()/"Documents"/"Canon Pictures";
}