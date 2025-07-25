#ifndef _LIST_H_
#define _LIST_H_

#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

#define TRUE 1
#define FALSE 0
typedef int BOOL;
typedef struct _object 
{ 
	int  index;
   	SDL_Surface *Img;
   	int X,Y,W,H,DIRX,DIRY,Life;
   	float FX,FY,DX,DY;
   	int Angle;
	BOOL visible;
	struct _object *next;
} OBJECT;


/* Adapted from Kernighan and Pike's "The Practice of Programming"  pp.46 et 
seq. (Addison-Wesley 1999) */

// create new list element of type OBJECT from the supplied text string
OBJECT *newelement(OBJECT temp);
OBJECT *addfront(OBJECT *head, OBJECT *newp);
OBJECT *addend (OBJECT *head, OBJECT *newp);
OBJECT *addmiddle (OBJECT *head, OBJECT *newp);
OBJECT *update(OBJECT *head, int index, OBJECT temp);
OBJECT *getObject(OBJECT *head, int index);
void RemoveThing(OBJECT **head, int index);
void deleteList(OBJECT **head);
OBJECT *delelement(OBJECT *head, int index);
void deleteObject(OBJECT **head,int index, BOOL sort);
int length(OBJECT **head);
void printlist(OBJECT **head);
void reindex(OBJECT **head);
#endif
