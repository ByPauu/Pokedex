//GrafPokedex -> A brew implementation of a pokedex in c++, It doesn't have poke info, just sprites until 7 gen
//ByPauu -> 05/04/2025 - 07/10/2025
//This was started a few months ago that i didn't program like now, so its a little updated

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//Normal Libs

#include "esat/window.h"
#include "esat/draw.h"
#include "esat/input.h"
#include "esat/sprite.h"
#include "esat/time.h"
//Custom ESAT libs

//Struct that gets every pokemon
struct pokemon{
	char name[50]; 										//Name of that pokemon
	int number;												//Pokedex number
	esat::SpriteHandle sprite = NULL;	//Sprite handler of that poke
  char filesprite[50];							//Route to that sprites

	pokemon* Next;										//List next indexer
	pokemon* Prev;										//List previous indexer
};

struct SpriteIndex{									//Struct to store sprites while init
	int id;
	int32_t offset;
	int32_t size;
};
//int32_t is used instead of long bc on otehr OS (macOS) long store 8bytes
//and spriteindex needs a variable of 4bytes (32bits)

struct GameTools{
	int manager = 0;
	int auxnumber;
	int auxkeynum = 0;

	int Mouse = 0;
	int numPokemons = 0;
	int currentShown = 0;

	char auxnumberchar[50];
	char auxname[50];

	esat::SpriteHandle background;
	esat::SpriteHandle bar;
};

struct Lists{
	pokemon* Aux;
	pokemon* Last;
	pokemon* First;
};


void ReadPokedex(Lists& list, GameTools& tool);
void InsertPoke(pokemon* Poke, GameTools& tool);

void ShowPokedex(Lists& list, GameTools& tool);
void AddPokemonName(GameTools& tool);
void AddPokemonNumber(Lists& list, GameTools& tool); 
void ShowHud(Lists& list, GameTools& tool);
void PokedexSort(Lists& list, GameTools& tool);
void DrawShadow(float x, float y, char* string);

int ExtractSprite(int number, const char *out_path);
void DeleteAllSprites();

void CleanLists(Lists& list);
void CleanPointers();

void WindowInit(GameTools& tool, Lists& list);

int esat::main(int argc, char **argv) {

	unsigned char fps = 60; //Control de frames por segundo
	double current_time = 0, last_time = 0;

	GameTools tool;
	Lists list;

	printf("%d",tool.manager);
	WindowInit(tool, list);

	ReadPokedex(list, tool);
    while(esat::WindowIsOpened() && !esat::IsSpecialKeyDown(esat::kSpecialKey_Escape)) {
      //Control fps 
    	do{
    		current_time = esat::Time();
    	}while((current_time-last_time)<=1000.0/fps);

    	last_time = esat::Time(); 
			
			switch(tool.manager){
				case 0:{ ShowHud(list, tool); break; }
				case 1:{ AddPokemonName(tool);break; }
				case 2:{ AddPokemonNumber(list, tool); break; }
				case 3:{ ShowPokedex(list, tool); break; }
				case 4:{ esat::WindowDestroy(); break; }
			}
			
    	esat::ResetBufferdKeyInput();
			if(esat::WindowIsOpened()) esat::WindowFrame();
  }
  esat::WindowDestroy();

	CleanLists(list);
	CleanPointers();
	DeleteAllSprites();
  return 0;
    
}

void WindowInit(GameTools& tool, Lists& list){
	//WindowSettings
	esat::WindowInit(512,392);
	esat::WindowSetMouseVisibility(true);

	//Drawing Settings
  esat::DrawSetFillColor(0,0,0);
  esat::DrawSetTextSize(35);
	esat::DrawSetTextFont("assets/fonts/pokemon.ttf");

	//Setting background
	tool.background = esat::SpriteFromFile("assets/hud/menu.png");
	tool.bar = esat::SpriteFromFile("assets/hud/bar.png");

	//Reset Key Input
	esat::ResetBufferdKeyInput();


	list.Aux = NULL;
	list.Last = NULL;
	list.First = NULL;
}

void ReadPokedex(Lists& list, GameTools& tool) {
	char catched;
	
	FILE *pfile = fopen("assets/pokedex.txt", "r"); // Abrir el archivo
	if (pfile != NULL) {
			for (int i = 0; i < 721; i++) { // Leer hasta 800 Pokémon
					int j = 0; // Índice para leer el nombre

					fread(&catched, sizeof(char), 1, pfile); // Leer si está capturado
					fseek(pfile, 1, SEEK_CUR); // Saltar el espacio entre el número y el nombre

					// Leer el nombre caracter por caracter hasta salto de línea o EOF
					while (fread(&tool.auxname[j], sizeof(char), 1, pfile) == 1 && tool.auxname[j] != '\n') {
							j++;
					}
					tool.auxname[j] = '\0'; // Terminar la cadena correctamente

					// Imprimir para comprobar que lee bien
					tool.auxnumber = i + 1;
					// Llamar a PokedexSort() si está capturado
					if (catched == '1') {
						PokedexSort(list, tool);
					}
					catched = '\0';
			}
			fclose(pfile);
	} else {
			printf("Error al abrir pokedex.txt\n");
	}
}



void ShowHud(Lists& list, GameTools& tool){

	esat::DrawBegin();
	esat::DrawClear(0,0,0);
	esat::DrawSprite(tool.background,0,0);
	
	esat::DrawText(86,117,"1. Add Pokemon:");
	esat::DrawText(86,173,"2. Show Pokedex:");
	esat::DrawText(86,227,"3. Exit:");
	esat::DrawText(86,280,"Insert number:");
	esat::DrawEnd();

  switch(esat::GetNextPressedKey()){
    case '1':{
      tool.manager = 1;
      tool.auxnumber = 0;
      memset(tool.auxname, '\0', 14);
      memset(tool.auxnumberchar, '\0', 50);
			tool.background = esat::SpriteFromFile("assets/hud/addpokemon.png");
      break;
    }
    case '2':{
			tool.background = esat::SpriteFromFile("assets/hud/showpokedex.png");
      tool.manager = 3;
			tool.currentShown = 1;
      list.Aux = list.First;
			tool.Mouse = esat::MouseWheelY();
      break;
    }
    case '3':{
      tool.manager = 4;
      break;
    }
  }

	esat::ResetBufferdKeyInput();
}

void AddPokemonName(GameTools& tool){
	char auxkey = '\0';
	esat::DrawBegin();
	esat::DrawClear(0,0,0);
	esat::DrawSprite(tool.background,0,0);
	esat::DrawText(60,150, "Introduce el nombre del pokemon:");
	esat::DrawText(60,226, tool.auxname );

	esat::DrawEnd();
	auxkey = esat::GetNextPressedKey();
	if(esat::IsSpecialKeyDown(esat::kSpecialKey_Enter)){
		tool.manager = 2;
		tool.auxkeynum = 0;
	}else if(esat::IsSpecialKeyDown(esat::kSpecialKey_Backspace)){
		if(tool.auxkeynum > 0){
			tool.auxkeynum--;
			tool.auxname[tool.auxkeynum] = '\0';
		}
	}else if(auxkey != '\0'){
		if(tool.auxkeynum > 0 && auxkey != 32){
			auxkey += 32;
		}
		tool.auxname[tool.auxkeynum] = auxkey;
		tool.auxkeynum++;
	}
	esat::ResetBufferdKeyInput();
	
}

void AddPokemonNumber(Lists& list, GameTools& tool){
	char auxkey = '\0';
	esat::DrawBegin();
	esat::DrawClear(0,0,0);
	esat::DrawSprite(tool.background,0,0);
	esat::DrawText(60,150, "Introduce el numero del pokemon:");
	esat::DrawText(60,226, tool.auxnumberchar);

	esat::DrawEnd();
	auxkey = esat::GetNextPressedKey();
	if(auxkey >= 48 && auxkey <= 57){
		tool.auxnumberchar[tool.auxkeynum] = auxkey;
		tool.auxkeynum++;
	}
	if(esat::IsSpecialKeyDown(esat::kSpecialKey_Enter)){
		tool.manager = 0;
		tool.background = esat::SpriteFromFile("assets/hud/menu.png");
		tool.auxkeynum = 0;
		tool.auxnumber = atoi(tool.auxnumberchar);
		PokedexSort(list, tool);
	}
	if(esat::IsSpecialKeyDown(esat::kSpecialKey_Backspace)){
		if(tool.auxkeynum > 0){
			tool.auxkeynum--;
			tool.auxnumberchar[tool.auxkeynum] = '\0';
		}
	}
	esat::ResetBufferdKeyInput();

}

void InsertPoke(pokemon* Poke, GameTools& tool){
    Poke->number = tool.auxnumber;
    strcpy(Poke->name, tool.auxname);

    snprintf(Poke->filesprite, 50, "assets/sprites/%04d.png", Poke->number);

    // Extraer el sprite directamente desde sprites.dat
    ExtractSprite(Poke->number, Poke->filesprite);

    Poke->sprite = esat::SpriteFromFile(Poke->filesprite);

    tool.numPokemons++;
}

void PokedexSort(Lists& list, GameTools& tool){
	//list.FIRST LIST
  if(list.First == NULL){
    list.Aux = (pokemon*) malloc(sizeof(pokemon)); //Creating the fist list

    InsertPoke(list.Aux, tool);
		list.First = list.Aux;
    list.Last = list.First;
		list.Aux->Next = NULL;
		list.Aux->Prev = NULL;
  }else{
		//LIST BEFORE list.FIRST
		if(tool.auxnumber < list.First->number){ 
			list.Aux = (pokemon*) malloc(sizeof(pokemon)); //Creating the fist list
      list.Aux->Next = list.First;
			list.First->Prev = list.Aux;
      list.First = list.Aux;
      
      InsertPoke(list.Aux, tool);
		//LIST AFTER list.LAST
    }else if(tool.auxnumber > list.Last->number){
			list.Aux = (pokemon*) malloc(sizeof(pokemon)); //Creating the fist list
      list.Last->Next = list.Aux;
			list.Aux->Prev = list.Last;
      list.Last = list.Aux;
      
      InsertPoke(list.Aux, tool);
			list.Aux->Next = NULL;
		//LIST BETWEEN
		}else if(tool.auxnumber > list.First->number && tool.auxnumber < list.Last->number){
			list.Aux = list.First;
      while(tool.auxnumber > list.Aux->Next->number){
        list.Aux = list.Aux->Next;
      }
			pokemon* Current = (pokemon*) malloc(sizeof(pokemon)); //Creating the fist list
			if(Current != NULL){
				Current->Next = list.Aux->Next;
				Current->Prev = list.Aux;
				list.Aux->Next = Current;
				
				InsertPoke(Current, tool);
			}
    }
  }
}

void ShowPokedex(Lists& list, GameTools& tool){
	if(list.Aux != NULL){
		esat::DrawBegin();
		esat::DrawClear(0,0,0);
		esat::DrawSprite(tool.background,0,0);
		esat::DrawSprite(list.Aux->sprite, 15, 100);

		float baroffset = (float) 200 / tool.numPokemons;
		esat::DrawSprite(tool.bar, 468, (float) tool.currentShown * baroffset + 68.0f);

		//Poke Down Middle name
		DrawShadow(30,80, list.Aux->name);
    
		//RIGHT POKEDEX SIDE NUMBERS
		for(int i = -3; i < 7; i++){
			if(list.Aux->number + i > 0 && list.Aux->number + i <= 721){
				pokemon* PokeName = list.Aux;
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
				char pokenumber[5];
				snprintf(pokenumber, 14, "%03d", list.Aux->number + i);
				float nextrow = 177 + i * 33;
				DrawShadow(270, nextrow, pokenumber);
				DrawShadow(320, nextrow, PokeName->name);
			}
		}
		if(esat::IsSpecialKeyPressed(esat::kSpecialKey_Enter) || esat::MouseWheelY() < tool.Mouse){
			tool.Mouse = esat::MouseWheelY();
			list.Aux = list.Aux->Next;
			tool.currentShown++;
		}
		if(esat::IsSpecialKeyPressed(esat::kSpecialKey_Backspace) || esat::MouseWheelY() > tool.Mouse){
			tool.Mouse = esat::MouseWheelY();
			list.Aux = list.Aux->Prev;
			tool.currentShown--;
		}
		esat::DrawEnd();

	}else{
    tool.manager = 0;
		tool.background = esat::SpriteFromFile("assets/hud/menu.png");
		esat::DrawSetFillColor(0,0,0);
  }

}

void DrawShadow(float x, float y, char* string){
	esat::DrawSetFillColor(168,184,184);
	esat::DrawText(x + 2, y + 2, string);
	esat::DrawSetFillColor(88,88,80);
	esat::DrawText(x, y, string);
}

void CleanLists(Lists& list){
	list.Aux = list.First;
	pokemon* tmp;
	while(list.Aux != NULL){
		tmp = list.Aux->Next;
		free(list.Aux);
		list.Aux = tmp;
	}
	list.First = NULL;
	list.Last = NULL;
	printf("No more Lists\n");
}

void CleanPointers(){
	//free(pointer)
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

//DESCROMPRESSS SPRITES FROM A .DAT FILE
//Not mine function
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

