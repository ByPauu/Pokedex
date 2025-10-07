#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
//Normal Libs

#include "esat_extra/sqlite3.h"
//ESAT Libs



int main(int argc, char* argv){
  sqlite3 *db;
  sqlite3_stmt *stmt;
  FILE *file;

  int32_t size;
  unsigned char *buffer;
  int rc;

  char route[50];
  int pokenumber = 1;

  rc = sqlite3_open("assets/sprites/pokemondb.db", &db);
  if(rc != SQLITE_OK){
    fprintf(stderr, "ERROR ABRIENDO LA BASE DE DATOS");
    return 1;
  }

  
  snprintf(route, sizeof(route), "assets/sprites/%4d", pokename);
  file = fopen(route, "rb");
  if(!file){
    sqlite3_close(db);
    return 0;
  }

  fseek(file, 0, SEEK_END);
  size = ftell(file);
  rewind(file);

  buffer = (char*) malloc(sizeof(size));
  fread(buffer, 1, size, file);
  fclose(file);
  

  sqlite3_bind_text(stmt, 1, "Pikachu", -1, SQLITE_STATIC);
  sqlite3_bind_blob(stmt, 2, buffer, file_size, SQLITE_STATIC);

  rc = sqlite3_step(stmt);
  if (rc != SQLITE_DONE) {
      fprintf(stderr, "Error insertando sprite: %s\n", sqlite3_errmsg(db));
  } else {
      printf("Sprite insertado correctamente (%.2f KB)\n", file_size / 1024.0);
  }

  sqlite3_finalize(stmt);
  sqlite3_close(db);
  free(buffer);

  return 0;
}