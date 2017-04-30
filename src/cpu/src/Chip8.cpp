//============================================================================
// Name        : Chip8.cpp
// Author      : Federico Calderon
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "../../cpu/inc/Chip8.h"

#include <iostream>
#include <stdio.h>

using namespace std;

Chip8::Chip8(){ /* Emtpy */ }
Chip8::~Chip8(){ /* Emtpy */ }

/*
 * Initialization function
 */
void Chip8::initialize(){
   display = { INDEX_OF_0 };
   keys = { INDEX_OF_0 };
   stack = { INDEX_OF_0 };
   memory = { INDEX_OF_0 };
   pc = PC_START_ADDR;
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
