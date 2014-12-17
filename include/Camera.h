#pragma once

#include "cinder/app/AppNative.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/gl.h"
#include "cinder/Capture.h"
#include "cinder/gl/GlslProg.h"


#include "Params.h"

using namespace std;

class PhotoCamera
{
	public:	
		static void setup();
		static void draw();
		static void drawDesaturate();
		static void update();

		static float getCadrWidth();
		static float getCadrHeight();
		static void takePhoto();
		static Surface8u lastFrame;
	
	private:

		static float CAMERA_HEIGHT;
		static float CAMERA_WIDTH;

		static float CADR_HEIGHT;
		static float CADR_WIDTH;

		static float CADR_X;
		static float CADR_Y;

		static float CONTRAST;
		static float BRIGHTNESS;
		static float CONTRAST_DES;
		static float BRIGHTNESS_DES;

		static ci::CaptureRef			mCapture;
	    static ci::gl::TextureRef		cameraTexture;
		static ci::gl::GlslProg		mShader, contrShader, mShaderColor;
		static ci::gl::Texture testPhoto;
		static ci::Surface8u drawsurf;
		
		
};