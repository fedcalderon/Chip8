//============================================================================
// Name        : Chip8.cpp
// Author      : Federico Calderon
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
// Opcode description taken from https://en.wikipedia.org/wiki/CHIP-8
//============================================================================

#include "../../cpu/inc/Chip8.h"

using namespace std;

Chip8::Chip8()
{ /* Emtpy */
}
Chip8::~Chip8()
{ /* Emtpy */
}

/*
 * Fontset definition. Memory position must be at 0x50 (80 decimal)
 */
unsigned char fontset[FONTSET_SIZE] = {
      0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
      0x20, 0x60, 0x20, 0x20, 0x70, // 1
      0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
      0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
      0x90, 0x90, 0xF0, 0x10, 0x10, // 4
      0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
      0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
      0xF0, 0x10, 0x20, 0x40, 0x40, // 7
      0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
      0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
      0xF0, 0x90, 0xF0, 0x90, 0x90, // A
      0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
      0xF0, 0x80, 0x80, 0x80, 0xF0, // C
      0xE0, 0x90, 0x90, 0x90, 0xE0, // D
      0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
      0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

static int keymap[0x10] =
{ SDLK_0, SDLK_1, SDLK_2, SDLK_3, SDLK_4, SDLK_5, SDLK_6, SDLK_7, SDLK_8, SDLK_9, SDLK_a, SDLK_b, SDLK_c, SDLK_d,
      SDLK_e, SDLK_f };

void Chip8::start()
{
   // TODO: Give user a choice to select a game to play
   gameSetup(TEST_APP); // prepare
}

/*
 * Prepare
 */
void Chip8::gameSetup(const char *fileName)
{
   // Initialize
   initializeChip8(fileName);

   if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
   {
      cout << "SDL failed to initialize!" << endl;
      SDL_Quit();
   }
   SDL_WM_SetCaption("FED's CHIP-8 EMULATOR", NULL);
   SDL_SetVideoMode(DISP_HOR, DISP_VER, SCREEN_BPP, SDL_HWSURFACE | SDL_DOUBLEBUF);

   int close_requested = 0;

   while (!close_requested)
   {
      SDL_Event event;

      while (SDL_PollEvent(&event))
      {

         switch (event.type)
         {
            case SDL_QUIT:
            {
               close_requested = 1;
               break;
            }
            default:
            {
               runEmulator();
               drawSprites();
               chip8_prec(&event);
               break;
            }
         }
      }
   }
}

/*
 * Initialization function
 */
void Chip8::initializeChip8(const char * fileName)
{
   cout << "Initialize() ... " << endl;

   progCounter = APP_START_ADDR;
   opcode = APP_START_ADDR;
   indexReg = COEFF_OF_0;
   stack_pointer &= COEFF_OF_0;
   delay_timer = COEFF_OF_0;
   sound_timer = COEFF_OF_0;

   // Load fontset to memory
   for (int i = 0; i < FONTSET_SIZE; i++)
   {
      memory[i] = fontset[i];
   }

   // Load the game
   loadApp(fileName);

   // Clear display
   memset(display, 0, sizeof(display));
   // Clear stack
   memset(stack, 0, sizeof(stack));
   // Clear V registers
   memset(V, 0, sizeof(V));
}

void Chip8::chip8_prec(SDL_Event * event)
{
   Uint8 * keys = SDL_GetKeyState(NULL);
   if (keys[SDLK_ESCAPE])
      exit(1);
//   if (keys[SDLK_r])
//      gameSetup(name);
   if (keys[SDLK_c])
      start();
   if (keys[SDLK_p])
   {
      while (1)
      {
         if (SDL_PollEvent(event))
         {
            keys = SDL_GetKeyState(NULL);
            if (keys[SDLK_ESCAPE])
               exit(1);
            if (keys[SDLK_u])
               break;
         }
      }
   }
}

void Chip8::chip8_timers()
{
   if (delay_timer > 0)
      delay_timer--;
   if (sound_timer > 0)
      sound_timer--;
   if (sound_timer != 0)
      printf("%c", 7);
}

/*
 * Load application program
 */
bool Chip8::loadApp(const char *file)
{

   // Open file, check for errors
   FILE *pApp = fopen(file, READ_MODE);
   if (pApp == NULL)
   {
      cout << "Error opening \n" << file << " ... " << endl;
      return false;
   }
   cout << "Loading " << file << " ... " << endl;
   // Compute file size
   fseek(pApp, COEFF_OF_0, SEEK_END);
   size_t fileSize = ftell(pApp);
   rewind(pApp);
   printf("Filesize: %d\n", fileSize);
   // Allocate memory for the application, check for errors
   char *buff = (char*) malloc(sizeof(char) * (fileSize + 1));
   if (buff == NULL)
   {
      cout << "Error loading app into memory ... " << endl;
      return false;
   }
   // Copy the file into the buffer
   size_t result = fread(buff, 1, fileSize, pApp);
   if (result != fileSize)
   {
      cout << "there seems to be a memory error ... " << endl;
      return false;
   }

   // Load app into virtual chip-8 memory, check if app size fits into app memory
   size_t allocated_space = MEMORY_SIZE - APP_START_ADDR;
   if (fileSize < allocated_space)
   {
      for (size_t i = 0; i < fileSize; i++)
      {
         memory[APP_START_ADDR + i] = (char) buff[i];
      }
   }
   else
   {
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
void Chip8::drawSprites()
{
   int i, j;
   SDL_Surface * surface = SDL_GetVideoSurface();
   SDL_LockSurface(surface);
   Uint32 * screen = (Uint32 *) surface->pixels;
   memset(screen, 0, surface->w * surface->h * sizeof(Uint32));

   for (i = 0; i < DISP_VER; i++)
      for (j = 0; j < DISP_HOR; j++)
      {
         screen[j + i * surface->w] = display[(j / 10) + (i / 10) * 64] ? 0xFFFFFFFF : 0;
      }

   SDL_UnlockSurface(surface);
   SDL_Flip(surface);
   SDL_Delay(15);
}

/*
 * Execute chip 8 cpu instructions
 */
void Chip8::runEmulator()
{
   Uint8 * keys;

   for (int i = 0; i < COEFF_OF_10; i++)
   {

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
      switch (opcode_type)
      {

         // Display and flow
         case TYPE_0:
         {  // 0x0000
            unsigned short opcode_internal_type = opcode & MASK_FF;
            log("opcode_internal_type", opcode_internal_type);
            switch (opcode_internal_type)
            {
               case CLEAR_SCREEN:
               {   // 00E0: Clears the screen
                  log("Clearing the screen", opcode_internal_type);
                  memset(display, COEFF_OF_0, sizeof(display));
                  // Advance program counter by 2 bytes
                  progCounter += COEFF_OF_2;
                  break;
               }
               case RTN_SUBROUTINE:
               { // 00EE: Returns from a subroutine.
                  stack_pointer--;
                  progCounter = stack[stack_pointer] + COEFF_OF_2;
                  break;
               }
               default:
               {
                  log("value is invalid!", opcode_internal_type);
                  break;
               }
            }
            break;
         }
            // Jumps to address NNN.
         case TYPE_1:
         { // 0x1000
           // Get address by bitwise AND opcode with 0x0FFF
            int address = opcode & MASK_0FFF;
            log("Set program counter to address", address);
            // Set program counter to address
            progCounter = address;
            break;
         }
            // Calls subroutine at NNN.
         case TYPE_2:
         { // 0x2000
            stack[(stack_pointer++) & MASK_F] = progCounter;
            progCounter = opcode & MASK_0FFF;
            log("Set program counter to address", progCounter);
            break;
         }
         // Skips the next instruction if VX equals NN. (Usually the next instruction is a jump to skip a code block)
         case TYPE_3:
         {
            unsigned short X = extractSecNibble(opcode);
            unsigned short NN = opcode & MASK_00FF;
            if (V[X] == NN)
            {
               progCounter += COEFF_OF_4;
            }
            else
            {
               progCounter += COEFF_OF_2;
            }
//            log("X", X);
//            log("NN", NN);
            log("Program counter", progCounter);
            break;
         }
         // Skips the next instruction if VX doesn't equal NN. (Usually the next instruction is a jump to skip a code block)
         case TYPE_4:
         {
            unsigned short X = extractSecNibble(opcode);
            unsigned short NN = opcode & MASK_00FF;
            if (V[X] != NN)
            {
               progCounter += COEFF_OF_4;
            }
            else
            {
               progCounter += COEFF_OF_2;
            }
//            log("X", X);
//            log("NN", NN);
            log("Program counter", progCounter);
            break;
         }
         // Skips the next instruction if VX equals VY. (Usually the next instruction is a jump to skip a code block)
         case TYPE_5:
         { // 5XY0
            unsigned short X = extractSecNibble(opcode);
            unsigned short Y = extractThirdNibble(opcode);
            if (V[X] == V[Y])
            {
               progCounter += COEFF_OF_4;
            }
            else
            {
               progCounter += COEFF_OF_2;
            }
//            log("X", X);
//            log("Y", Y);
            log("Program counter", progCounter);
            break;
         }
            // 6XNN - Sets VX to NN.
         case TYPE_6:
         {
            unsigned short X = extractSecNibble(opcode);
            unsigned short NN = opcode & MASK_00FF;
            V[X] = NN;
            progCounter += COEFF_OF_2;

//            log("X", X);
//            log("NN", NN);
            log("Program counter", progCounter);
            break;
         }
            // 7XNN - Adds NN to VX.
         case TYPE_7:
         {
            unsigned short X = extractSecNibble(opcode);
            unsigned short NN = opcode & MASK_00FF;
            V[X] += NN;
            progCounter += COEFF_OF_2;

//            log("X", X);
//            log("NN", NN);
//            log("V[X]", V[X]);
            log("Program counter", progCounter);
            break;
         }
            // Math operations
         case TYPE_8:
         {
            // Inner switch over last nibble to process internal options.
            unsigned short oc = opcode & MASK_000F;
            unsigned short X = extractSecNibble(opcode);
            unsigned short Y = extractThirdNibble(opcode);

            switch (oc)
            {
               // 8XY0: sets VX to the value of VY
               case COEFF_OF_0:
               {
                  V[X] = V[Y];
                  progCounter += COEFF_OF_2;
                  break;
               }
               // 8XY1: Sets VX to VX or VY. (Bitwise OR operation) VF is reset to 0.
               case COEFF_OF_1:
               {
                  V[X] = V[X] | V[Y];
                  progCounter += COEFF_OF_2;
                  break;
               }
               // 8XY2: Sets VX to VX and VY. (Bitwise AND operation) VF is reset to 0.
               case COEFF_OF_2:
               {
                  V[X] = V[X] & V[Y];
                  progCounter += COEFF_OF_2;
                  break;
               }
               // 8XY3: Sets VX to VX xor VY. VF is reset to 0.
               case COEFF_OF_3:
               {
                  V[X] = V[X] ^ V[Y];
                  progCounter += COEFF_OF_2;
                  break;
               }
               // 8XY4: Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't.
               case COEFF_OF_4:
               {

                  if ((V[X] + V[Y]) < CARRY_LIMIT)
                  {
                     V[ADDR_F] &= COEFF_OF_0; // carry
                  }
                  else
                  {
                     V[ADDR_F] = COEFF_OF_1; // no carry
                  }
                  V[X] += V[Y];
                  progCounter += COEFF_OF_2;
                  break;
               }
               // 8XY5: VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
               case COEFF_OF_5:
               {

                  if ((V[X] - V[Y]) >= COEFF_OF_0)
                  {
                     V[ADDR_F] = COEFF_OF_1; // no borrow
                  }
                  else
                  {
                     V[ADDR_F] &= COEFF_OF_0; // borrow
                  }
                  V[X] -= V[Y];
                  progCounter += COEFF_OF_2;
                  break;
               }
               // 8XY6: Shifts VX right by one. VF is set to the value of the least significant bit of VX before the shift.
               case COEFF_OF_6:
               {
                  V[ADDR_F] = V[X] & MASK_7;
                  V[X] = V[X] >> COEFF_OF_1;
                  progCounter += COEFF_OF_2;
                  break;
               }
               // 8XY7: Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
               case COEFF_OF_7:
               {

                  if ((V[X] - V[Y]) > 0)
                  {
                     V[ADDR_F] = COEFF_OF_1; // no borrow
                  }
                  else
                  {
                     V[ADDR_F] &= COEFF_OF_0; // borrow
                  }
                  V[X] = V[Y] - V[X];
                  progCounter += COEFF_OF_2;
                  break;
               }
               // 8XYE: Shifts VX left by one. VF is set to the value of the most significant bit of VX before the shift.
               case COEFF_OF_E:
               {
                  V[ADDR_F] = V[X] >> MASK_7;
                  V[X] = V[X] << COEFF_OF_1;
                  progCounter += COEFF_OF_2;
                  break;
               }
               default:
               {
                  break;
               }
            }
            break;
         }
         // Skips the next instruction if VX doesn't equal VY. (Usually the next instruction is a jump to skip a code block)
         case TYPE_9:
         { // 9XY0
            unsigned short X = extractSecNibble(opcode);
            unsigned short Y = extractThirdNibble(opcode);
            if (V[X] != V[Y])
            {
               progCounter += COEFF_OF_4;
            }
            else
            {
               progCounter += COEFF_OF_2;
            }
            break;
         }
         // ANNN - Sets I to the address NNN.
         case TYPE_A:
         {
            unsigned short NNN = opcode & MASK_0FFF;
            indexReg = NNN;
            progCounter += COEFF_OF_2;
            break;
         }
         // BNNN - Jumps to the address NNN plus V0.
         case TYPE_B:
         {
            unsigned short NNN = opcode & MASK_0FFF;
            progCounter = NNN + V[0];
            break;
         }
         // CXNN - Sets VX to the result of a bitwise and operation on a random number (Typically: 0 to 255) and NN.
         case TYPE_C:
         {
            unsigned short X = extractSecNibble(opcode);
            unsigned short NN = opcode & MASK_00FF;
            unsigned short random = rand();// % MASK_00FF;
            V[X] = random & NN;
            progCounter += COEFF_OF_2;
            break;
         }
         /*
          * Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels.
          * Each row of 8 pixels is read as bit-coded starting from memory location I;
          * I value doesn�t change after the execution of this instruction.
          * As described above, VF is set to 1 if any screen pixels are flipped from set to unset
          * when the sprite is drawn, and to 0 if that doesn�t happen
         */
         case TYPE_D:
         { // DXYN
            int VX = V[extractSecNibble(opcode)];
            int VY = V[extractThirdNibble(opcode)];
            unsigned short N = opcode & MASK_000F;
            V[ADDR_F] = COEFF_OF_0;

            // Outer loop controls y-coordinate from 0 to height N
            for (int yCoord = 0; yCoord < N; yCoord++)
            {
               int line = memory[indexReg + yCoord]; // pixel

               // Inner loop controls x-coordinate from 0 to 7, 8 pixels wide
               for (int xCoord = 0; xCoord < COEFF_OF_8; xCoord++)
               {
//                  unsigned short shifting_mask = 0b10000000 >> xCoord;
//                  int pixel = line & shifting_mask;
                  if (line & (MASK_80 >> xCoord))
                  {
                     int totalDrawX = (VX + xCoord);// % DISP_HOR;
                     int totalDrawY = (VY + yCoord);// % DISP_VER;
                     int index = (totalDrawY * 64) + totalDrawX;
                     if (display[index])
                     {
                        V[ADDR_F] = COEFF_OF_1;
                     }
                     // As the shifting mask determines the value of pixel,
                     // set the new value of display[index] as bitwise exclusive OR until pixel
                     // is zero. Then, move to the next y-coordinate
                     display[index] ^= COEFF_OF_1;
                  }
               }
            }
            progCounter += COEFF_OF_2;
            break;
         }
         case TYPE_E:
         {  // KeyOp
            unsigned short oc = opcode & MASK_00FF;
            log("last_byte", oc);
            int X = extractSecNibble(opcode);

            switch (oc)
            {
               // Skips the next instruction if the key stored in VX is pressed.
               // (Usually the next instruction is a jump to skip a code block)
               case IF_KEY_PRESSED:
               { // 0x009E
                  keys = SDL_GetKeyState(NULL);
                  if (keys[keymap[V[X]]] == COEFF_OF_1)
                  {
                     progCounter += COEFF_OF_4;
                     cout << "key pressed" << endl;
                  }
                  else
                  {
                     progCounter += COEFF_OF_2;
                     cout << "key not pressed" << endl;
                  }
                  break;
               }
                  // Skips the next instruction if the key stored in VX isn't pressed.
                  // (Usually the next instruction is a jump to skip a code block)
               case IF_KEY_NOT_PRESSED:
               { // 0x00A1
                  keys = SDL_GetKeyState(NULL);
                  if (keys[keymap[V[X]]] == COEFF_OF_0)
                  {
                     progCounter += COEFF_OF_4;
                     cout << "key not pressed" << endl;
                  }
                  else
                  {
                     progCounter += COEFF_OF_2;
                     cout << "key pressed" << endl;
                  }
                  break;
               }
               default:
               {
                  break;
               }
            }
            break;
         }
            /*
             * Timer, KeyOp, sound, mem, bcd
             */
         case TYPE_F:
         {
            unsigned short oc = opcode & MASK_00FF;
            int X = extractSecNibble(opcode);

            switch (oc)
            {
               // 0xFX07 - Sets VX to the value of the delay timer.
               case TYPE_TIMER:
               {
                  V[X] = delay_timer;
                  progCounter += COEFF_OF_2;
                  break;
               }
               // 0x000A - A key press is awaited, and then stored in VX. (Blocking Operation. All instruction halted until next key event)
               case AWAITED_KEY_PRESSED:
               {
                  keys = SDL_GetKeyState(NULL);
                  cout << "key pressed" << endl;
                  for (int i = 0; i < NUM_KEYS; i++)
                  {
                     if (keys[keymap[i]] == COEFF_OF_1)
                     {
                        V[X] = i;
                        progCounter += COEFF_OF_2;
                     }
                  }
                  break;
               }
               // 0x0015 - Sets the delay timer to VX.
               case DELAY_TIMER_TO_VX:
               {
                  delay_timer = V[X];
                  progCounter += COEFF_OF_2;
                  break;
               }
               // 0X0018 - Sets the sound timer to VX.
               case SOUND_TIMER_TO_VX:
               {
                  sound_timer = V[X];
                  progCounter += COEFF_OF_2;
                  break;
               }
               // 0X001E - Adds VX to I. VF is set to 1 when there is a range overflow (I+VX>0xFFF), and to 0 when there isn't.
               // This is an undocumented feature of the CHIP-8 and used by the Spacefight 2091! game.
               case ADD_VX_TO_I:
               {
                  indexReg += V[X];
                  progCounter += COEFF_OF_2;
                  break;
               }
               // Sets I to the location of the sprite for the character in VX. Characters 0-F (in hexadecimal)
               // are represented by a 4x5 font.
               case SPRITE_LOCATION:
               {   // 0X0029
                  indexReg =V[X] * COEFF_OF_5;
                  progCounter += COEFF_OF_2;
                  break;
               }
               /*
                * Stores the binary-coded decimal representation of VX, with the most significant of three
                * digits at the address in I, the middle digit at I plus 1, and the least significant digit
                * at I plus 2. (In other words, take the decimal representation of VX, place the hundreds
                * digit in memory at location in I, the tens digit at location I+1, and the ones digit at
                * location I+2.)
               */
               case BIN_CODED_DEC:
               { //0X0033
                  memory[indexReg] = V[X] / COEFF_OF_100;
                  memory[indexReg + COEFF_OF_1] = (V[X] / COEFF_OF_10) % COEFF_OF_10;
                  memory[indexReg + COEFF_OF_2] = V[X] % COEFF_OF_10;
                  progCounter += COEFF_OF_2;
                  break;
               }
               // 0X0055 - Stores V0 to VX (including VX) in memory starting at address I.
               case V0_TO_VX:
               {
                  for (int i = 0; i <= X; i++)
                  {
                     memory[indexReg + i] = V[i];
                  }
                  progCounter += COEFF_OF_2;
                  break;
               }
               // Fills V0 to VX (including VX) with values from memory starting at address I.
               case FILL_V0_TO_VX:
               {
                  for (int i = 0; i <= X; i++)
                  {
                     V[i] = memory[indexReg + i];
                  }
                  progCounter += COEFF_OF_2;
                  break;
               }
               default:
               {
                  log("value is invalid!", oc);
                  break;
               }
            }
            break;
         }

         default:
         {
            log("value is invalid!", opcode);
            break;
         }
      }
      chip8_timers();
   }
}

/*
 * Process type F opcodes
 */
bool Chip8::processTypeF(unsigned short oc, int X)
{
   bool rtn = true;
   return rtn;
}

/*
 * Extract second nibble
 */
int Chip8::extractSecNibble(unsigned short oc)
{
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
int Chip8::extractThirdNibble(unsigned short oc)
{
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
void Chip8::log(std::string type, unsigned short n)
{
   cout << std::showbase << std::hex << type << " : " << std::uppercase << n << endl;
}
