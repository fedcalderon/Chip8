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
{
   SDL_Quit();
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

/*
 * SDL key map
 */
static int keymap[0x10] = {
      SDLK_0,
      SDLK_1,
      SDLK_2,
      SDLK_3,
      SDLK_4,
      SDLK_5,
      SDLK_6,
      SDLK_7,
      SDLK_8,
      SDLK_9,
      SDLK_a,
      SDLK_b,
      SDLK_c,
      SDLK_d,
      SDLK_e,
      SDLK_f
};

/*
 * Public start entry method
 */
void Chip8::start()
{
   int gameSelect = 0;
   printf("Welcome to the Chip-8 emulator game console. \n1. Tetris \n2. Invaders \n3. Pong \n4. Exit\nType a number and press Enter to select the game: ");
   scanf("%d", &gameSelect);

   switch(gameSelect)
   {
      case COEFF_OF_1:
      {
         printf("\n**********************************\n");
         printf("Let's play TETRIS!\nKey map:\n4. Rotate\n5. Left\n6. Right\n");
         printf("**********************************\n\n");
         speed = 5;
         initialize("tetris.c8");
         break;
      }
      case COEFF_OF_2:
      {
         printf("\n**********************************\n");
         printf("Let's play INVADERS!\nKey map:\n5. Fire\n4. Left\n6. Right\n");
         printf("**********************************\n\n");
         speed = 10;
         initialize("invaders.c8");
         break;
      }
      case COEFF_OF_3:
      {
         printf("\n**********************************\n");
         printf("Let's play PONG!\nKey map:\n1. UP\n4. DOWN\n");
         printf("**********************************\n\n");
         speed = 10;
         initialize("pong2.c8");
         break;
      }
      case COEFF_OF_4:
      {
         printf("\n**********************************\n");
         printf("  Thanks for playing! See ya!\n");
         printf("**********************************\n\n");
         break;
      }
      default:
      {
         printf("\n**********************************\n");
         printf("  Wrong selection! Try again!\n");
         printf("**********************************\n\n");
         break;
      }
   }

   exit(1);
}

/*
 * Initialization function
 */
void Chip8::initialize(const char * fileName)
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
   // Initialize SDL
   if (SDL_Init(SDL_INIT_EVERYTHING) < COEFF_OF_0)
   {
      cout << "SDL failed to initialize!" << endl;
      SDL_Quit();
   }
   // Setup video window
   SDL_WM_SetCaption("CHIP-8 EMULATOR | Keys: ESC (exit), P (pause), U (unpause), C (reset), R (restart)", NULL);
   SDL_SetVideoMode(DISP_HOR, DISP_VER, SCREEN_BPP, SDL_HWSURFACE | SDL_DOUBLEBUF);
   // Setup SDL event
   SDL_Event event;
   // Game loop
   while (true)
   {
      if (SDL_PollEvent(&event))
      {
         continue;
      }
      runDisassembler();
      drawSprites();
      scanKeyboard(&event);
   }
}

/*
 *
 */
void Chip8::scanKeyboard(SDL_Event * event)
{
   Uint8 * keys = SDL_GetKeyState(NULL);
   if (keys[SDLK_ESCAPE])
   {
      exit(1);
   }
   if (keys[SDLK_r])
   {
      start();
   }
   if (keys[SDLK_c])
   {
      start();
   }
   if (keys[SDLK_p])
   {
      while (COEFF_OF_1)
      {
         if (SDL_PollEvent(event))
         {
            keys = SDL_GetKeyState(NULL);
            if (keys[SDLK_ESCAPE]) // exit
            {
               exit(COEFF_OF_1);
            }
            if (keys[SDLK_u])
            {
               break;
            }
         }
      }
   }
}

/*
 * This method keeps track of the timers
 */
void Chip8::configureTimers()
{
   if (delay_timer > COEFF_OF_0)
   {
      delay_timer--;
   }
   if (sound_timer > COEFF_OF_0)
   {
      sound_timer--;
   }
   if (sound_timer != COEFF_OF_0)
   {
      cout << "beep" << endl;
   }
}

/*
 * Load application
 */
void Chip8::loadApp(const char *file)
{

   // Open file, check for errors
   FILE *pApp = fopen(file, READ_MODE);
   if (pApp == NULL)
   {
      cout << "Error opening \n" << file << " ... " << endl;
      exit(COEFF_OF_1);
   }
   cout << "Loading " << file << " ... " << endl;
   // Compute file size
   fseek(pApp, COEFF_OF_0, SEEK_END);
   size_t fileSize = ftell(pApp);
   rewind(pApp);

   /********************************  For debugging, uncomment ********************************************/
//   printf("File size: %d\n", (int)fileSize);


   // Allocate memory for the application, check for errors
   char *buff = (char*) malloc(sizeof(char) * (fileSize + 1));
   if (buff == NULL)
   {
      cout << "Error loading app into memory ... " << endl;
      exit(COEFF_OF_1);
   }
   // Copy the file into the buffer
   size_t result = fread(buff, 1, fileSize, pApp);
   if (result != fileSize)
   {
      cout << "there seems to be a memory error ... " << endl;
      exit(COEFF_OF_1);
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
      exit(COEFF_OF_1);
   }
   // Close file
   fclose(pApp);
   // Free buff
   free(buff);
}

/*
 * Run display method
 */
void Chip8::drawSprites()
{
   SDL_Surface * surface = SDL_GetVideoSurface();
   SDL_LockSurface(surface);
   Uint32 * screen = (Uint32 *) surface->pixels;
   memset(screen, COEFF_OF_0, surface->w * surface->h * sizeof(Uint32));

   for (int yCoord = COEFF_OF_0; yCoord < DISP_VER; yCoord++)
   {
      for (int xCoord = COEFF_OF_0; xCoord < DISP_HOR; xCoord++)
      {
         screen[xCoord + yCoord * surface->w] = display[(xCoord / COEFF_OF_10) + (yCoord / COEFF_OF_10) * COEFF_OF_64] ? 0xFFFFFFFF : COEFF_OF_0;
      }
   }
   SDL_UnlockSurface(surface);
   SDL_Flip(surface);
   SDL_Delay(30);
}

/*
 * Process opcodes type 0
 */
void Chip8::processType0()
{
   // 0x0000
   unsigned short opcode_internal_type = opcode & MASK_FF;
   switch (opcode_internal_type)
   {
      case CLEAR_SCREEN: // 00E0: Clears the screen
      {
         memset(display, COEFF_OF_0, sizeof(display));
         progCounter += COEFF_OF_2;
         break;
      }
      case RTN_SUBROUTINE: // 00EE: Returns from a subroutine.
      {
         stack_pointer--;
         progCounter = stack[stack_pointer] + COEFF_OF_2;
         break;
      }
      default:
      {
         break;
      }
   }
}

/*
 * Process opcodes type 8
 */
void Chip8::processType8(unsigned short X, unsigned short Y)
{
   // Inner switch over last nibble to process internal options.
   unsigned short Type8 = opcode & MASK_000F;
   switch (Type8)
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
         V[ADDR_F] = ((V[X] + V[Y]) < CARRY_LIMIT) ? COEFF_OF_0 : COEFF_OF_1;
         V[X] += V[Y];
         progCounter += COEFF_OF_2;
         break;
      }
         // 8XY5: VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
      case COEFF_OF_5:
      {
         V[ADDR_F] = ((V[X] - V[Y]) >= COEFF_OF_0) ? COEFF_OF_1 : COEFF_OF_0;
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
         V[ADDR_F] = ((V[X] - V[Y]) > 0) ? COEFF_OF_1 : COEFF_OF_0;
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
}

/*
 * Process opcode type D
 */
void Chip8::processTypeD(unsigned short X, unsigned short Y)
{
   int VX = V[X];
   int VY = V[Y];
   unsigned short N = opcode & MASK_000F;
   V[ADDR_F] = COEFF_OF_0;
   // Outer loop controls y-coordinate from 0 to height N
   for (int yCoord = COEFF_OF_0; yCoord < N; yCoord++)
   {
      int line = memory[indexReg + yCoord];
      // Inner loop controls x-coordinate from 0 to 7, 8 pixels wide
      for (int xCoord = 0; xCoord < COEFF_OF_8; xCoord++)
      {
         if (line & (MASK_80 >> xCoord))
         {
            int totalDrawX = (VX + xCoord);
            int totalDrawY = (VY + yCoord);
            int index = (totalDrawY * COEFF_OF_64) + totalDrawX;
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
}

/*
 * Process opcodes type E
 */
void Chip8::processTypeE(unsigned short ETypes, unsigned short X, Uint8* keys)
{
   switch (ETypes)
   {
      // Skips the next instruction if the key stored in VX is pressed.
      // (Usually the next instruction is a jump to skip a code block)
      case IF_KEY_PRESSED:
      {
         // 0x009E
         keys = SDL_GetKeyState(NULL);
         progCounter += (keys[keymap[V[X]]] == COEFF_OF_1) ? COEFF_OF_4 : COEFF_OF_2;
         break;
      }
         // Skips the next instruction if the key stored in VX isn't pressed.
         // (Usually the next instruction is a jump to skip a code block)
      case IF_KEY_NOT_PRESSED:
      {
         // 0x00A1
         keys = SDL_GetKeyState(NULL);
         progCounter += (keys[keymap[V[X]]] == COEFF_OF_0) ? COEFF_OF_4 : COEFF_OF_2;
         break;
      }
      default:
      {
         printf("E type opcode value [%04X] is invalid!", ETypes);
         break;
      }
   }
}

/*
 * Process opcodes type F
 */
void Chip8::processTypeF(unsigned short FTypes, unsigned short X, Uint8* keys)
{
   switch (FTypes)
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
         for (int i = COEFF_OF_0; i < NUM_KEYS; i++)
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
      {
         // 0X0029
         indexReg = V[X] * COEFF_OF_5;
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
      {
         //0X0033
         memory[indexReg] = V[X] / COEFF_OF_100;
         memory[indexReg + COEFF_OF_1] = (V[X] / COEFF_OF_10) % COEFF_OF_10;
         memory[indexReg + COEFF_OF_2] = V[X] % COEFF_OF_10;
         progCounter += COEFF_OF_2;
         break;
      }
         // 0X0055 - Stores V0 to VX (including VX) in memory starting at address I.
      case V0_TO_VX:
      {
         for (int i = COEFF_OF_0; i <= X; i++)
         {
            memory[indexReg + i] = V[i];
         }
         progCounter += COEFF_OF_2;
         break;
      }
         // Fills V0 to VX (including VX) with values from memory starting at address I.
      case FILL_V0_TO_VX:
      {
         for (int i = COEFF_OF_0; i <= X; i++)
         {
            V[i] = memory[indexReg + i];
         }
         progCounter += COEFF_OF_2;
         break;
      }
      default:
      {
         printf("F type opcode value [%04X] is invalid!", FTypes);
         break;
      }
   }
}

/*
 * Execute chip 8 cpu instructions
 */
void Chip8::runDisassembler()
{
   Uint8 * keys = NULL;

   // The limit tunes how fast the game moves. The lower the number, the slower it runs.
   for (int i = COEFF_OF_0; i < speed; i++)
   {

      /*
       * Get the opcode. Opcode is 2 bytes long, therefore can obtain as follows:
       *
       * 1. Read first byte from memory[pc]
       * 2. Shift 8 bits to the left
       * 3. Read contiguous byte from memory[pc+1]
       * 4. Bitwise OR both values to form opcode
       */
      unsigned short first_byte = memory[progCounter] << COEFF_OF_8;
      unsigned short second_byte = memory[progCounter + COEFF_OF_1];
      opcode = first_byte | second_byte;

      /********************************  For debugging, uncomment ********************************************/
//      printf ("Opcode: %04X | PC: %04X | I:%02X | SP:%02X\n", opcode, progCounter, indexReg, stack_pointer);
      /*******************************************************************************************************/

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

      // Calculate X, Y, and NN
      unsigned short X = extractSecondNibble(opcode);
      unsigned short Y = extractThirdNibble(opcode);
      unsigned short NN = opcode & MASK_00FF;


      // Outer switch to control the types of opcodes
      switch (opcode_type)
      {

         // Display and flow
         case TYPE_0: // 0x0000
         {
            processType0();
            break;
         }
         // Jumps to address NNN.
         case TYPE_1: // 0x1000
         {
            int NNN = opcode & MASK_0FFF;
            progCounter = NNN;
            break;
         }
         // Calls subroutine at NNN.
         case TYPE_2: // 0x2000
         {
            stack[(stack_pointer++) & MASK_F] = progCounter;
            progCounter = opcode & MASK_0FFF;
            break;
         }
         // Skips the next instruction if VX equals NN. (Usually the next instruction is a jump to skip a code block)
         case TYPE_3:
         {
            progCounter += (V[X] == NN) ? COEFF_OF_4 : COEFF_OF_2;
            break;
         }
         // Skips the next instruction if VX doesn't equal NN. (Usually the next instruction is a jump to skip a code block)
         case TYPE_4:
         {
            progCounter += (V[X] != NN) ? COEFF_OF_4 : COEFF_OF_2;
            break;
         }
         // Skips the next instruction if VX equals VY. (Usually the next instruction is a jump to skip a code block)
         case TYPE_5: // 5XY0
         {
            progCounter += (V[X] == NN) ? COEFF_OF_4 : COEFF_OF_2;
            break;
         }
         // 6XNN - Sets VX to NN.
         case TYPE_6:
         {
            V[X] = NN;
            progCounter += COEFF_OF_2;
            break;
         }
         // 7XNN - Adds NN to VX.
         case TYPE_7:
         {
            V[X] += NN;
            progCounter += COEFF_OF_2;
            break;
         }
         // Math operations
         case TYPE_8:
         {
            // Inner switch over last nibble to process internal options.
            processType8(X, Y);
            break;
         }
         // Skips the next instruction if VX doesn't equal VY. (Usually the next instruction is a jump to skip a code block)
         case TYPE_9: // 9XY0
         {
            progCounter += (V[X] != V[Y]) ? COEFF_OF_4 : COEFF_OF_2;
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
         case TYPE_D: // DXYN
         {
            processTypeD(X, Y);
            break;
         }
         case TYPE_E:
         {
            unsigned short ETypes = opcode & MASK_00FF;
            processTypeE(ETypes, X, keys);
            break;
         }
         /*
          * Timer, KeyOp, sound, mem, bcd
          */
         case TYPE_F:
         {
            unsigned short FTypes = opcode & MASK_00FF;
            processTypeF(FTypes, X, keys);
            break;
         }

         default:
         {
            printf("opcode value [%04X] is invalid!", opcode);
            break;
         }
      }
      configureTimers();
   }
}


/*
 * Extract second nibble. To extract X, bitwise AND 0x0X07 with 0x0F00, shift 8 bits to the right
 */
int Chip8::extractSecondNibble(unsigned short oc)
{
   return (oc & MASK_0F00) >> COEFF_OF_8;
}

/*
 * Extract Third nibble. To extract Y, bitwise AND 0x00XY with 0x00F0, shift 4 bits to the right
 */
int Chip8::extractThirdNibble(unsigned short oc)
{
   return (oc & MASK_00F0) >> COEFF_OF_4;
}
