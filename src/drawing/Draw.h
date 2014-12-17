#pragma once

#include "cinder/app/AppNative.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/gl.h"
#include "cinder/CinderMath.h"

using namespace std;
using namespace ci;
using namespace ci::app;

class Draw
{
	public:	
		
		static void circleSlice(float x,float y, float radius, float lowAngle, float highAngle, bool closed, bool radians);	
};