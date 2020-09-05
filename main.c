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
#include "list.h" 

#define PI 3.14159265
//GAME CONSTANTS
#define SCREEN_W 640
#define SCREEN_H 480
#define ASTW0 80
#define ASTH0 80
#define ASTW1 60
#define ASTH1 60
#define ASTW2 32
#define ASTH2 32
#define MAX_LIFE 100
#define MAX_KEY 1000
#define UP 1
#define DOWN 0
#define SPEED 8
#define ROTATION 5.5
//MESSAGES
#define STS_MSG0 "SDL Graphics loaded successfully.\n"
#define ERR_MSG0 "SDL Graphics could not be loaded. \n"
#define ERR_MSG1 "Failed to load asset. \n"


typedef struct _sprite{
   SDL_Surface *Img;
} SPRITE;

enum SHIPSTATE{HALTED, UTHRUST, DTHRUST, LTHRUST, RTHRUST, DAMAGED};
/* GLOBALS */
OBJECT ship;
OBJECT *asteroids;
OBJECT *projectiles;
SPRITE shipSprite[9];
SDL_Surface *background,*asteroid,*projectile;
SDL_Event ev;
SDL_Renderer *ren1;
SDL_Window *win1;
long KeyState[MAX_KEY];
BOOL Running=TRUE;
BOOL keypressed=FALSE;
BOOL mouseclicked=FALSE;
time_t t;
Mix_Music *Theme;
Mix_Chunk *sound,*shot;
enum SHIPSTATE ShipState;
int level=0; int lives=MAX_LIFE;
double PlayerShootTime;
//int Angle=0;
//Mix_Chunk *Snd1, *Snd2, *Snd3;
 

/* ---------------------------------------------- */
/* FUNCTION PROTOTYPES */
/* ---------------------------------------------- */
/* Mathematics & Physics */
double sinD(int degree);
double cosD(int degree);
int getSign(int number);
BOOL Collision(int AX1, int AY1, int AX2, int AY2, int BX1, int BY1, int BX2, int BY2); 
/*SDL Related */
BOOL  InitVideo();
BOOL  InitAudio();
void  ToggleFullscreen(SDL_Window* Window);
void  CleanMemory();
/* EVENTS */
BOOL Key(long K);
void HandleKey(long Sym, BOOL Down);
void HandleEvents();
/* Game engine */

void  LaunchProjectile(double X, double Y, double DX, double DY, SDL_Surface *Img);
void  LoadAssets();
void  NewGame();
void  UpdateGame();
void  Main_Loop();
void  movePlayerXY(int speed, int direction);
void  rotateBy(OBJECT *Object, float D);
void  Ship_Behaviour();
void  moveAsteroids();
void  moveProjectiles();
void  ShootPlayerBullet();
//void  deleteObject(OBJECT **head, int index);
/* Drawing */
void Draw(int X, int Y, SDL_Surface *Img);
void DrawObject(OBJECT Object);
void DrawDynamicObject(OBJECT *Object);
void DrawScreen();
void LoadAsteroids();
//BOOL Animation(int *ticks, double *time, int frames); 
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
//Mathematics and Physics
BOOL Collision(int AX1, int AY1, int AX2, int AY2, int BX1, int BY1, int BX2, int BY2){
  return (AX1 < BX1 + (BX2-BX1)) && (AX1 + (AX2-AX1) > BX1) && (AY1 < BY1 + (BY2-BY1)) && (AY1 + (AY2-AY1) > BY1);
}

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

//SDL Initialization
BOOL InitVideo(){
   SDL_Init(SDL_INIT_VIDEO);
   IMG_Init(IMG_INIT_PNG);
   win1 = SDL_CreateWindow(" > Ast3r0id <", 50,0,SCREEN_W,SCREEN_H,SDL_WINDOW_SHOWN);
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
void ToggleFullscreen(SDL_Window* Window) {
    Uint32 FullscreenFlag = SDL_WINDOW_FULLSCREEN;
    BOOL IsFullscreen = SDL_GetWindowFlags(Window) & FullscreenFlag;
    SDL_SetWindowFullscreen(Window, IsFullscreen ? 0 : FullscreenFlag);
    SDL_ShowCursor(IsFullscreen);
}

void CleanMemory(){
  if (asteroids != NULL) deleteList(&asteroids);
  SDL_DestroyRenderer(ren1);
  SDL_DestroyWindow(win1);
  Mix_Quit();
  IMG_Quit(); 
  SDL_Quit();
} 

//Events Functions

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
  if (Sym == SDLK_SPACE) Sym = SDL_SCANCODE_SPACE;
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
    if (e.type == SDL_MOUSEBUTTONDOWN){
      mouseclicked = TRUE;
    }
    if (e.type == SDL_MOUSEBUTTONUP){
      mouseclicked = FALSE;
    }
    if (e.type == SDL_KEYUP){
      keypressed = FALSE;
      HandleKey(e.key.keysym.sym, FALSE);
    }  
  }
}

// Game Engine

void LoadAssets(){
  /* Images */
  background = SDL_LoadBMP("data/pics/spexpb.bmp");
  if (background == NULL) {fprintf(stderr, ERR_MSG1); exit(0);} 
//  ship.Img = IMG_Load("data/pics/ship.png");
//  if (ship.Img == NULL) {fprintf(stderr, ERR_MSG1); exit(0);}   
  asteroid = IMG_Load("data/pics/asteroid.png");
  if (asteroid == NULL)  {fprintf(stderr, ERR_MSG1); exit(0);}  
  projectile = IMG_Load("data/pics/fire1.png");
  if (projectile == NULL)  {fprintf(stderr, ERR_MSG1); exit(0);}  

  shipSprite[0].Img = IMG_Load("data/pics/ship.png");
  if (shipSprite[0].Img == NULL) {fprintf(stderr, ERR_MSG1);  printf("0");exit(0);}   
   shipSprite[1].Img = IMG_Load("data/pics/ship_plume.png");
  if (shipSprite[1].Img == NULL) {fprintf(stderr, ERR_MSG1);  printf("1");exit(0);}   
  shipSprite[2].Img = IMG_Load("data/pics/ship_plume2.png");
  if (shipSprite[2].Img == NULL) {fprintf(stderr, ERR_MSG1);  printf("2");exit(0);}   
   shipSprite[3].Img = IMG_Load("data/pics/ship_plume3.png");
  if (shipSprite[3].Img == NULL) {fprintf(stderr, ERR_MSG1);  printf("3");exit(0);}   
   shipSprite[4].Img = IMG_Load("data/pics/ship_plume4.png");
  if (shipSprite[4].Img == NULL) {fprintf(stderr, ERR_MSG1);  printf("4");exit(0);}   
  shipSprite[5].Img = IMG_Load("data/pics/ship_plume5.png");
  if (shipSprite[5].Img == NULL) {fprintf(stderr, ERR_MSG1);  printf("4");exit(0);}   
   shipSprite[6].Img = IMG_Load("data/pics/ship_plume6.png");
  if (shipSprite[6].Img == NULL) {fprintf(stderr, ERR_MSG1);  printf("5");exit(0);}
   shipSprite[7].Img = IMG_Load("data/pics/ship_dmg0.png");
  if (shipSprite[7].Img == NULL) {fprintf(stderr, ERR_MSG1);  printf("5");exit(0);}
   shipSprite[8].Img = IMG_Load("data/pics/ship_dmg1.png");
  if (shipSprite[8].Img == NULL) {fprintf(stderr, ERR_MSG1);  printf("5");exit(0);}
  
  ship.Img = shipSprite[0].Img;   
  /* Music and Sounds */
  Theme = Mix_LoadMUS("data/snd/theme.ogg");
   if (Theme == NULL)  {fprintf(stderr, ERR_MSG1); exit(0);} 
   sound = Mix_LoadWAV("data/snd/rockets.wav");
   if (sound == NULL) {fprintf(stderr, ERR_MSG1); exit(0);} 
   Mix_VolumeChunk( sound, MIX_MAX_VOLUME );

   shot = Mix_LoadWAV("data/snd/shot.wav");
   if (shot == NULL) {fprintf(stderr, ERR_MSG1); exit(0);}    
   Mix_VolumeChunk(shot, MIX_MAX_VOLUME ); 
}

void LaunchProjectile(double X, double Y, double DX, double DY, SDL_Surface *Img){
OBJECT p;
  if (projectiles == NULL) 
    p.index = 0;
  else{
    p.index = length(&projectiles);
  } 
  p.Img = Img;
  p.Angle = ship.Angle;
  p.W = 16;
  p.H = 16;
  p.FX = (ship.X+(ship.W-10)/2) + ((cosD(p.Angle) *-1) - (sinD(p.Angle) * 40)*-1);
  p.FY = (ship.Y+(ship.H-30)/2) + (sinD(p.Angle) * -1) + ((cosD(p.Angle) * 40*-1));
  p.DX = DX;
  p.DY = DY;
  p.X = round(p.FX);
  p.Y = round(p.FY);
  printf("FX: %f FY: %f  | DX:%f DY%f | X: %d Y:%d \n", p.FX,p.FY,p.DX,p.DY,p.X, p.Y);
  projectiles=addend(projectiles, newelement(p)); 
} 

void NewGame(){
  int i,a;
  OBJECT temp;

  if (asteroids != NULL) deleteList(&asteroids);
  
  lives = MAX_LIFE;
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
    temp.index = i;
    a = rand() % 2;
    if (a==0) temp.DIRX = 1;
    else
      temp.DIRX = -1;
    a = rand() % 2;
    if (a==0) temp.DIRY = 1;
    else
      temp.DIRY = -1; 
    printf("%d:%d\n", temp.DIRX, temp.DIRY);
    temp.X = rand() % 640;
    temp.Y = rand() % 480;
    temp.DX = temp.X;
    temp.DY = temp.Y;
    a = rand() % 3;
    if (a==0){
     temp.W = ASTH0;
     temp.H = ASTW0; 
     temp.Life = 3;
    } 
    if(a==1) {
     temp.W = ASTH2;
     temp.H = ASTW2;
     temp.Life = 2; 
    }
    if(a==2){
     temp.W = ASTH1;
     temp.H = ASTW1; 
     temp.Life = 3; 
    }
    temp.Angle=0;
    temp.Img = asteroid;
    asteroids = addend(asteroids, newelement(temp)); 
   }
   /* Music */
    Mix_PlayMusic(Theme, -1); 
}

// Drawing Functions 
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

void DrawDynamicObject(OBJECT *Object){
  SDL_Rect R;
  SDL_Texture *text;
  
  R.x = Object->X;
  R.y = Object->Y;
  R.w = Object->W;
  R.h = Object->H;
  text = SDL_CreateTextureFromSurface(ren1,Object->Img);
  SDL_RenderCopyEx(ren1, text, NULL, &R, Object->Angle,NULL, SDL_FLIP_NONE);
  SDL_DestroyTexture(text);
}


void DrawObject(OBJECT Object){
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
   int i, a;
   SDL_RenderClear(ren1);
   Draw(0,0,background);
   switch (ShipState){
 	case HALTED: ship.Img = shipSprite[0].Img; break;			        
 	case UTHRUST: a=rand() & 1; ship.Img = shipSprite[a+1].Img; break;			        
 	case DTHRUST: a=rand() & 1; ship.Img = shipSprite[a+3].Img; break;		       
 	case LTHRUST: ship.Img = shipSprite[6].Img; break;			        
 	case RTHRUST: ship.Img = shipSprite[5].Img; break;			        
 	case DAMAGED: a=rand() & 1; ship.Img = shipSprite[a+7].Img; break;			        
    }
   DrawObject(ship);
   
   if (asteroids != NULL) {
     for (i=0;i<length(&asteroids);i++){
       DrawDynamicObject(getObject(asteroids,i));
     }
   }

   if (projectiles != NULL && length(&projectiles) > 0) {
     for (i=0;i<length(&projectiles);i++){
       DrawDynamicObject(getObject(projectiles,i));
     }
   }


   SDL_RenderPresent(ren1);
}

//Move functions

void movePlayerXY(int speed, int direction){
   ship.DX = ship.DX + (speed*sinD(ship.Angle))*-1; 
   ship.DY = ship.DY + (speed*cosD(ship.Angle)); 
   ship.X = round(ship.DX);
   ship.Y = round(ship.DY);
   printf("\nAngle: %d | Ship.X: %d | Ship.Y: %d | SinD : %f | CosD : %f\n", ship.Angle,ship.X, ship.Y, sinD(ship.Angle), cosD(ship.Angle));        
}

void moveProjectiles(){
  int i,j;
  OBJECT *p,*a;
  BOOL bcollision=FALSE;
 for (i=0; i<length(&projectiles);i++){
    p = getObject(projectiles,i);
    //Vector rotation
    p->FX = p->FX + (p->DX*sinD(p->Angle)); 
    p->FY = p->FY + (p->DY*cosD(p->Angle))*-1; 
    p->X = round(p->FX);
    p->Y = round(p->FY);
   
    if (p->Y < -10 || p->Y > SCREEN_H + 10 || p-> X < -10 || p-> X > SCREEN_W + 10) {
	deleteObject(&projectiles,0,TRUE);
	if (projectiles != NULL) printf("%d\n",length(&projectiles));
	break;
    }
   //Collision with Asteroids.
  for (j=0; j<length(&asteroids); j++){
     a=getObject(asteroids,j);
    //Collision with Projectile
    if (Collision(p->X,p->Y,p->X+p->W,p->Y+p->H,a->X,a->Y,
    a->X+a->W,a->Y+a->H)) {
	printf("\n Asteroid #%d destroyed: \n",a->index);
	deleteObject(&asteroids,j,TRUE);
	deleteObject(&projectiles,i,TRUE);
	bcollision = TRUE;
	break;
     }
   }
  if (bcollision == TRUE) break;
 }
}
void rotateBy(OBJECT *Object, float D){
   float temp;
   if(abs(Object->Angle + D) < 181) {
     temp = Object->Angle + D;
     Object->Angle = round(temp);
   } else{
      Object->Angle = Object->Angle * -1;  
    }
}


void Ship_Behaviour(){
  if (ship.Y < -10) {ship.Y = SCREEN_H; ship.DY = SCREEN_H;}
  if (ship.Y > SCREEN_H+10) {ship.Y = 0; ship.DY = 0;}
  if (ship.X > SCREEN_W + 10) {ship.X = 0; ship.DX = 0;}
  if (ship.X < -10) {ship.X = SCREEN_W; ship.DX = SCREEN_W;}
}

void moveAsteroids(){

 int i,j;
 OBJECT *p;
  
   for (i=0; i<length(&asteroids); i++){
     p=getObject(asteroids,i);
    //Collision with Ship
    if (Collision(ship.X+5,ship.Y+5,ship.X+ship.W-5,ship.Y+ship.H-5,p->X,p->Y,
    p->X+p->W,p->Y+p->H)) {
    	p->DIRX = p->DIRX * -1;
    	p->DIRY = p->DIRY * -1;
	ShipState = DAMAGED;
 	lives = lives -1;
	printf("\n Collision with Asteroid #%d: \n", p->index);
        if(lives == 0) {SDL_Delay(1000);NewGame();}
    }
    //Collision with Asteroids
    for (j=0; j<length(&asteroids); j++){
      if (j!=i){
        if (Collision(p->X,p->Y+5,p->X+p->W-5,p->Y+p->H-5,
	p->X,p->Y,p->X+p->W,p->Y+p->H)) {
            p->DIRX = p->DIRX * -1;
    	    p->DIRY = p->DIRY * -1;
            p->DIRX = p->DIRX * -1;
    	    p->DIRY = p->DIRY * -1;
       }
      }
    } 
    p->DX = p->DX + (1.5 * p->DIRX);
    p->DY = p->DY + (1.5 * p->DIRY);
    p->X = round(p->DX);
    p->Y = round(p->DY);
    p->Angle += 3.5;
    if (p->Y < -10) {p->Y = SCREEN_H; p->DY = SCREEN_H;}
    if (p->Y > SCREEN_H + 10) {p->Y = 0; p->DY = 0;}
    if (p->X > SCREEN_W + 10) {p->X = 0; p->DX = 0;}
    if (p->X < -10) {p->X = SCREEN_W; p->DX = SCREEN_W;}
  } 
}

void ShootPlayerBullet(){
 if (SDL_GetTicks() - PlayerShootTime < 200) {
   //Do nothing
 } else{
   PlayerShootTime = SDL_GetTicks();
   if (Mix_Playing(2) == 0) Mix_PlayChannel(2, shot, 0);
   LaunchProjectile(ship.X+16, ship.Y-2, 10,10, projectile);
   //printf("FX: %f FY: %f  | DX:%f DY%f | X: %d Y:%d \n", p->FX,p->FY,p->DX,p->DY,p->X, p->Y);   
}
}

void UpdateGame(){
  //if (Key(SDLK_q)) printf("Q\n");
  if (Key(SDLK_f)) ToggleFullscreen(win1);
  if (Key(SDL_SCANCODE_SPACE)) {ShootPlayerBullet();}
  if (Key(SDL_SCANCODE_UP) || Key(SDLK_w)) {if (Mix_Playing(1) == 0) Mix_PlayChannel( 1, sound, 0);ShipState = UTHRUST; movePlayerXY(-SPEED,UP);}
  if (Key(SDL_SCANCODE_DOWN) || Key(SDLK_s)) {if (Mix_Playing(1) == 0) Mix_PlayChannel( 1, sound, 0);ShipState = DTHRUST; movePlayerXY(SPEED,DOWN);}
  if (Key(SDL_SCANCODE_RIGHT) || Key(SDLK_d)) {if (Mix_Playing(1) == 0) Mix_PlayChannel( 1, sound, 0);ShipState = RTHRUST; rotateBy(&ship, ROTATION);}
  if (Key(SDL_SCANCODE_LEFT) || Key(SDLK_a)) {if (Mix_Playing(1) == 0) Mix_PlayChannel( 1, sound, 0);ShipState = LTHRUST; rotateBy(&ship, -ROTATION);}
  if (!keypressed) {
     ShipState = HALTED;
     if (Mix_Playing(1) != 0) Mix_FadeOutChannel(1,500);	
  }
  if (mouseclicked == TRUE) {ShootPlayerBullet();}
  Ship_Behaviour();
  moveAsteroids();
  moveProjectiles();
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
/* ---------------------------------------------- */
/* END 						  */
/* ---------------------------------------------- */
