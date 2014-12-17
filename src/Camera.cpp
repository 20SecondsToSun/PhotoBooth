#include "Camera.h"
using namespace ci;
using namespace ci::app;
using namespace std;

float PhotoCamera::CAMERA_HEIGHT = 1080;////480.0f;
float PhotoCamera::CAMERA_WIDTH = 1280;//640.0f;

float PhotoCamera::CADR_HEIGHT = 548.0f;
float PhotoCamera::CADR_WIDTH  = 858.0f;

float PhotoCamera::CADR_X = 210.0f;
float PhotoCamera::CADR_Y  = 170.0f;

ci::CaptureRef			PhotoCamera::mCapture;
ci::gl::TextureRef		PhotoCamera::cameraTexture;

ci::gl::GlslProg		PhotoCamera::mShader;
ci::gl::GlslProg		PhotoCamera::mShaderColor;

ci::gl::GlslProg		PhotoCamera::contrShader;
ci::Surface8u			PhotoCamera::drawsurf;
ci::gl::Texture			PhotoCamera::testPhoto;

Surface8u PhotoCamera::lastFrame;


void PhotoCamera::setup()
{	
	
}

void PhotoCamera::takePhoto()
{

}

void PhotoCamera::draw()
{	
	if(drawsurf)
	 {
		mShaderColor.bind();
		mShaderColor.uniform( "basetex", 2 );
		gl::draw( drawsurf );
		mShaderColor.unbind();		
	 }
}

void PhotoCamera::drawDesaturate()
{	
	if( drawsurf ) 
	{
		mShader.bind();
		mShader.uniform( "basetex", 2 );
		gl::draw( drawsurf );
		mShader.unbind();	
	}
}

void PhotoCamera::update()
{
	if( mCapture && mCapture->checkNewFrame() ) 
	{
		Surface8u surf = mCapture->getSurface();		
		drawsurf.copyFrom(surf, Area(CADR_X, CADR_Y, CADR_X + CADR_WIDTH, CADR_HEIGHT + CADR_Y), -Vec2i(CADR_X,CADR_Y));			
	}	
}

float PhotoCamera::getCadrWidth()
{
	return float(CADR_WIDTH);
}

float PhotoCamera::getCadrHeight()
{
	return float(CADR_HEIGHT);
}