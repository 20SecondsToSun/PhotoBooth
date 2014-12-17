#pragma once

#include "cinder/app/AppNative.h"
#include "cinder/Timeline.h"
#include "cinder/gl/Texture.h"
#include "cinder/Text.h"
#include "cinder/Timer.h"

#include "Location.h"
#include "Params.h"
#include "ShareScreen.h"
#include "AssetsManager.h"
#include "CameraAdapter.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace gl;

//const std::string EXTRENSION = ".jpg";

class PhotoMaker : public Location
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

	static PhotoMaker* Instance() {
		return &PhotoMakerState;
	}

protected:
	PhotoMaker() { }

private:

	LocationEngine*			_game;
	static PhotoMaker		PhotoMakerState;

	int						state, allSeconds;

	Texture					*mainPhoto;

	Anim<float>				_scaleRectAnim, _mainPhotoAlphaAnim, _alphaFlashAnim;
	Anim <Vec2f>			_startShiftNum2, _startShiftNum1;

	Font					numFont, countFont;	
	Texture					textTexture;
	Timer					photoTimer;
	
	void					startPhotoSaver();	
	void					endFirstAnim();	
	
	bool					fakePhoto, needToSaveFocusOut;

	void					flashFinished();
	void					saveFocusErrorPhoto();	
	void					saveFakeScreenPhoto();
};