#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>

#include "esat/window.h"
#include "esat/draw.h"
#include "esat/input.h"
#include "esat/sprite.h"
#include "esat/time.h"


struct pokemon{
	char* name;
	int number;
	esat::SpriteHandle sprite;
	pokemon* Next;
};

pokemon* Aux = NULL;
pokemon* Last = NULL;
pokemon* First = NULL;


int auxnumber;
char* auxname = (char*) malloc(sizeof(char) * 50); //Adding Slots into array

char game_manager;
int salir = 0;
int number_of_pokemon = 0;

void PressKey();
void ShowPokedex();
void AddPokemon();
void ShowHud(char &game_manager);
void PokedexSort();

int main(int argc, char **argv) {

	while(salir != 1){
		ShowHud(game_manager);

		switch(game_manager){
			case '1':{
				AddPokemon();
				break;
			}
			case '2':{
				ShowPokedex();
				break;
			}
			case '3':{
				salir = 1;
				break;
			}
		}
	}
  return 0;
}
void ShowHud(char &game_manager){
	//system("clear");
	system("cls");
	printf("\nPokedex:");
	printf("\n1. Add Pokemon:");
	printf("\n2. Show Pokedex:");
	printf("\n3. Exit:");
	printf("\nInsert Number: ");

  //scanf("%c", &game_manager);
	game_manager = getch();
}

void AddPokemon(){
	if(auxname != NULL){
		system("cls");
		//system("clear");

		
		printf("Introduce el nombre del Pokemon: ");
		scanf("%s", &(*auxname));
		printf("Introduce el numero de entrada del Pokemon: ");
		scanf("%d", &auxnumber);

		system("cls");
		//system("clear");
		printf("\n<--------------------------->");
		printf("\n#%03d-->%s", auxnumber, auxname);
		printf("\n<--------------------------->");

		printf("\n\nPulsa una tecla para continuar: ");
		number_of_pokemon++;
		PressKey();

		PokedexSort();
	}
}
void PokedexSort(){
  if(First == NULL){
    Aux = (pokemon*) malloc(sizeof(pokemon)); //Creating the fist list

    Aux->name = (char*) malloc(sizeof(char) * 50); //Adding Slots into array
    Aux->number = auxnumber;
    strcpy(Aux->name, &(*auxname));
		First = Aux;
    Last = First;
		Aux->Next = NULL;
  }else{
		if(auxnumber < First->number){ 
			Aux = (pokemon*) malloc(sizeof(pokemon)); //Creating the fist list
      Aux->Next = First;
      First = Aux;
      
      First->name = (char*) malloc(sizeof(char) * 50); //Adding Slots into array
      First->number = auxnumber;
      strcpy(First->name, &(*auxname));
    }else if(auxnumber > Last->number){
			Aux = (pokemon*) malloc(sizeof(pokemon)); //Creating the fist list
      Last->Next = Aux;
      Last = Aux;
      
      Aux->name = (char*) malloc(sizeof(char) * 50); //Adding Slots into array
      Aux->number = auxnumber;
      strcpy(Aux->name, &(*auxname));
			Aux->Next = NULL;
		}else if(auxnumber > First->number && auxnumber < Last->number){
			Aux = First;
      while(auxnumber > Aux->Next->number){
        Aux = Aux->Next;
      }
			
			pokemon* Current = (pokemon*) malloc(sizeof(pokemon)); //Creating the fist list
			if(Current != NULL){
				Current->Next = Aux->Next;
				Aux->Next = Current;
				
				Current->name = (char*) malloc(sizeof(char) * 50); //Adding Slots into array
				Current->number = auxnumber;
				strcpy(Current->name,&(*auxname));
			}
    }
  }
}

void ShowPokedex(){
  system("cls");
	//system("clear");
	Aux = First;
	printf("\nYou have a total of %d of pokemon", number_of_pokemon);
	printf("\n<--------------------------->");
	while(Aux != NULL){
		printf("\n#%03d-->%s", Aux->number, Aux->name);
		Aux = Aux->Next;
	}
	printf("\n<--------------------------->");
	printf("\nPulsa una tecla para continuar: ");

  PressKey();
}

void PressKey(){
	//int auxliar;
  //scanf("%d", &auxliar);
	char auxiliar = getch();
}