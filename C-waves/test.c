#include<time.h>
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<SDL2/SDL_ttf.h>
#include<stdlib.h>
#include<stdio.h>

SDL_Texture* titleInscription(SDL_Renderer* gRenderer,TTF_Font* timerFont,SDL_Color timerColor,int*x,int* k,SDL_Rect yourNameRect,char*writeYourName)
{
  
 // char* writeYourName = "TIME:";
  SDL_Surface* image = TTF_RenderText_Solid(timerFont,writeYourName,timerColor);
  *x = image->w,*k=image->h;
  yourNameRect.w = *x; yourNameRect.h = *k;
  SDL_Texture* groundTexture = SDL_CreateTextureFromSurface(gRenderer,image);
  SDL_RenderCopy(gRenderer,groundTexture,NULL,&yourNameRect);
  SDL_RenderPresent(gRenderer);
  return groundTexture;
}

void pasteNumberToChar(char **bufferString,int minNumber) {
   char* bufferString2 =  (char*)malloc(25);
   int BufferCounter = 0;
  while(minNumber >= 1) {
      *bufferString2 = (minNumber%10) + '0';
      bufferString2++;
      minNumber = minNumber/10;
  }
  bufferString2--;
  while(*bufferString2){
      **bufferString = *bufferString2;
      bufferString2--; (*bufferString)++;
      BufferCounter++;
  }
  **bufferString =' ';
  (*bufferString)++;
  (**bufferString) = '\0';
  (*bufferString) -=BufferCounter +1;
  return; 
}

void changeFPS (int *quit, SDL_Event e,int *delayTime, int *timerZero, int *framesInt){

	while(SDL_PollEvent(&e) != 0)
	{
		if (e.type == SDL_QUIT) {
			*quit =1 ;

		}else if (e.type == SDL_KEYDOWN || e.type == SDL_MOUSEBUTTONDOWN) {
      if ( e.key.keysym.sym ==SDLK_UP ) {
       if ( *delayTime >5 ){

         (*delayTime) += 5; 
         *framesInt = 1; 
         *timerZero = time(NULL);

       }

      }else if ( e.key.keysym.sym ==SDLK_DOWN) {

       if( *delayTime <300 ){
         *framesInt = 1; 
         (*delayTime) -= 5;
         *timerZero = time(NULL);
       }
       }else {
          *quit =1;
       }
    }
 }
}

int main ( void )
{

int mWidth = 800, mHeight = 600;
  SDL_Init(SDL_INIT_VIDEO);
  TTF_Init();
  SDL_Window* window = SDL_CreateWindow("Read_Write", SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,mWidth,mHeight,SDL_WINDOW_SHOWN);
  SDL_Renderer* gRenderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);
  SDL_Color timerColor = {0,255,0};
  
 TTF_Font* timerFont = TTF_OpenFont("v_DigitalStrip_v1.5.ttf",25);

 SDL_Rect timerPosition = {700,50,100,20};

 SDL_Rect titleRect = { 580,50, 200,50};
 SDL_Surface* timerSurface = NULL;
 SDL_Texture* timerTexture = NULL;

 SDL_Rect frameRect = { 580,100, 200,50};
 SDL_Rect framePosition = { 700,100, 200,50};
 SDL_Surface* frameSurface = NULL;
 SDL_Texture* frameTexture = NULL;

  SDL_Event e;
  int quit = 0, checkerTime= 0;
  int timerZero,timerNow;
  timerZero = time(NULL);
  char* timerText = "TIME:";
  char* frameText = "FRAME:";
  char* timerChar =(char*)malloc(12);
  char* frameChar =(char*)malloc(12);
  SDL_Texture* titleTime = titleInscription( gRenderer, timerFont, timerColor, &titleRect.w, &titleRect.h,titleRect,timerText);
  SDL_Texture* titleFrame = titleInscription( gRenderer, timerFont, timerColor, &frameRect.w, &frameRect.h,frameRect,frameText);

  int framesInt = 0, delayTime = 15;
 
while (!quit)
  {
    changeFPS(&quit, e, &delayTime,&timerZero,&framesInt);

    SDL_SetRenderDrawColor(gRenderer,0,0,0,0);
    SDL_RenderClear(gRenderer);

framesInt++;

  timerNow = time(NULL)+1;

    pasteNumberToChar( &timerChar, (timerNow - timerZero)); 
    pasteNumberToChar( &frameChar, (framesInt)/(timerNow - timerZero)); 

    timerSurface = TTF_RenderText_Solid(timerFont,timerChar,timerColor);
    timerTexture = SDL_CreateTextureFromSurface(gRenderer,timerSurface);
    frameSurface = TTF_RenderText_Solid(timerFont,frameChar,timerColor);
    frameTexture = SDL_CreateTextureFromSurface(gRenderer,frameSurface);

    timerPosition.w = timerSurface->w; 
    timerPosition.h = timerSurface->h;
    framePosition.w = frameSurface->w; 
    framePosition.h = frameSurface->h;


    SDL_RenderCopy(gRenderer,timerTexture,NULL,&timerPosition); 
    SDL_RenderCopy(gRenderer,frameTexture,NULL,&framePosition); 
    SDL_RenderCopy(gRenderer,titleTime,NULL,&titleRect); 
    SDL_RenderCopy(gRenderer,titleFrame,NULL,&frameRect); 
    SDL_RenderPresent(gRenderer);
    
    SDL_FreeSurface(timerSurface);
    SDL_FreeSurface(frameSurface);
    SDL_DestroyTexture(timerTexture);
    SDL_DestroyTexture(frameTexture);
   SDL_Delay( delayTime ); 
  } 

   SDL_DestroyWindow( window );
   TTF_Quit();
   SDL_Quit();

}