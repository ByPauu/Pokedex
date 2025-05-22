#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "esat/window.h"
#include "esat/draw.h"
#include "esat/input.h"
#include "esat/sprite.h"
#include "esat/time.h"


struct pokemon{
	char name[50];
	int number;
	esat::SpriteHandle sprite = NULL;
  char filesprite[50];
	int pcatched;
	pokemon* Next;
	pokemon* Prev;
};

pokemon* Aux = NULL;
pokemon* Last = NULL;
pokemon* First = NULL;

esat::SpriteHandle background;
int catched;
char auxkey;
int manager = 0;
int auxnumber;
int auxkeynum = 0;
char* auxnumberchar = (char*) malloc(sizeof(char) * 50); //Adding Slots into array
char* auxname = (char*) malloc(sizeof(char) * 50); //Adding Slots into array
char* pokenumber = (char*) malloc(sizeof(char) * 50); //Adding Slots into array
double Mouse;

unsigned char fps=25; //Control de frames por segundo
double current_time,last_time;

void ReadPokedex();
void InsertPoke(pokemon* Poke);
void ShowPokedex();
void AddPokemonName();
void AddPokemonNumber();
void ShowHud();
void PokedexSort();

int esat::main(int argc, char **argv) {
	esat::WindowInit(512,384);
	WindowSetMouseVisibility(true);
  esat::DrawSetFillColor(0,0,0);
  esat::DrawSetTextSize(35);
	esat::DrawSetTextFont("fonts/pokemon.ttf");
	background = SpriteFromFile("hud/menu.png");
	esat::ResetBufferdKeyInput();

	ReadPokedex();
    while(esat::WindowIsOpened() && !esat::IsSpecialKeyDown(esat::kSpecialKey_Escape)) {
      //Control fps 
    	do{
    		current_time = esat::Time();
    	}while((current_time-last_time)<=1000.0/fps);

    	last_time = esat::Time(); 
			
			switch(manager){
				case 0:{ ShowHud(); break; }
				case 1:{ AddPokemonName();break; }
				case 2:{ AddPokemonNumber(); break; }
				case 3:{ ShowPokedex(); break; }
				case 4:{ esat::WindowDestroy(); break; }
			}
    	esat::ResetBufferdKeyInput();
    	esat::WindowFrame();
  }
  
	
  esat::WindowDestroy();

  
  return 0;
    
}
void ReadPokedex() {
	FILE *pfile = fopen("pokedex.txt", "r"); // Abre el archivo
	if (pfile != NULL) {
		for (int i = 0; i < 721; i++) { // Cargar hasta 800 Pokémon en memoria
			fscanf(pfile, "%d %49[^\n]", &catched, auxname);

			auxnumber = i + 1;
			if (catched == 1) {
				PokedexSort();
			}
		}
		fclose(pfile);
	}
}
void ShowHud(){
	esat::DrawBegin();
	esat::DrawClear(0,0,0);
	esat::DrawSprite(background,0,0);
	esat::DrawText(86,117,"1. Add Pokemon:");
	esat::DrawText(86,173,"2. Show Pokedex:");
	esat::DrawText(86,227,"3. Exit:");
	esat::DrawText(86,280,"Insert number:");
	esat::DrawEnd();

  switch(esat::GetNextPressedKey()){
    case '1':{
      manager = 1;
      auxnumber = 0;
      memset(auxname, '\0', 50);
      memset(auxnumberchar, '\0', 50);
			background = esat::SpriteFromFile("hud/addpokemon.png");
      break;
    }
    case '2':{
			background = esat::SpriteFromFile("hud/showpokedex.png");
      manager = 3;
      Aux = First;
			Mouse = esat::MouseWheelY();
      break;
    }
    case '3':{
      manager = 4;
      break;
    }
  }

	esat::ResetBufferdKeyInput();
}

void AddPokemonName(){
	esat::DrawBegin();
	esat::DrawClear(0,0,0);
	esat::DrawSprite(background,0,0);
	esat::DrawText(60,150, "Introduce el nombre del pokemon:");
	esat::DrawText(60,226, auxname );

	esat::DrawEnd();
	auxkey = esat::GetNextPressedKey();
	if(esat::IsSpecialKeyDown(esat::kSpecialKey_Enter)){
		manager = 2;
		auxkeynum = 0;
	}else if(esat::IsSpecialKeyDown(esat::kSpecialKey_Backspace)){
		if(auxkeynum > 0){
			auxkeynum--;
			auxname[auxkeynum] = '\0';
		}
	}else if(auxkey != '\0'){
		if(auxkeynum > 0 && auxkey != 32){
			auxkey += 32;
		}
		auxname[auxkeynum] = auxkey;
		auxkeynum++;
	}

	esat::ResetBufferdKeyInput();
	
}
void AddPokemonNumber(){
	esat::DrawBegin();
	esat::DrawClear(0,0,0);
	esat::DrawSprite(background,0,0);
	esat::DrawText(60,150, "Introduce el numero del pokemon:");
	esat::DrawText(60,226, auxnumberchar);

	esat::DrawEnd();
	auxkey = esat::GetNextPressedKey();
	if(auxkey >= 48 && auxkey <= 57){
		auxnumberchar[auxkeynum] = auxkey;
		auxkeynum++;
	}
	if(esat::IsSpecialKeyDown(esat::kSpecialKey_Enter)){
		manager = 0;
		background = esat::SpriteFromFile("hud/menu.png");
		auxkeynum = 0;
		auxnumber = atoi(auxnumberchar);
		PokedexSort();
	}
	if(esat::IsSpecialKeyDown(esat::kSpecialKey_Backspace)){
		if(auxkeynum > 0){
			auxkeynum--;
			auxnumberchar[auxkeynum] = '\0';
		}
	}
	esat::ResetBufferdKeyInput();

}
void InsertPoke(pokemon* Poke){
	Poke->number = auxnumber;
	strcpy(Poke->name, &(*auxname));

	sprintf(Poke->filesprite, "sprites/rescaled/%04d.png", Poke->number);
	Poke->sprite = esat::SpriteFromFile(Poke->filesprite);
}

void PokedexSort(){
	//FIRST LIST
  if(First == NULL){
    Aux = (pokemon*) malloc(sizeof(pokemon)); //Creating the fist list

    InsertPoke(Aux);
		First = Aux;
    Last = First;
		Aux->Next = NULL;
		Aux->Prev = NULL;
  }else{
		//LIST BEFORE FIRST
		if(auxnumber < First->number){ 
			Aux = (pokemon*) malloc(sizeof(pokemon)); //Creating the fist list
      Aux->Next = First;
			First->Prev = Aux;
      First = Aux;
      
      InsertPoke(Aux);
		//LIST AFTER LAST
    }else if(auxnumber > Last->number){
			Aux = (pokemon*) malloc(sizeof(pokemon)); //Creating the fist list
      Last->Next = Aux;
			Aux->Prev = Last;
      Last = Aux;
      
      InsertPoke(Aux);
			Aux->Next = NULL;
		//LIST BETWEEN
		}else if(auxnumber > First->number && auxnumber < Last->number){
			Aux = First;
      while(auxnumber > Aux->Next->number){
        Aux = Aux->Next;
      }
			pokemon* Current = (pokemon*) malloc(sizeof(pokemon)); //Creating the fist list
			if(Current != NULL){
				Current->Next = Aux->Next;
				Current->Prev = Aux;
				Aux->Next = Current;
				
				InsertPoke(Current);
			}
    }
  }
}

void ShowPokedex(){
	if(Aux != NULL || !esat::IsSpecialKeyPressed(esat::kSpecialKey_Space)){
		esat::DrawBegin();
		esat::DrawClear(0,0,0);
		esat::DrawSprite(background,0,0);

		
    esat::DrawSprite(Aux->sprite, 15, 100);
		
		sprintf(pokenumber, "%03d", Aux->number);

		

    esat::DrawSetFillColor(168,184,184);
		esat::DrawText(272,180, pokenumber);
		esat::DrawText(322,180, Aux->name );

		esat::DrawSetFillColor(88,88,80);
		esat::DrawText(270,178, pokenumber);
		esat::DrawText(320,178, Aux->name );

    
		esat::DrawSetFillColor(168,184,184);
		esat::DrawText(32,82, Aux->name );
		esat::DrawSetFillColor(88,88,80);
		esat::DrawText(30,80, Aux->name );

		//RIGHT POKEDEX SIDE NUMBERS
		sprintf(pokenumber, "%03d", Aux->number - 3);
			esat::DrawSetFillColor(168,184,184);
			esat::DrawText(272,80, pokenumber);
			esat::DrawSetFillColor(88,88,80);
			esat::DrawText(270,78, pokenumber);
		sprintf(pokenumber, "%03d", Aux->number - 2);
			esat::DrawSetFillColor(168,184,184);
			esat::DrawText(272,113, pokenumber);
			esat::DrawSetFillColor(88,88,80);
			esat::DrawText(270,111, pokenumber);
		sprintf(pokenumber, "%03d", Aux->number - 1);
			esat::DrawSetFillColor(168,184,184);
			esat::DrawText(272,145, pokenumber);
			esat::DrawSetFillColor(88,88,80);
			esat::DrawText(270,142, pokenumber);
		sprintf(pokenumber, "%03d", Aux->number + 1);
			esat::DrawSetFillColor(168,184,184);
			esat::DrawText(272,212, pokenumber);
			esat::DrawSetFillColor(88,88,80);
			esat::DrawText(270,210, pokenumber);
		sprintf(pokenumber, "%03d", Aux->number + 2);
			esat::DrawSetFillColor(168,184,184);
			esat::DrawText(272,244, pokenumber);
			esat::DrawSetFillColor(88,88,80);
			esat::DrawText(270,242, pokenumber);
		sprintf(pokenumber, "%03d", Aux->number + 3);
			esat::DrawSetFillColor(168,184,184);
			esat::DrawText(272,278, pokenumber);
			esat::DrawSetFillColor(88,88,80);
			esat::DrawText(270,276, pokenumber);
		sprintf(pokenumber, "%03d", Aux->number + 4);
			esat::DrawSetFillColor(168,184,184);
			esat::DrawText(272,311, pokenumber);
			esat::DrawSetFillColor(88,88,80);
			esat::DrawText(270,309, pokenumber);
		sprintf(pokenumber, "%03d", Aux->number + 5);
			esat::DrawSetFillColor(168,184,184);
			esat::DrawText(272,343, pokenumber);
			esat::DrawSetFillColor(88,88,80);
			esat::DrawText(270,341, pokenumber);
		sprintf(pokenumber, "%03d", Aux->number + 6);
			esat::DrawSetFillColor(168,184,184);
			esat::DrawText(272,375, pokenumber);
			esat::DrawSetFillColor(88,88,80);
			esat::DrawText(270,373, pokenumber);


		if(esat::IsSpecialKeyDown(esat::kSpecialKey_Enter) || esat::MouseWheelY() > Mouse){
			Aux = Aux->Next;
			Mouse = esat::MouseWheelY();
		}
		if(esat::IsSpecialKeyDown(esat::kSpecialKey_Backspace) || esat::MouseWheelY() < Mouse && Aux->Prev != NULL){
			Aux = Aux->Prev;
			Mouse = esat::MouseWheelY();
		}
		esat::DrawEnd();

	}else{
    manager = 0;
		background = esat::SpriteFromFile("hud/menu.png");
		esat::DrawSetFillColor(0,0,0);
  }

}
