#include <stdio.h>
#include <SDL2/SDL.h>
#include "Physics2.h"

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_FPS = 60;
const int SCREEN_TICK_PER_FRAME = 1000 / SCREEN_FPS;

int tempx = SCREEN_WIDTH / 2;
int tempy = SCREEN_HEIGHT / 2;

class LTimer {
    public:
		LTimer();

		void start();
		void stop();
		void pause();
		void unpause();

		Uint32 getTicks();

		bool isStarted();
		bool isPaused();

    private:
		Uint32 mStartTicks;

		Uint32 mPausedTicks;

		bool mPaused;
		bool mStarted;
};

LTimer::LTimer() {
    mStartTicks = 0;
    mPausedTicks = 0;

    mPaused = false;
    mStarted = false;
}

void LTimer::start() {
    mStarted = true;
    mPaused = false;

    mStartTicks = SDL_GetTicks();
	mPausedTicks = 0;
}

void LTimer::stop() {
    mStarted = false;
    mPaused = false;

	mStartTicks = 0;
	mPausedTicks = 0;
}

void LTimer::pause() {
    if( mStarted && !mPaused ) {
        mPaused = true;

        mPausedTicks = SDL_GetTicks() - mStartTicks;
		mStartTicks = 0;
    }
}

void LTimer::unpause() {
    if( mStarted && mPaused ) {
        mPaused = false;
        mStartTicks = SDL_GetTicks() - mPausedTicks;
        mPausedTicks = 0;
    }
}

Uint32 LTimer::getTicks() {
	Uint32 time = 0;

    if( mStarted ) {
        if( mPaused ) {
            time = mPausedTicks;
        } else {
            time = SDL_GetTicks() - mStartTicks;
        }
    }
    return time;
}

bool LTimer::isStarted() {
    return mStarted;
}

bool LTimer::isPaused() {
    return mPaused && mStarted;
}

RopeSimulation* ropeSimulation = new RopeSimulation ( 25, 0.01f, 2000.0f, 0.05f, 0.2f, Vector2D(0, 9.81f), 0.05f );

void SimUpdate( float ms, int mx, int my ){

    //printf( "mouse: %d, %d mouset: %d, %d\n", mx, my, tempx, tempy );

    Vector2D ropeConnectionVel;

    if ( tempx != mx || tempy != my  ){
        ropeConnectionVel.x += ( mx - tempx ) / 4.0;
        ropeConnectionVel.y += ( my - tempy ) / 4.0;
    }

    tempx = mx;
    tempy = my;

    ropeSimulation->setRopeConnectionVel( ropeConnectionVel );

	float dt = ms / 1000.0f;

	float maxPossible_dt = 0.002f;

  	int numOfIterations = (int)( dt / maxPossible_dt ) + 1;
	if ( numOfIterations != 0 )
		dt = dt / numOfIterations;

	for ( int a = 0; a < numOfIterations; ++a )
		ropeSimulation->operate(dt);

}

int main(int argc, char ** argv) {

    LTimer fpsTimer;
    LTimer capTimer;

    int countedFrames       = 0;
    fpsTimer.start();

    bool quit               = false;
    int mousePosX           = 0;
    int mousePosY           = 0;
    int stowConst           = 250;

    SDL_Event e;


    SDL_Init ( SDL_INIT_VIDEO );

    SDL_Window      * window    = SDL_CreateWindow      ( "Spring Sim", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, 0 );
    SDL_Renderer    * renderer  = SDL_CreateRenderer    ( window, -1, 0 );


    SDL_SetRenderDrawColor( renderer, 255, 255, 255, 255 );

    while ( !quit ) {
        capTimer.start();

        while( SDL_PollEvent( &e ) != 0 ) {
            if( e.type == SDL_QUIT ) {
                quit = true;
            }
            if( e.type == SDL_KEYDOWN ) {
                if ( e.key.keysym.sym == SDLK_ESCAPE )
                    quit = true;
            }
        }

        SimUpdate( float( SCREEN_TICK_PER_FRAME ), mousePosX, mousePosY );

        /*
        switch ( event.type ) {
            case SDL_QUIT:
                quit = true;
                break;

            case SDL_KEYDOWN:
                switch ( event.key.keysym.sym ) {
                    case SDLK_ESCAPE:
                        quit = true;
                        break;
                }
                break;
        }*/

        float avgFPS = countedFrames / ( fpsTimer.getTicks() / 1000.f );
        if( avgFPS > 2000000 ) {
            avgFPS = 0;
        }

        SDL_SetRenderDrawColor( renderer, 0, 0, 0, 255 );
        SDL_RenderClear( renderer );
        SDL_GetMouseState( &mousePosX, &mousePosY );
        SDL_SetRenderDrawColor( renderer, 255, 255, 255, 255 );
        //SDL_RenderDrawLine( renderer, mousePosX, mousePosY, 320, 240 );

        for ( int a = 0; a < ropeSimulation->numOfMasses - 1; ++a) {
            Mass* mass1 = ropeSimulation->getMass(a);
            Vector2D* pos1 = &mass1->pos;

            Mass* mass2 = ropeSimulation->getMass(a + 1);
            Vector2D* pos2 = &mass2->pos;

            SDL_RenderDrawLine( renderer, SCREEN_WIDTH / 2 + pos1->x * stowConst,
                                          -1 * SCREEN_HEIGHT / 2 + ( pos1->y + 1.92 ) * stowConst,
                                          SCREEN_WIDTH / 2 + pos2->x * stowConst,
                                          -1 * SCREEN_HEIGHT / 2 + ( pos2->y + 1.92 ) * stowConst );
            //if (a == 0) printf( "pos: %f, %f;", pos1->x, -1*pos1->y );
        }
        SDL_RenderPresent( renderer );
        ++countedFrames;

        int frameTicks = capTimer.getTicks();
        if( frameTicks < SCREEN_TICK_PER_FRAME ) {
            SDL_Delay( SCREEN_TICK_PER_FRAME - frameTicks );
        }
    }

    SDL_DestroyRenderer ( renderer );
    SDL_DestroyWindow ( window );
    SDL_Quit ();

    return 0;
}
