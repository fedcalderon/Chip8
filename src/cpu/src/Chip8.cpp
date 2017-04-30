//============================================================================
// Name        : Chip8.cpp
// Author      : Federico Calderon
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "../../cpu/inc/Chip8.h"

#include <iostream>
#include <string>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
using namespace std;

Chip8::Chip8(){ /* Emtpy */ }
Chip8::~Chip8(){ /* Emtpy */ }

/*
 * Initialization function
 */
void Chip8::initializeChip8(){
   cout << "Entering initialize() ... " << endl;

   pc = APP_START_ADDR;
   opcode = INDEX_OF_0;
   I = INDEX_OF_0;
   stack_pointer = INDEX_OF_0;
   needRedraw = true;
   delay_timer = INDEX_OF_0;
   sound_timer = INDEX_OF_0;

   // Load fontset to memory
   for(int i = 0; i < FONTSET_SIZE; i++){
      memory[i] = fontset[i];
   }
}

/*
 * Load application program
 */
bool Chip8::loadApp(const char *file){
   bool rtn = false;

   // Initialize
   initializeChip8();

   // Open file, check for errors
   FILE *pApp = fopen(file, READ_MODE);
   if(pApp == NULL){
      cout << "Error opening \n" << file << " ... " << endl;
      return rtn;
   }
   else{
      cout << "Loading " << file << " ... " << endl;

      // Compute file size
      fseek(pApp, INDEX_OF_0, SEEK_END);
      long fileSize = ftell(pApp);

      // Allocate memory for the application, check for errors
      char *buff = (char*) malloc(sizeof(char) *fileSize);
      if(buff == NULL){
         cout << "Error loading app into memory ... " << endl;
         return rtn;
      }

      // Load app into virtual chip-8 memory, check if app size fits into app memory
      int allocated_space = MEMORY_SIZE - APP_START_ADDR;
      if(fileSize < allocated_space){
         for(int i = 0; i < fileSize; i++){
            memory[APP_START_ADDR + i] = buff[i];
         }
      }
      else{
         cout << "App is too large! " << endl;
         return rtn;
      }

      // Close file
      fclose(pApp);

      // Free buff
      free(buff);

      rtn = true;
   }

   return rtn;
}

/*
* Run emulator method
*/
void Chip8::runEmulator(){

}
