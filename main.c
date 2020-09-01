/* *************************** Ast3r0id ***************************   */
/* Author: V3lorek				       		      */
/* Date : 2020				               		      */
/* [+] STRUCTURE: 		|-------------------v		      */
/* [MAIN] -> [NEWGAME] -> [MAIN_LOOP] ->[UPDATEGAME && HANDLE_EVENTS] */  
/*				^	            |	              */
/*                     [DRAWTOSCREEN] <-      [MOVEOBJECTS]           */
/* *****************************************************************  */
/* [Authors of Assets]		      				      */
/* - Ship: [Seki]from cleanpng.com    				      */
/* - Asteroid:  pngwave.com	      				      */
/* - Theme: [Jan125] opengameart.org  				      */
/* *****************************************************************  */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

#define TRUE 1
#define FALSE 0
#define PI 3.14159265
//GAME CONSTANTS
#define MAX_KEY 1000
#define UP 1
#define DOWN 0
#define SPEED 8
#define ROTATION 5.5
//MESSAGES
#define STS_MSG0 "SDL Graphics loaded successfully.\n"
#define ERR_MSG0 "SDL Graphics could not be loaded. \n"
#define ERR_MSG1 "Failed to load asset. \n"

typedef struct _tobject { 
   //int Index;
   int X,Y,W,H;
   float DX,DY;
   int Angle;
   SDL_Surface *Img;
   //struct _tobject *next; 
} TOBJECT;

typedef int BOOL;

/* GLOBALS */
TOBJECT ship;
TOBJECT asteroids[7];
SDL_Surface *background,*asteroid;
SDL_Event ev;
SDL_Renderer *ren1;
SDL_Window *win1;
long KeyState[MAX_KEY];
BOOL Running=TRUE;
BOOL keypressed=FALSE;
time_t t;
Mix_Music *Theme;
//int Angle=0;
//Mix_Chunk *Snd1, *Snd2, *Snd3;
 

/* ---------------------------------------------- */
/* FUNCTION PROTOTYPES */
/* ---------------------------------------------- */
/* Game engine */
BOOL  InitVideo();
BOOL  InitAudio();
void  LoadAssets();
void  NewGame();
void  UpdateGame();
void  Main_Loop();
void  CleanMemory();
void  movePlayerXY(int speed, int direction);
void  rotateBy(TOBJECT *Object, float D);
void  Ship_Behaviour();
void  moveAsteroids();
/* Mathematics */
double sinD(int degree);
double cosD(int degree);
int getSign(int number);
/* Drawing */
void Draw(int X, int Y, SDL_Surface *Img);
void DrawObject(TOBJECT Object);
void DrawScreen();
void LoadAsteroids();
/* EVENTS */
BOOL Key(long K);
void HandleKey(long Sym, BOOL Down);
void HandleEvents();

/* ---------------------------------------------- */
/* MAIN CODE */ 
/* ---------------------------------------------- */
int main(int argc, char ** argv){
  InitVideo();
  InitAudio();
  LoadAssets();
  NewGame();
  Main_Loop(); //UPDATE EVENTS AND DRAW TO SCREEN
  CleanMemory();
  return 0;
}

/* FUNCTIONS */

BOOL InitVideo(){
   SDL_Init(SDL_INIT_VIDEO);
   IMG_Init(IMG_INIT_PNG);
   win1 = SDL_CreateWindow(" > Ast3r0id <", 50,50, 640,480,SDL_WINDOW_SHOWN);
   ren1 = SDL_CreateRenderer(win1, -1, 0);
   return (ren1 != NULL) && (win1 != NULL);
}

BOOL InitAudio(){
  if (SDL_Init(SDL_INIT_AUDIO) < 0){
    return FALSE;
  }
  Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 
  MIX_DEFAULT_CHANNELS, 4096);
  return TRUE;
} 
void LoadAssets(){
  int i;
  /* Images */
  background = SDL_LoadBMP("data/pics/spexpb.bmp");
  if (background == NULL) {fprintf(stderr, ERR_MSG1); exit(0);} 
  ship.Img = IMG_Load("data/pics/ship.png");
  if (ship.Img == NULL) {fprintf(stderr, ERR_MSG1); exit(0);}   
  asteroid = IMG_Load("data/pics/asteroid.png");
  if (asteroid == NULL)  {fprintf(stderr, ERR_MSG1); exit(0);}  
  for (i=0; i<7; i++){
    asteroids[i].Img = asteroid;
  }  
  /* Music and Sounds */
  Theme = Mix_LoadMUS("data/snd/theme.ogg");
  if (Theme == NULL)  {fprintf(stderr, ERR_MSG1); exit(0);}  
}

void NewGame(){
  int i;
  /* SHIP */ 
  ship.X = 100;
  ship.Y = 100;
  ship.DX = ship.X;
  ship.DY = ship.Y;
  ship.W = 50;
  ship.H = 70;
  ship.Angle = 0;  
 
  /* Asteroids */
  srand((unsigned) time(&t));
  for (i=0; i<7; i++){
    asteroids[i].X = rand() % 640;
    asteroids[i].Y = rand() % 480;
    asteroids[i].W = 60;
    asteroids[i].H = 60;
    asteroids[i].Angle = 0;
  }
  Mix_PlayMusic(Theme, -1);   
}
//TOBJECT *newobject(int x, int y, int w, int h, int Angle, SDL_Surface *IMG){
//}   

/*Drawing Functions */
void Draw(int X, int Y, SDL_Surface *Img) {
  SDL_Rect R;
  SDL_Texture *text;
  
  R.x = X;
  R.y = Y;
  R.w = Img->w;
  R.h = Img->h;
  text = SDL_CreateTextureFromSurface(ren1,Img);
  SDL_RenderCopy(ren1, text, NULL, &R);
  SDL_DestroyTexture(text);
}


void DrawObject(TOBJECT Object){
  SDL_Rect R;
  SDL_Texture *text;
  
  R.x = Object.X;
  R.y = Object.Y;
  R.w = Object.W;
  R.h = Object.H;
  text = SDL_CreateTextureFromSurface(ren1,Object.Img);
  SDL_RenderCopyEx(ren1, text, NULL, &R, Object.Angle,NULL, SDL_FLIP_NONE);
  SDL_DestroyTexture(text);
}

void DrawScreen() {
   int i;
   SDL_RenderClear(ren1);
   Draw(0,0,background);
   DrawObject(ship);
   for (i=0;i<7;i++){
     DrawObject(asteroids[i]);
   }
   SDL_RenderPresent(ren1);
}

/* Move functions */
double sinD(int degree){
    double ret,val;
    val = PI / 180;
    ret = sin(degree*val);
    return ret;
}

double cosD(int degree){
    double ret,val;
    val = PI / 180;
    ret = cos(degree*val);
    return ret;
}
int getSign(int number){
  if (number < 0) 
    return -1;
  else
    return 1;
}
void movePlayerXY(int speed, int direction){
   ship.DX = ship.DX + (speed*sinD(ship.Angle))*-1; 
   ship.DY = ship.DY + (speed*cosD(ship.Angle)); 
   ship.X = round(ship.DX);
   ship.Y = round(ship.DY);
   printf("\nAngle: %d | Ship.X: %d | Ship.Y: %d | SinD : %f | CosD : %f\n", ship.Angle,ship.X, ship.Y, sinD(ship.Angle), cosD(ship.Angle));        
}

void rotateBy(TOBJECT *Object, float D){
   float temp;
   if(abs(Object->Angle + D) < 181) {
     temp = Object->Angle + D;
     Object->Angle = round(temp);
   } else{
      Object->Angle = Object->Angle * -1;  
    }
}


void Ship_Behaviour(){
  if (ship.Y < -10) {ship.Y = 480; ship.DY = 480;}
  if (ship.Y > 490) {ship.Y = 0; ship.DY = 0;}
  if (ship.X > 640) {ship.X = 0; ship.DX = 0;}
  if (ship.X < -10) {ship.X = 640; ship.DX = 640;}
}
void moveAsteroids(){
}


void UpdateGame(){
  if (Key(SDLK_q)) printf("Q\n");
  if (Key(SDL_SCANCODE_UP)) movePlayerXY(-SPEED,UP);
  if (Key(SDL_SCANCODE_DOWN)) movePlayerXY(SPEED,DOWN);
  if (Key(SDL_SCANCODE_RIGHT)) rotateBy(&ship, ROTATION);
  if (Key(SDL_SCANCODE_LEFT)) rotateBy(&ship, -ROTATION);
  Ship_Behaviour();
  moveAsteroids();
}

BOOL Key(long K){
  if ((K>= 0) && (K <= MAX_KEY)) 
    return KeyState[K]; 
  else 
    return FALSE;
}

void HandleKey(long Sym, BOOL Down){
  if (Sym == SDLK_UP) Sym = SDL_SCANCODE_UP;
  if (Sym == SDLK_DOWN) Sym = SDL_SCANCODE_DOWN;
  if (Sym == SDLK_LEFT) Sym = SDL_SCANCODE_LEFT;
  if (Sym == SDLK_RIGHT) Sym = SDL_SCANCODE_RIGHT;
  if ((Sym >= 0) && (Sym <= MAX_KEY)) {
    KeyState[Sym] = Down;
    if (Sym == SDLK_ESCAPE) Running = FALSE;    
  }
} 

void HandleEvents(){
  SDL_Event e;
  if (SDL_PollEvent(&e)) {
    if (e.type == SDL_QUIT) {
      Running = FALSE;
    }
    
    if (e.type == SDL_KEYDOWN){
      keypressed = TRUE;
      HandleKey(e.key.keysym.sym, TRUE);
    }
    if (e.type == SDL_KEYUP){
      keypressed = FALSE;
      HandleKey(e.key.keysym.sym, FALSE);
    }  
  }
}

void Main_Loop(){
/* Update + HandleEvents - Draw */
  unsigned int LastTime, CurrentTime;

  LastTime = SDL_GetTicks();
  while (Running==TRUE) {
    	CurrentTime = SDL_GetTicks();
 	if (CurrentTime - LastTime > 1000) LastTime = CurrentTime - 60;
 	while (CurrentTime - LastTime > 1000/30) {
 	  UpdateGame();
          LastTime = LastTime + 30;
 	}  
       HandleEvents();         
       DrawScreen(ren1);
       //Delete_Objects();
    }
} 

void CleanMemory(){
  SDL_DestroyRenderer(ren1);
  SDL_DestroyWindow(win1);
  Mix_Quit();
  IMG_Quit(); 
  SDL_Quit();
} 
/* ---------------------------------------------- */
/* END 						  */
/* ---------------------------------------------- */
