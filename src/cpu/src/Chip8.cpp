//============================================================================
// Name        : Chip8.cpp
// Author      : Federico Calderon
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
// Opcode description taken from https://en.wikipedia.org/wiki/CHIP-8
//============================================================================

#include "../../cpu/inc/Chip8.h"

#include <iostream>
#include <string>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iomanip>

using namespace std;

Chip8::Chip8() { /* Emtpy */
}
Chip8::~Chip8() { /* Emtpy */
}

/*
 * Initialization function
 */
void Chip8::initializeChip8() {
   cout << "Entering initialize() ... " << endl;

   progCounter = APP_START_ADDR;
   opcode = COEFF_OF_0;
   indexReg = COEFF_OF_0;
   stack_pointer = COEFF_OF_0;
   repaint = true;
   delay_timer = COEFF_OF_0;
   sound_timer = COEFF_OF_0;

   // Load fontset to memory
   for (int i = 0; i < FONTSET_SIZE; i++) {
      memory[FONTSET_SIZE + i] = fontset[i];
   }
}

/*
 * Load application program
 */
bool Chip8::loadApp(const char *file) {
   // Initialize
   initializeChip8();
   // Open file, check for errors
   FILE *pApp = fopen(file, READ_MODE);
   if (pApp == NULL) {
      cout << "Error opening \n" << file << " ... " << endl;
      return false;
   }
   cout << "Loading " << file << " ... " << endl;
   // Compute file size
   fseek(pApp, COEFF_OF_0, SEEK_END);
   long fileSize = ftell(pApp);
   rewind(pApp);
   printf("Filesize: %d\n", (int)fileSize);
   // Allocate memory for the application, check for errors
   char *buff = (char*) malloc(sizeof(char) * fileSize);
   if (buff == NULL) {
      cout << "Error loading app into memory ... " << endl;
      return false;
   }
   // Copy the file into the buffer
   fread (buff, 1, fileSize, pApp);

   // Load app into virtual chip-8 memory, check if app size fits into app memory
   int allocated_space = MEMORY_SIZE - APP_START_ADDR;
   if (fileSize < allocated_space) {
      for (int i = 0; i < fileSize; i++) {
         memory[APP_START_ADDR + i] = (char) buff[i];
//         log("", memory[APP_START_ADDR + i]);
      }
   }
   else {
      cout << "App is too large! " << endl;
      return false;
   }
   // Close file
   fclose(pApp);
   // Free buff
   free(buff);
   return true;
}

/*
 * Run display method
 */
void Chip8::runDisplay() {

   while (runEmulator()) {
      // Draw to the screen
      cout << "Drawing to the screen ... " << endl;
   }
}

/*
 * Run emulator method
 */
bool Chip8::runEmulator() {
   bool rtn = true;
   /*
    * Get the opcode. Opcode is 2 bytes long, therefore can obtain as follows:
    *
    * 1. Read first byte from memory[pc]
    * 2. Shift 8 bits to the left
    * 3. Read contiguous byte from memory[pc+1]
    * 4. Bitwise OR both values to form opcode
    */
   unsigned short first_byte = memory[progCounter];
   first_byte = first_byte << COEFF_OF_8;
   unsigned short second_byte = memory[progCounter + COEFF_OF_1];
   opcode = first_byte | second_byte;

   // For debugging. For tetris.c8, first opcode should be A2B4
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
   log("opcode type", opcode_type);

   // Outer switch to control the types of opcodes
   switch (opcode_type) {

      // Display and flow
      case TYPE_0: {  // 0x0000
         unsigned short opcode_internal_type = opcode & OPCODE_INNER_MASK;
         log("opcode_internal_type", opcode_internal_type);
         rtn = processType0(opcode_internal_type);
         break;
      }
         // Jumps to address NNN.
      case TYPE_1: { // 0x1000
         // Get address by bitwise AND opcode with 0x0FFF
         int address = opcode & MASK_0FFF;
         log("Set program counter to address", address);
         // Set program counter to address
         progCounter = address;
         break;
      }
         // Calls subroutine at NNN.
      case TYPE_2: { // 0x2000
         // Save address in the stack
         stack[stack_pointer] = progCounter;
         log("stack[stack_pointer] = progCounter", progCounter);
         // Point to the next item in the stack
         stack_pointer++;
         // Set program counter to address NNN decoded from opcode
         progCounter = opcode & MASK_0FFF;
         log("Set program counter to address", progCounter);
         break;
      }
         // Skips the next instruction if VX equals NN.
         // (Usually the next instruction is a jump to skip a code block)
      case TYPE_3: {
         unsigned short X = extractSecNibble(opcode);
         unsigned short NN = opcode & MASK_00FF;
         if (V[X] == NN) {
            progCounter += COEFF_OF_4;
         }
         else {
            progCounter += COEFF_OF_2;
         }
         log("X", X);
         log("NN", NN);
         log("Program counter", progCounter);
         break;
      }
         // Skips the next instruction if VX doesn't equal NN.
         // (Usually the next instruction is a jump to skip a code block)
      case TYPE_4: {
         unsigned short X = extractSecNibble(opcode);
         unsigned short NN = opcode & MASK_00FF;
         if (V[X] != NN) {
            progCounter += COEFF_OF_4;
         }
         else {
            progCounter += COEFF_OF_2;
         }
         log("X", X);
         log("NN", NN);
         log("Program counter", progCounter);
         break;
      }
         // Skips the next instruction if VX equals VY.
         // (Usually the next instruction is a jump to skip a code block)
      case TYPE_5: { // 5XY0
         unsigned short X = extractSecNibble(opcode);
         unsigned short Y = extractThirdNibble(opcode);
         if (V[X] == V[Y]) {
            progCounter += COEFF_OF_4;
         }
         else {
            progCounter += COEFF_OF_2;
         }
         log("X", X);
         log("Y", Y);
         log("Program counter", progCounter);
         break;
      }
         // 6XNN - Sets VX to NN.
      case TYPE_6: {
         unsigned short X = extractSecNibble(opcode);
         unsigned short NN = opcode & MASK_00FF;
         V[X] = NN;
         progCounter += COEFF_OF_2;

         log("X", X);
         log("NN", NN);
         log("Program counter", progCounter);
         break;
      }
         // 7XNN - Adds NN to VX.
      case TYPE_7: {
         unsigned short X = extractSecNibble(opcode);
         unsigned short NN = opcode & MASK_00FF;
         V[X] += NN;
         progCounter += COEFF_OF_2;

         log("X", X);
         log("NN", NN);
         log("V[X]", V[X]);
         log("Program counter", progCounter);
         break;
      }
         // Math operations
      case TYPE_8: {
         // Inner switch over last nibble to process internal options.
         unsigned short oc = opcode & MASK_000F;
         unsigned short X = extractSecNibble(opcode);
         unsigned short Y = extractThirdNibble(opcode);
         log("X", X);
         log("Y", Y);
         log("opcode & 000F", oc);

         rtn = processType8(oc, X, Y);
         break;
      }
         // Skips the next instruction if VX doesn't equal VY.
         // (Usually the next instruction is a jump to skip a code block)
      case TYPE_9: { // 9XY0
         unsigned short X = extractSecNibble(opcode);
         unsigned short Y = extractThirdNibble(opcode);
         if (V[X] != V[Y]) {
            progCounter += COEFF_OF_4;
         }
         else {
            progCounter += COEFF_OF_2;
         }

         log("X", X);
         log("Y", Y);
         log("V[X]", V[X]);
         log("V[Y]", V[Y]);
         log("Program counter", progCounter);
         break;
      }
         // ANNN - Sets I to the address NNN.
      case TYPE_A: {
         unsigned short NNN = opcode & MASK_0FFF;
         indexReg = NNN;
         progCounter += COEFF_OF_2;

         log("index register", NNN);
         log("Program counter", progCounter);
         break;
      }
         // BNNN - Jumps to the address NNN plus V0.
      case TYPE_B: {
         unsigned short NNN = opcode & MASK_0FFF;
         progCounter = NNN + V[0];

         log("NNN", NNN);
         log("V[0]", V[0]);
         log("Program counter", progCounter);
         break;
      }
         // CXNN - Sets VX to the result of a bitwise and operation on a random number (Typically: 0 to 255) and NN.
      case TYPE_C: {
         unsigned short X = extractSecNibble(opcode);
         unsigned short NN = opcode & MASK_00FF;
         unsigned short random = rand() % MASK_00FF;
         V[X] = random & NN;
         progCounter += COEFF_OF_2;

         log("X", X);
         log("NN", NN);
         log("random", random);
         log("Program counter", progCounter);
         break;
      }
         /*
          * Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels.
          * Each row of 8 pixels is read as bit-coded starting from memory location I;
          * I value doesn’t change after the execution of this instruction.
          * As described above, VF is set to 1 if any screen pixels are flipped from set to unset
          * when the sprite is drawn, and to 0 if that doesn’t happen
          */
      case TYPE_D: { // DXYN
         rtn = processTypeD(opcode);
         progCounter += COEFF_OF_2;
         repaint = true;

         log("Program counter", progCounter);
         break;
      }
      case TYPE_E: {  // KeyOp
         unsigned short oc = opcode & MASK_00FF;
         log("last_byte", oc);
         int X = extractSecNibble(oc);
         rtn = processTypeE(oc, X);

         log("opcode & 00FF", oc);
         log("X", X);
         log("Program counter", progCounter);
         break;
      }
         /*
          * Timer, KeyOp, sound, mem, bcd
          */
      case TYPE_F: {
         unsigned short oc = opcode & MASK_00FF;
         int X = extractSecNibble(oc);
         rtn = processTypeF(oc, X);

         log("X", X);
         log("opcode & 00FF", oc);
         log("Program counter", progCounter);
         break;
      }

      default: {
         rtn = false;
         break;
      }
   }

   return rtn;
}

/*
 * This method processes type 0 opcode options
 */
bool Chip8::processType0(unsigned short opcode_internal_type) {
   bool rtn = true;
   // Inner switch to process internal options
   switch (opcode_internal_type) {
      case CLEAR_SCREEN: {   // 00E0: Clears the screen
         log("Clearing the screen", opcode_internal_type);
         display[DISP_HOR * DISP_VER] = {0};
         // Advance program counter by 2 bytes
         progCounter += COEFF_OF_2;
         repaint = true;

         log("Program counter", progCounter);
         break;
      }
      case RTN_SUBROUTINE: { // 00EE: Returns from a subroutine.
         log("return from a subroutine", 0);
         stack_pointer--;
         progCounter = stack[stack_pointer + COEFF_OF_2];
         progCounter += COEFF_OF_2;

         log("stack pointer", stack_pointer);
         log("stack[stack_pointer + 2]", stack[stack_pointer + COEFF_OF_2]);
         log("Program counter", progCounter);
         break;
      }
      default: {
         rtn = false;
         log("value is invalid!", opcode_internal_type);
         break;
      }
   }
   return rtn;
}

/*
 * Process type 8 opcodes
 */
bool Chip8::processType8(unsigned short oc, int X, int Y) {
   bool rtn = true;
   switch (oc) {
      // 8XY0: sets VX to the value of VY
      case COEFF_OF_0: {
         V[X] = V[Y];

         log("8XY0: V[X]", V[X]);
         break;
      }
         // 8XY1: Sets VX to VX or VY. (Bitwise OR operation) VF is reset to 0.
      case COEFF_OF_1: {
         V[X] = V[X] | V[Y];

         log("8XY1: V[X] bitwise OR V[Y]", V[X]);
         break;
      }
         // 8XY2: Sets VX to VX and VY. (Bitwise AND operation) VF is reset to 0.
      case COEFF_OF_2: {
         V[X] = V[X] & V[Y];

         log("8XY2: V[X] bitwise AND V[Y]", V[X]);
         break;
      }
         // 8XY3: Sets VX to VX xor VY. VF is reset to 0.
      case COEFF_OF_3: {
         V[X] = V[X] ^ V[Y];

         log("8XY1: V[X] bitwise XOR V[Y]", V[X]);
         break;
      }
         // 8XY4: Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't.
      case COEFF_OF_4: {
         V[X] += V[Y];
         if (V[Y] > (CARRY_LIMIT - V[X])) {
            V[ADDR_F] = COEFF_OF_1; // carry
            cout << "carry" << endl;
         }
         else {
            V[ADDR_F] = COEFF_OF_0; // no carry
            cout << "no carry" << endl;
         }

         log("8XY4: V[X] + V[Y]", V[X]);
         break;
      }
         // 8XY5: VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
      case COEFF_OF_5: {
         V[X] -= V[Y];
         if (V[X] > V[Y]) {
            V[ADDR_F] = COEFF_OF_1; // no borrow
            cout << "no borrow" << endl;
         }
         else {
            V[ADDR_F] = COEFF_OF_0; // borrow
            cout << "borrow" << endl;
         }

         log("8XY5: V[X] - V[Y]", V[X]);
         break;
      }
         // 8XY6: Shifts VX right by one. VF is set to the value of the least significant bit of VX before the shift.
      case COEFF_OF_6: {
         V[ADDR_F] = V[X] & MASK_1;
         V[X] = V[X] >> COEFF_OF_1;

         log("8XY6: V[F] bitwise AND 0x1", V[0xF]);
         log("8XY6: V[X] >> 1", V[X]);
         break;
      }
         // 8XY7: Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
      case COEFF_OF_7: {
         V[X] = V[Y] - V[X];
         if (V[X] > V[Y]) {
            V[ADDR_F] = COEFF_OF_1; // no borrow
            cout << "no borrow" << endl;
         }
         else {
            V[ADDR_F] = COEFF_OF_0; // borrow
            cout << "borrow" << endl;
         }

         log("8XY7: V[Y] - V[X]", V[X]);
         break;
      }
         // 8XYE: Shifts VX left by one. VF is set to the value of the most significant bit of VX before the shift.
      case COEFF_OF_E: {
         V[ADDR_F] = V[X] & MASK_80;
         V[X] = V[X] << COEFF_OF_1;

         log("8XYE: V[F] bitwise AND 0x80", V[0xF]);
         log("8XYE: V[X] << 1", V[X]);
         break;
      }
      default: {
         rtn = false;
         log("value is invalid!", oc);
         break;
      }
   }

   // Program counter is always incremented by 2 bytes after each opcode execution
   progCounter += COEFF_OF_2;
   return rtn;
}

/*
 * Process type D opcodes
 */
bool Chip8::processTypeD(unsigned short opcode_type) {
   bool rtn = true;
   unsigned short X = extractSecNibble(opcode_type);
   unsigned short Y = extractThirdNibble(opcode_type);
   unsigned short N = opcode_type & MASK_000F;
   V[ADDR_F] = COEFF_OF_0;

   log("type D: X", X);
   log("type D: Y", Y);
   log("type D: N", N);

   // Outer loop controls y-coordinate from 0 to height N
   for (int yCoord = 0; yCoord < N; yCoord++) {
      int line = memory[indexReg + yCoord];

      // Inner loop controls x-coordinate from 0 to 7, 8 pixels wide
      for (int xCoord = 0; xCoord < COEFF_OF_8; xCoord++) {
         unsigned short shifting_mask = 0b10000000 >> xCoord;
         int pixel = line & shifting_mask;
         if (pixel != 0) {
            int totalDrawX = (X + xCoord) % DISP_HOR;
            int totalDrawY = (Y + yCoord) % DISP_VER;
            int index = (totalDrawY * DISP_HOR) + totalDrawX;
            if (display[index] == 1) {
               V[ADDR_F] = COEFF_OF_1;
            }
            // As the shifting mask determines the value of pixel,
            // set the new value of display[index] as bitwise exclusive OR until pixel
            // is zero. Then, move to the next y-coordinate
            display[index] = display[index] ^ COEFF_OF_1;
         }
      }
   }

   return rtn;
}

/*
 * Process type E opcodes
 */
bool Chip8::processTypeE(unsigned short oc, int X) {
   bool rtn = true;

   switch (oc) {
      // Skips the next instruction if the key stored in VX is pressed.
      // (Usually the next instruction is a jump to skip a code block)
      case IF_KEY_PRESSED: { // 0x009E
         int key = V[X];
         if (keys[key] == COEFF_OF_1) {
            progCounter += COEFF_OF_4;
            cout << "key pressed" << endl;
         }
         else {
            progCounter += COEFF_OF_2;
            cout << "key not pressed" << endl;
         }
         break;
      }
         // Skips the next instruction if the key stored in VX isn't pressed.
         // (Usually the next instruction is a jump to skip a code block)
      case IF_KEY_NOT_PRESSED: { // 0x00A1
         int key = V[X];
         if (keys[key] == COEFF_OF_0) {
            progCounter += COEFF_OF_4;
            cout << "key not pressed" << endl;
         }
         else {
            progCounter += COEFF_OF_2;
            cout << "key pressed" << endl;
         }
         break;
      }
      default: {
         rtn = false;
         break;
      }
   }

   return rtn;
}

/*
 * Process type F opcodes
 */
bool Chip8::processTypeF(unsigned short oc, int X) {
   bool rtn = true;

   switch (oc) {
      // 0xFX07 - Sets VX to the value of the delay timer.
      case TYPE_TIMER: {
         V[X] = delay_timer;

         log("type FX07 -> V[X] = delay timer", V[X]);
         break;
      }
         // 0x000A - A key press is awaited, and then stored in VX.
         // (Blocking Operation. All instruction halted until next key event)
      case AWAITED_KEY_PRESSED: {
         cout << "type FX0A -> key pressed" << endl;
         for (int i = 0; i < NUM_KEYS; i++) {
            if (keys[i] == COEFF_OF_1) {
               V[X] = i;

               log("i", i);
               log("V[X]", V[X]);
               break;
            }
         }
         break;
      }
         // 0x0015 - Sets the delay timer to VX.
      case DELAY_TIMER_TO_VX: {
         delay_timer = V[X];

         log("type FX15 -> delay_timer = V[X]", delay_timer);
         break;
      }
         // 0X0018 - Sets the sound timer to VX.
      case SOUND_TIMER_TO_VX: {
         sound_timer = V[X];

         log("type FX18 -> sound_timer = V[X]", sound_timer);
         break;
      }
         // 0X001E - Adds VX to I.
         // VF is set to 1 when there is a range overflow (I+VX>0xFFF), and to 0 when there isn't.
         // This is an undocumented feature of the CHIP-8 and used by the Spacefight 2091! game.
      case ADD_VX_TO_I: {

         if ((indexReg + V[X]) > OVERFLOW_LIMIT) {
            V[ADDR_F] = COEFF_OF_1;
         }
         else {
            V[ADDR_F] = COEFF_OF_0;
         }
         indexReg += V[X];
         break;
      }
         // Sets I to the location of the sprite for the character in VX. Characters 0-F (in hexadecimal)
         // are represented by a 4x5 font.
      case SPRITE_LOCATION: {   // 0X0029
         int character_pixel = V[X];
         indexReg = 0x050 + (character_pixel * 5);
         break;
      }
         /*
          * Stores the binary-coded decimal representation of VX, with the most significant of three
          * digits at the address in I, the middle digit at I plus 1, and the least significant digit
          * at I plus 2. (In other words, take the decimal representation of VX, place the hundreds
          * digit in memory at location in I, the tens digit at location I+1, and the ones digit at
          * location I+2.)
          */
      case BIN_CODED_DEC: { //0X0033
         // Compute hundreds and tens
         int n = V[X];
         int hundreds = (n - (n % COEFF_OF_100)) / COEFF_OF_100;
         n = n - (hundreds * COEFF_OF_100);
         int tens = (n - (n % COEFF_OF_10)) / COEFF_OF_10;
         n = n - (tens * COEFF_OF_10);
         // Save hundreds and tens in memory
         memory[indexReg] = hundreds;
         memory[indexReg + COEFF_OF_1] = tens;
         memory[indexReg + COEFF_OF_2] = n;
         break;
      }
         // 0X0055 - Stores V0 to VX (including VX) in memory starting at address I.
      case V0_TO_VX: {
         for (int i = 0; i <= X; i++) {
            memory[indexReg + i] = V[i];
         }
         break;
      }
         // Fills V0 to VX (including VX) with values from memory starting at address I.
      case FILL_V0_TO_VX: {
         for (int i = 0; i <= X; i++) {
            V[i] = memory[indexReg + i];
         }
         indexReg += (X + COEFF_OF_1);
         break;
      }
      default: {
         rtn = false;
         log("value is invalid!", oc);
         break;
      }
   }

   // Program counter is always incremented by 2 bytes after each opcode execution
   progCounter += COEFF_OF_2;
   return rtn;
}

/*
 * Extract second nibble
 */
int Chip8::extractSecNibble(unsigned short oc) {
   /*
    * To extract X, bitwise AND 0x0X07 with 0x0F00, shift 8 bits to the right
    */
   int x = oc & MASK_0F00;
   int x_shifted = x >> COEFF_OF_8;
   return x_shifted;
}

/*
 * Extract Third nibble
 */
int Chip8::extractThirdNibble(unsigned short oc) {
   /*
    * To extract Y, bitwise AND 0x00XY with 0x00F0, shift 4 bits to the right
    */
   int x = oc & MASK_00F0;
   int x_shifted = x >> COEFF_OF_4;
   return x_shifted;
}

/*
 * Print messages to the console for debugging
 */
void Chip8::log(std::string type, unsigned short n) {
   cout << std::showbase << std::hex << type << " : " << std::uppercase << n << endl;
   //cout << type << " = " << std::uppercase << n << endl;
}
