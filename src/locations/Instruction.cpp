#include "Instruction.h"
#include <GL/glu.h>

Instruction Instruction::InstructionState;


void Instruction::Setup()
{
	fon1						= AssetManager::getInstance()->getTexture(  "locations/01/01_plash.png" ) ;
	centerText					= AssetManager::getInstance()->getTexture( "locations/01/01_text.png" ) ;
	centerTextBlock				= AssetManager::getInstance()->getTexture( "locations/02/02_green.png");
	centerTextBlockTextOver		= AssetManager::getInstance()->getTexture( "locations/02/02_select.png");

	buttonReplay				= AssetManager::getInstance()->getTexture( "locations/02/02_replay.png");
	buttonNoColor				= AssetManager::getInstance()->getTexture( "locations/02/02_buttonblack.png");
	buttonColor					= AssetManager::getInstance()->getTexture( "locations/02/02_buttoncolor.png");

	counterCircleColor			= AssetManager::getInstance()->getTexture( "locations/03/03_loadcolor.png");	
	counterCircleNoColor		= AssetManager::getInstance()->getTexture( "locations/03/03_circle.png");		
	krugShadow					= AssetManager::getInstance()->getTexture( "locations/03/03_circle_shd.jpg");

	counterFont					 = Font( loadFile(getAssetPath("fonts/ToyotaDisplay_Bd.ttf")), 376 );	
}

void Instruction::Init( LocationEngine* game)
{
	console()<< "INIT Instruction:: "<<std::endl;	

	_game = game;
	counterCircle = counterCircleNoColor;	

	state = 1;
	currentSec = 5;
	percentToStartPhoto = -1;

	circleAnimationStart = false;
	circleAnimationFinish = false;
	isCameraNeedToUpdate = false;

	angleShift = 0;
	

	colorPhotoCoord		= Vec2f(-961 , 230);
	noColorPhotoCoord	= Vec2f(WIDTH, 230);

	alphaFirstScreen	= ColorA(1.0f, 1.0f, 1.0f, 0.0f);
	alphaOverlay		= ColorA(1.0f, 1.0f, 1.0f, 0.0f);
	buttonReplayAlpha	= ColorA(1.0f, 1.0f, 1.0f, 0.0f);
	alphaKrugShadow		= ColorA(0.0f, 0.0f, 0.0f, 0.0f);

	button1Coord				= Vec2f(123,  HEIGHT);
	button2Coord				= Vec2f(WIDTH - 123 - buttonNoColor->getWidth() , HEIGHT);
	centerTextBlockCoord		= Vec2f(472 , HEIGHT);
	counterCircleScale			= Vec2f(0 , 0);

	CameraAdapter::Instance()->live();

	showState1();	

}
void Instruction::Cleanup()
{
	console()<< "CLEAN :: Instruction"<<std::endl;
	returnToBegin.stop();	
}
void Instruction::Pause()
{
	console()<< "PAUSE :: Instruction"<<std::endl;
}
void Instruction::Resume()
{
	console()<< "RESUME :: Instruction"<<std::endl;
}
void Instruction::animationFinish()
{
	state++;

	_game->animationRunning(false);

	if (state == 1)
	{
		isCameraNeedToUpdate = false;
	}

	if (state == 2)
	{
		returnToBegin.start();
	}
	else
	{
		returnToBegin.stop();
	}

	if (state == 3)
	{
		isCameraNeedToUpdate = false;		
		
	}

	if (state == 5)
	{
		_game->ChangeState(PhotoMaker::Instance());
	}
}
void Instruction::MouseEvents( LocationEngine* game )
{
	MouseEvent event = game->getMouseEvent();	

	if(game->isAnimationRunning()) return;


	if(event.isLeftDown())
	{
		Rectf rec = Rectf(82.0f, 70.0f, 82.0f +  buttonReplay->getWidth(), 70.0f +  buttonReplay->getHeight());
		if(rec.contains(event.getPos()))
		{
			if (state == 2)
			{
				game->animationRunning(true);
				hideState2();
				showState1();				
				return;
			}
		}

		if (state == 1)
		{
			game->animationRunning(true);
			//PhotoCamera::live();
			isCameraNeedToUpdate = true;

			
			alpha = ColorA(1.0f, 1.0f, 1.0f, 0.0f);
			timeline().apply( &alphaFirstScreen,ColorA(1.0f, 1.0f, 1.0f, 0.0f), 0.5f, EaseOutQuart() );

			buttonReplayAlpha = ColorA(1.0f, 1.0f, 1.0f, 0.0f);			
			timeline().apply( &buttonReplayAlpha,ColorA(1.0f, 1.0f, 1.0f, 1.0f), 0.5f, EaseOutQuart() );
			timeline().apply( &alpha,ColorA(1.0f, 1.0f, 1.0f, 1.0f), 0.5f, EaseOutQuart() );
			timeline().apply( &colorPhotoCoord, Vec2f(0.0f, 205.0f), 0.5f, EaseOutQuart() ).delay(0.2f);
			timeline().apply( &noColorPhotoCoord, Vec2f(961.0f, 205.0f), 0.5f, EaseOutQuart() ).delay(0.2f);
			timeline().apply( &alphaOverlay,ColorA(1.0f, 1.0f, 1.0f, 1.0f), 0.5f, EaseOutQuart() );		
			timeline().apply( &button1Coord, Vec2f(75.0f, HEIGHT - 353.0f), 0.5f, EaseOutQuart()).delay(0.2f);
			timeline().apply( &button2Coord, Vec2f(WIDTH - 115.0f - buttonNoColor->getWidth(), HEIGHT - 353.0f), 0.5f, EaseOutQuart() ).delay(0.2f);
			timeline().apply( &centerTextBlockCoord, Vec2f(472.0f, HEIGHT - centerTextBlock->getHeight()+ 50.0f), 0.5f, EaseOutQuart() ).delay(0.3f).finishFn( bind( &Instruction::animationFinish, this ) );
		
		}
		else if (state == 2)
		{
			if (event.getY() < 200) 	return;

			returnToBegin.stop();
			game->animationRunning(true);
			Params::isDesaturate = event.getX() > WIDTH * 0.5;			
			circleAnimationStart = true;

			hideState2();
			timeline().apply( &counterCircleScale, Vec2f(1.0f, 1.0f), 0.8f, EaseOutQuart() ).delay(0.4f);
			timeline().apply( &alphaKrugShadow, ColorA(1.0f, 1.0f, 1.0f, 1.0f), 0.8f, EaseOutQuart()).delay(0.4f).finishFn( bind( &Instruction::animationFinish, this ) );			
			
		}
	}

	event.setHandled(true );
}
void Instruction::hideState2()
{
	timeline().apply( &buttonReplayAlpha,ColorA(1.0f, 1.0f, 1.0f, 0.0f), 0.5f, EaseOutQuart() );
	timeline().apply( &button1Coord, Vec2f(123.0f, HEIGHT), 0.8f, EaseOutQuart() ).delay(0.2f);
	timeline().apply( &button2Coord, Vec2f(WIDTH - 123.0f - buttonNoColor->getWidth(), HEIGHT), 0.8f, EaseOutQuart() ).delay(0.2f);
	timeline().apply( &centerTextBlockCoord, Vec2f(515.0f, HEIGHT), 0.8f, EaseOutQuart());			
	timeline().apply( &colorPhotoCoord, Vec2f(-961.0f, 205.0f), 0.8f, EaseOutQuart() ).delay(0.2f);
	timeline().apply( &noColorPhotoCoord, Vec2f(1920.0f, 205.0f), 0.8f, EaseOutQuart() ).delay(0.2f);
}
void Instruction::showState1()
{
	state = 0;
	timeline().apply( &alphaFirstScreen,ColorA(1.0f, 1.0f, 1.0f, 1.0f), 0.5f, EaseOutQuart() ).finishFn( bind( &Instruction::animationFinish, this ) );
}
void Instruction::HandleEvents(LocationEngine* game)
{

}
void Instruction::Update(LocationEngine* game) 
{
	if (state == 3)
	{
		
		if(circleAnimationFinish)
		{
			state = 4 ;
			game->animationRunning(true);

			timeline().apply( &counterCircleScale, Vec2f(6, 6), 0.7f, EaseOutQuart()).finishFn( bind( &Instruction::animationFinish, this ) );		
			timeline().apply( &alphaKrugShadow, ColorA(0.0f, 0.0f, 0.0f, 0.0f), 0.5f, EaseOutQuart());	
		}
	}

	if (returnToBegin.isStopped() == false)
	{
		int sec = int(returnToBegin.getSeconds());
		if (sec >= Params::returnFromCameraChooseSeconds )
		{
			game->animationRunning(true);
			hideState2();
			showState1();
			returnToBegin.stop();
		}
	}	

	if (isCameraNeedToUpdate)
	{
		//PhotoCamera::updateCanon();
		CameraAdapter::Instance()->update();
	}
}

void Instruction::Draw(LocationEngine* game) 
{	
	ColorA _alpha  = alphaFirstScreen;


	if (_alpha.a > 0)
	{
		gl::pushMatrices();			
					gl::translate(265, 100);
					gl::color( alphaFirstScreen );				
					gl::draw( *fon1);
					gl::translate(210, 230);
					gl::draw( *centerText);
		gl::popMatrices();	
	}

	gl::color(Color::white());

	//
	if (isCameraNeedToUpdate)
	{
		gl::pushMatrices();	
		gl::translate(Vec2f(82,70));				
		gl::color( buttonReplayAlpha);
		gl::draw( *buttonReplay);
		gl::popMatrices();
		gl::color(Color::white());
			
		gl::pushMatrices();				
		gl::translate(centerTextBlockCoord);		
		gl::draw(*centerTextBlock);
		gl::translate(Vec2f(310,247));
		gl::draw(*centerTextBlockTextOver);			
		gl::popMatrices();

		gl::pushMatrices();			
		gl::translate(colorPhotoCoord);		
		float __scale = 961.0f/CameraAdapter::Instance()->getWidth();
		gl::scale(__scale, __scale);

		CameraAdapter::Instance()->draw();
		gl::popMatrices();

		gl::pushMatrices();			
		gl::translate(noColorPhotoCoord);	
		gl::scale(__scale, __scale);
		
		CameraAdapter::Instance()->drawDesaturate();
		
		gl::popMatrices();

		gl::pushMatrices();				
		gl::translate(button1Coord);		
		gl::draw( *buttonColor);
		gl::popMatrices();

		gl::pushMatrices();	
		gl::translate(button2Coord);		
		gl::draw( *buttonNoColor);
		gl::popMatrices();	
	}			
	
	if(circleAnimationStart == true || state == 3)
	{
		Vec2f v = counterCircleScale;
		gl::color(Color::white());		
		gl::pushMatrices();	
		gl::translate(0.5f*(getWindowWidth()-counterCircle->getWidth())-50+11, 0.5f*(getWindowHeight()-counterCircle->getHeight())+100.0f+45);
	
		gl::popMatrices();		
		
		gl::color(Color::white());			
		gl::pushMatrices();	
		gl::translate(0.5f*(getWindowWidth()-counterCircle->getWidth()*v.x), 0.5f*(getWindowHeight()-counterCircle->getHeight()*v.x));
		gl::scale(counterCircleScale);		
		gl::draw( *counterCircle);

		drawMask();

		if (circleAnimationFinish)
		{
			gl::color(Color::white());	
			gl::drawSolidCircle(Vec2f(counterCircle->getWidth()*0.5f, counterCircle->getHeight()*0.5f), 285.0f);
		}

		gl::popMatrices();
	}		
}

void Instruction::drawMask() 
{	
		gl::draw(*counterCircleColor);

		glEnable(GL_BLEND); 
		
		glColorMask(false, false, false, true);  
		glBlendFunc(GL_SRC_ALPHA, GL_ZERO);  
		glColor4f(0.5,0.5,1,0.0f); 
		
		Draw::circleSlice(870*0.5, 870*0.5, 870*0.5, 90, 90 + angleShift ,true,false);
		
		if (angleShift>= 360)
		{			
			circleAnimationFinish  = true;
		}
		else
		{
			angleShift+=2;
		}

		glColorMask(true,true,true,true);  
		glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_DST_ALPHA);  
		glColor4f(1,1, 1 , 1.0f);  
		gl::draw(*counterCircleNoColor);

		glColor4f(0.5,0.5,1,0.0f); 
		glBlendFunc(GL_SRC_ALPHA, GL_ZERO);  
		glDisable(GL_BLEND);
}