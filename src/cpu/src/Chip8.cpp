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
#include <iomanip>

using namespace std;

Chip8::Chip8(){ /* Emtpy */ }
Chip8::~Chip8(){ /* Emtpy */ }

/*
 * Initialization function
 */
void Chip8::initializeChip8(){
   cout << "Entering initialize() ... " << endl;

   pc = (char) APP_START_ADDR;
   opcode = INDEX_OF_0;
   I = INDEX_OF_0;
   stack_pointer = INDEX_OF_0;
   repaint = true;
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
 * Run display method
 */
void Chip8::runDisplay(){

   while(runEmulator()){
      // Draw to the screen
      cout << "Drawing to the screen ... " << endl;
   }
}

/*
* Run emulator method
*/
bool Chip8::runEmulator(){
   bool rtn = true;
   /*
    * Get the opcode. Opcode is 2 bytes long, therefore can obtain as follows:
    *
    * 1. Read first byte from memory[pc]
    * 2. Shift 8 bits to the left
    * 3. Read contiguous byte from memory[pc+1]
    * 4. Bitwise OR both values to form opcode
    */
   unsigned short first_byte = memory[pc];
   first_byte = first_byte << INDEX_OF_8;
   unsigned short second_byte = memory[pc + INDEX_OF_1];
   opcode = first_byte | second_byte;
   // For debugging
   log("opcode", opcode);

   /*
    * Decode opcode.
    * Per the spec, the most significant digit shows the type of operation and the
    * remainder digits are to be parsed to know what to do. Obtain as follows:
    *
    * 1. In order to obtain the first digit, bitwise AND the opcode with 0xF000.
    * 2. In a switch statement, jump based on first digit.
    * 3. Once in a valid case, process the opcode.
    */
   unsigned short opcode_type = opcode & OPCODE_OUTER_MASK;
   log("opcode_type", opcode_type);

   // Outer switch to control the types of opcodes
   switch (opcode_type) {
      case TYPE_0:{  // Display and flow
         // Inner switch to process internal options
         unsigned short opcode_internal_type = opcode_type & OPCODE_INNER_MASK;
         log("opcode_internal_type", opcode_internal_type);

         switch (opcode_internal_type) {
            case CLEAR_SCREEN:{   // 00E0: Clears the screen
               display[DISP_HOR * DISP_VER] = { 0 };
               pc += 2;
               repaint = true;
               break;
            }
            case RTN_SUBROUTINE:{ // 00EE: Returns from a subroutine.
               break;
            }
            default:{
               rtn = false;
               log("value is invalid!", opcode_internal_type);
               break;
            }
         }
         break;
      }
      case TYPE_1:{   // Jumps to address NNN.
               break;
      }
      case TYPE_2:{   // Calls subroutine at NNN.
               break;
      }
      case TYPE_3:{   // Skips the next instruction if VX equals NN. (Usually the next instruction is a jump to skip a code block)
               break;
      }
      case TYPE_4:{   // Skips the next instruction if VX doesn't equal NN. (Usually the next instruction is a jump to skip a code block)
               break;
      }
      case TYPE_5:{   // Skips the next instruction if VX equals VY. (Usually the next instruction is a jump to skip a code block)
               break;
      }
      case TYPE_6:{   // Sets VX to NN.
               break;
      }
      case TYPE_7:{   // Adds NN to VX.
               break;}

      case TYPE_8:{   // Math operations
         // Inner switch over last nibble to process internal options.
         unsigned short last_nibble = opcode & TYPE_000N;
         log("last_nibble", last_nibble);

         switch (last_nibble) {
            case INDEX_OF_0:{  // 8XY0: sets VX to the value of VY
               break;
            }
            case INDEX_OF_1:{  // 8XY1: Sets VX to VX or VY. (Bitwise OR operation) VF is reset to 0.
               break;
            }
            case INDEX_OF_2:{  // 8XY2: Sets VX to VX and VY. (Bitwise AND operation) VF is reset to 0.
               break;
            }
            case INDEX_OF_3:{  // 8XY3: Sets VX to VX xor VY. VF is reset to 0.
               break;
            }
            case INDEX_OF_4:{  // 8XY4: Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't.
               break;
            }
            case INDEX_OF_5:{  // 8XY5: VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
               break;
            }
            case INDEX_OF_6:{  // 8XY6: Shifts VX right by one. VF is set to the value of the least significant bit of VX before the shift.
               break;
            }
            case INDEX_OF_7:{  // 8XY7: Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
               break;
            }
            case INDEX_OF_E:{  // 8XYE: Shifts VX left by one. VF is set to the value of the most significant bit of VX before the shift.
               break;
            }
            default:{
               rtn = false;
               log("value is invalid!", last_nibble);
               break;
            }
         }
               break;
      }
      case TYPE_9:{    // Skips the next instruction if VX doesn't equal VY. (Usually the next instruction is a jump to skip a code block)
               break;
      }
      case TYPE_A:{    // Sets I to the address NNN.
               break;
      }
      case TYPE_B:{    // Jumps to the address NNN plus V0.
               break;
      }
      case TYPE_C:{    // Sets VX to the result of a bitwise and operation on a random number (Typically: 0 to 255) and NN.
               break;
      }
         /*
          * Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels.
          * Each row of 8 pixels is read as bit-coded starting from memory location I;
          * I value doesn’t change after the execution of this instruction.
          * As described above, VF is set to 1 if any screen pixels are flipped from set to unset
          * when the sprite is drawn, and to 0 if that doesn’t happen
          */
      case TYPE_D:{
               break;
      }
      case TYPE_E:{               // KeyOp
         unsigned short last_byte = opcode & TYPE_00NN;
         log("last_byte", last_byte);

         switch (last_byte) {
            case IF_KEY_PRESSED:{   // Skips the next instruction if the key stored in VX is pressed. (Usually the next instruction is a jump to skip a code block)
               break;
            }
            case IF_KEY_NOT_PRESSED:{   // Skips the next instruction if the key stored in VX isn't pressed. (Usually the next instruction is a jump to skip a code block)
               break;
            }
            default:{
               rtn = false;
               log("value is invalid!", last_byte);
               break;
            }
         }
               break;
      }
      /*
       * Timer, KeyOp, sound, mem, bcd
       */
      case TYPE_F:{
         unsigned short lastbyte = opcode & TYPE_00NN;
         log("lastbyte", lastbyte);

         switch (lastbyte) {
            case TYPE_TIMER:{   // Sets VX to the value of the delay timer.
               break;
            }
            case AWAITED_KEY_PRESSED:{   // A key press is awaited, and then stored in VX. (Blocking Operation. All instruction halted until next key event)
               break;
            }
            case DELAY_TIMER_TO_VX:{   // Sets the delay timer to VX.
               break;
            }
            case SOUND_TIME_TO_VX:{   // Sets the sound timer to VX.
               break;
            }
            case ADD_VX_TO_I:{   // Adds VX to I.
               break;
            }
            case SPRITE_LOCATION:{   // Sets I to the location of the sprite for the character in VX. Characters 0-F (in hexadecimal) are represented by a 4x5 font.
               break;
            }
            /*
             * Stores the binary-coded decimal representation of VX, with the most significant of three
             * digits at the address in I, the middle digit at I plus 1, and the least significant digit
             * at I plus 2. (In other words, take the decimal representation of VX, place the hundreds
             * digit in memory at location in I, the tens digit at location I+1, and the ones digit at
             * location I+2.)
             */
            case BIN_CODED_DEC:{
               break;
            }
            case V0_TO_VX:{   // Stores V0 to VX (including VX) in memory starting at address I.
               break;
            }
            case FILL_V0_TO_VX:{   // Fills V0 to VX (including VX) with values from memory starting at address I.
               break;
            }
            default:{
               rtn = false;
               log("value is invalid!", lastbyte);
               break;
            }
         }
               break;
      }

      default:{
         break;
      }
   }

   return rtn;
}

/*
 * Print messages to the console for debugging
 */
void Chip8::log(const char *type, unsigned short n){
   cout << std::showbase << std::hex;
   cout << type << " = " << std::uppercase << n << endl;
}
