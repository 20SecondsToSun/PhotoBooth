#pragma once

#include "cinder/app/AppNative.h"
#include "cinder/Timeline.h"
#include "cinder/gl/Texture.h"
#include "cinder/Text.h"
#include "cinder/Timer.h"

#include "Location.h"
#include "Params.h"
#include "PhotoMaker.h"
#include "Draw.h"
#include "AssetsManager.h"
#include "CameraAdapter.h"


using namespace ci;
using namespace ci::app;
using namespace gl;
using namespace std;

class Instruction : public Location
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

	static Instruction* Instance() {
		return &InstructionState;
	}

protected:
	Instruction() { }

private:
	void					 MouseEvents( LocationEngine* game );

	LocationEngine*			_game;
	static Instruction		InstructionState;
	int						state, currentSec;

	Texture					*fon1, *centerText; 
	Texture					*btnShadow, *centerTextBlock, *buttonNoColor, *buttonColor;
	Texture					*counterCircle, *krugShadow,  *buttonReplay, *centerTextBlockTextOver;
	Texture					*counterCircleColor, *counterCircleNoColor;

	Anim<ColorA>			alpha, alphaOverlay, alhaBtnShadow, alphaKrugShadow, alphaShadowLogo, alphaFirstScreen, buttonReplayAlpha;
	Anim<Vec2f>				noColorPhotoCoord, colorPhotoCoord, button1Coord, button2Coord, centerTextBlockCoord;
	Anim<Vec2f>				counterCircleScale;

	Font					counterFont;

	Timer					returnToBegin;	

	void					animationFinish();
	void					hideState2();
	void					showState1();

	bool					isCameraNeedToUpdate, circleAnimationStart, circleAnimationFinish;
	double					percentToStartPhoto;

	void					drawMask() ;

	float					angleShift;
};