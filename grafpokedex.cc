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

struct SpriteIndex{
	int id;
	long offset;
	long size;
};

pokemon* Aux = NULL;
pokemon* Last = NULL;
pokemon* First = NULL;

esat::SpriteHandle background;
esat::SpriteHandle bar;

int manager = 0;
int auxnumber;
int auxkeynum = 0;

int Mouse = 0;
int numPokemons = 0;
float currentShown = 0;

char* auxnumberchar = (char*) malloc(sizeof(char) * 50);
char* auxname = (char*) malloc(sizeof(char) * 50);
char* pokenumber = (char*) malloc(sizeof(char) * 50);

unsigned char fps = 60; //Control de frames por segundo
double current_time,last_time;

void ReadPokedex();
void InsertPoke(pokemon* Poke);
void ShowPokedex();
void AddPokemonName();
void AddPokemonNumber(); 
void ShowHud();
void PokedexSort();
void DrawShadow(float x, float y, char* string);


int ExtractSprite(int number, const char *out_path);
void DeleteAllSprites();

void CleanLists();
void CleanPointers();

int esat::main(int argc, char **argv) {
	esat::WindowInit(512,392);
	WindowSetMouseVisibility(true);
  esat::DrawSetFillColor(0,0,0);
  esat::DrawSetTextSize(35);
	esat::DrawSetTextFont("assets/fonts/pokemon.ttf");

	background = esat::SpriteFromFile("assets/hud/menu.png");
	bar = esat::SpriteFromFile("assets/hud/bar.png");

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
			if(esat::WindowIsOpened()) esat::WindowFrame();
  }
  esat::WindowDestroy();

	CleanLists();
	CleanPointers();
	DeleteAllSprites();
  return 0;
    
}
void ReadPokedex() {
	char catched;
	
	FILE *pfile = fopen("assets/pokedex.txt", "r"); // Abrir el archivo
	if (pfile != NULL) {
			for (int i = 0; i < 721; i++) { // Leer hasta 800 Pokémon
					int j = 0; // Índice para leer el nombre

					fread(&catched, sizeof(char), 1, pfile); // Leer si está capturado
					fseek(pfile, 1, SEEK_CUR); // Saltar el espacio entre el número y el nombre

					// Leer el nombre caracter por caracter hasta salto de línea o EOF
					while (fread(&auxname[j], sizeof(char), 1, pfile) == 1 && auxname[j] != '\n') {
							j++;
					}
					auxname[j] = '\0'; // Terminar la cadena correctamente

					// Imprimir para comprobar que lee bien
					auxnumber = i + 1;
					// Llamar a PokedexSort() si está capturado
					if (catched == '1') {
						PokedexSort();
					}
					catched = '\0';
			}
			fclose(pfile);
	} else {
			printf("Error al abrir pokedex.txt\n");
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
      memset(auxname, '\0', 14);
      memset(auxnumberchar, '\0', 50);
			background = esat::SpriteFromFile("assets/hud/addpokemon.png");
      break;
    }
    case '2':{
			background = esat::SpriteFromFile("assets/hud/showpokedex.png");
      manager = 3;
			currentShown = 1;
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
	char auxkey = '\0';
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
	char auxkey = '\0';
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
		background = esat::SpriteFromFile("assets/hud/menu.png");
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
    strcpy(Poke->name, auxname);

    snprintf(Poke->filesprite, 50, "assets/sprites/%04d.png", Poke->number);

    // Extraer el sprite directamente desde sprites.dat
    ExtractSprite(Poke->number, Poke->filesprite);

    Poke->sprite = esat::SpriteFromFile(Poke->filesprite);

    numPokemons++;
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
	if(Aux != NULL){
		esat::DrawBegin();
		esat::DrawClear(0,0,0);
		esat::DrawSprite(background,0,0);
		esat::DrawSprite(Aux->sprite, 15, 100);

		float baroffset = (float) 200 / numPokemons;
		esat::DrawSprite(bar, 468, currentShown * baroffset + 68.0f);
		
		snprintf(pokenumber,50, "%03d", Aux->number);

		//Poke Down Middle name
		DrawShadow(30,80, Aux->name);
    
		//RIGHT POKEDEX SIDE NUMBERS
		for(int i = -3; i < 7; i++){
			if(Aux->number + i > 0 && Aux->number + i <= 721){
				pokemon* PokeName = Aux;
				int e = 0;
				while(e != i){
					if(i < 0){
						PokeName = PokeName->Prev;
						e--;
					}else if(i > 0){
						PokeName = PokeName->Next;
						e++;
					}
				}
				snprintf(pokenumber, 14, "%03d", Aux->number + i);
				float nextrow = 177 + i * 33;
				DrawShadow(270, nextrow, pokenumber);
				DrawShadow(320, nextrow, PokeName->name);
			}
		}
		if(esat::IsSpecialKeyPressed(esat::kSpecialKey_Enter) || esat::MouseWheelY() < Mouse){
			Mouse = esat::MouseWheelY();
			Aux = Aux->Next;
			currentShown++;
		}
		if(esat::IsSpecialKeyPressed(esat::kSpecialKey_Backspace) || esat::MouseWheelY() > Mouse){
			Mouse = esat::MouseWheelY();
			Aux = Aux->Prev;
			currentShown--;
		}
		esat::DrawEnd();

	}else{
    manager = 0;
		background = esat::SpriteFromFile("assets/hud/menu.png");
		esat::DrawSetFillColor(0,0,0);
  }

}


void DrawShadow(float x, float y, char* string){
	esat::DrawSetFillColor(168,184,184);
	esat::DrawText(x + 2, y + 2, string);
	esat::DrawSetFillColor(88,88,80);
	esat::DrawText(x, y, string);
}

void CleanLists(){
	Aux = First;
	pokemon* tmp;
	while(Aux != NULL){
		tmp = Aux->Next;
		free(Aux);
		Aux = tmp;
	}
	First = NULL;
	Last = NULL;
	printf("No more Lists\n");
}

void CleanPointers(){
	free(auxnumberchar);
	free(auxname);
	free(pokenumber);
	printf("No more pointers\n");
}


void DeleteAllSprites(){
	char filename[64];
	for(int i = 1; i <= 721; i++){
		snprintf(filename, sizeof(filename), "assets/sprites/%04d.png", i);
		if(remove(filename) == 0){
			printf("Sprite %s eliminado.\n", filename);
		}
	}
}

//CHAT GPT TO DESCROMPRESSS SPRITES FROM A .DAT FILE
//Create a file that descompresses a .dat file that was compressed with this composition (struct) to store png
int ExtractSprite(int number, const char *out_path) {
    FILE *in = fopen("assets/sprites/sprites.dat", "rb");
    if (!in) { perror("fopen"); return 1; }

    // Saltar al índice específico en la cabecera
    fseek(in, (number - 1) * sizeof(SpriteIndex), SEEK_SET);

    SpriteIndex idx;
    if (fread(&idx, sizeof(SpriteIndex), 1, in) != 1) {
        printf("Error leyendo índice %d\n", number);
        fclose(in);
        return 1;
    }

    // Ir al bloque de datos
    fseek(in, idx.offset, SEEK_SET);

    char *buffer = (char*) malloc(idx.size);
    if (!buffer) { perror("malloc"); fclose(in); return 1; }

    if (fread(buffer, 1, idx.size, in) != (size_t)idx.size) {
        printf("Error leyendo sprite %d\n", number);
        free(buffer);
        fclose(in);
        return 1;
    }

    FILE *out = fopen(out_path, "wb");
    if (!out) { perror("fopen salida"); free(buffer); fclose(in); return 1; }

    fwrite(buffer, 1, idx.size, out);

    fclose(out);
    free(buffer);
    fclose(in);
    return 0;
}

