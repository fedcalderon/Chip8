/*
 * Chip8.h
 *
 *  Created on: Apr 29, 2017
 *      Author: fedcalderon
 */

#ifndef CHIP_INC_CHIP8_H_
#define CHIP_INC_CHIP8_H_

#include <iostream>
#include <string>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iomanip>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_ttf.h>

// Numeric constants
#define FONTSET_SIZE         80
#define MEMORY_SIZE          0x1000   // 4096
#define DISP_HOR             640
#define DISP_VER             320
#define NUM_KEYS             16
#define REG_SIZE             16
#define STACK_SIZE           16
#define APP_START_ADDR       0x200    // 512
#define OVERFLOW_LIMIT       0xFFF
#define CARRY_LIMIT          0x100

#define COEFF_OF_0           0x0000
#define COEFF_OF_1           0x0001
#define COEFF_OF_2           0x0002
#define COEFF_OF_3           0x0003
#define COEFF_OF_4           0x0004
#define COEFF_OF_5           0x0005
#define COEFF_OF_6           0x0006
#define COEFF_OF_7           0x0007
#define COEFF_OF_8           0x0008
#define COEFF_OF_E           0x000E
#define COEFF_OF_10          0x000A
#define COEFF_OF_20          0x0014
#define COEFF_OF_64          0x0040
#define COEFF_OF_100         0x0064

#define ADDR_F               0xF

#define OPCODE_OUTER_MASK    0xF000
#define OPCODE_INNER_MASK    0xF000

// Outer opcode types
#define TYPE_0               0x0000
#define TYPE_1               0x1000
#define TYPE_2               0x2000
#define TYPE_3               0x3000
#define TYPE_4               0x4000
#define TYPE_5               0x5000
#define TYPE_6               0x6000
#define TYPE_7               0x7000
#define TYPE_8               0x8000
#define TYPE_9               0x9000
#define TYPE_A               0xA000
#define TYPE_B               0xB000
#define TYPE_C               0xC000
#define TYPE_D               0xD000
#define TYPE_E               0xE000
#define TYPE_F               0xF000

// Inner opcode types
#define MASK_0FFF            0x0FFF
#define MASK_00FF            0x00FF
#define MASK_00F0            0x00F0
#define MASK_000F            0x000F
#define MASK_0F00            0x0F00
#define MASK_F000            0xF000
#define MASK_FF              0xFF
#define MASK_F               0xF
#define MASK_80              0x80
#define MASK_1               0x1
#define MASK_7               0x7

#define CLEAR_SCREEN         0x00E0
#define RTN_SUBROUTINE       0x00EE
#define TYPE_TIMER           0x0007
#define KEY_OP               0x000A
#define IF_KEY_PRESSED       0x009E
#define IF_KEY_NOT_PRESSED   0x00A1
#define AWAITED_KEY_PRESSED  0x000A
#define DELAY_TIMER_TO_VX    0x0015
#define SOUND_TIMER_TO_VX    0X0018
#define ADD_VX_TO_I          0X001E
#define SPRITE_LOCATION      0X0029
#define BIN_CODED_DEC        0X0033
#define V0_TO_VX             0X0055
#define FILL_V0_TO_VX        0X0065

#define SCREEN_BPP           0x20

// Character constants
#define READ_MODE            "rb"

// String constants
#define PATH                 "/home/student/workspace/Chip8-master/"
#define TEST_APP             "/home/student/workspace/Chip8-master/Apps/pong2.c8"
#define SOUND_PATH           "/home/student/workspace/Chip8-master/beep.wav"


class Chip8 {
   public:
      /*
       * Constructor
       */
      Chip8();
      /*
       * Destructor
       */
      ~Chip8();

      void start();

   private:

      int speed;
      /*
       * The display is 64 x 32 pixels
       */
      unsigned char display[(DISP_HOR/10) * (DISP_VER/10)] = { 0 };
      /*
       * Key array has 16 keys, 0 - F
       */
      unsigned char keys[NUM_KEYS];
      /*
       * Memory is 4KB
       * Internal use: 0x0000 - 0x0200
       * Fontset: 0x50
       * Programs: 0x200
       */
      unsigned char memory[MEMORY_SIZE];
      /*
       * There are 16 8-bit registers named from V0 - VF.
       * Reg 0xF used for carry, borrow and collision detection.
       */
      unsigned char V[REG_SIZE];
      /*
       * There are 16 levels of nesting for the subroutine callstack
       */
      unsigned short stack[STACK_SIZE];
      /*
       * Program counter is 16 bit (12 are used).
       * PC points to the current operation.
       */
      unsigned short progCounter;
      /*
       * The index register, 16-bit register for memory address.
       */
      unsigned short indexReg;
      /*
       * The Opcode. There are 35 opcodes which are 2 bytes long and stored big-endian.
       */
      unsigned short opcode;
      /*
       * The delay timer
       */
      unsigned char delay_timer;
      /*
       * The sound timer
       */
      unsigned char sound_timer;
      /*
       * The Stack Pointer
       */
      unsigned short stack_pointer;
      /*
       * Initialization method
       */
      void initialize(const char *);
      /*
       * Run method
       */
      void runDisassembler();
      /*
       * Print to the console hex values for debugging
       */
      void log(std::string , unsigned short);
      /*
       * Process type 0 opcodes
       */
      bool processType0(unsigned short);
      /*
       * Process type 8 opcodes
       */
      bool processType8(unsigned short, int, int);
      /*
       * Process type D opcodes
       */
      bool processTypeD(unsigned short);
      /*
       * Process type E opcodes
       */
      bool processTypeE(unsigned short, int);
      /*
       * Process type F opcodes
       */
      bool processTypeF(unsigned short, int);
      /*
       * Load application program
       */
      void loadApp(const char* fileName);
      /*
       * Run display method
       */
      void drawSprites();
      /*
       * Extract second nibble
       */
      int extractSecondNibble(unsigned short);
      /*
       * Extract third nibble
       */
      int extractThirdNibble(unsigned short);
      char *readFile(const char *fileName);
      void gameSetup(const char *fileName);
      void scanKeyboard(SDL_Event *);
      void configureTimers();
      void soundBeep();
      void my_audio_callback(Uint8 *, int);
      void processType0();
      void processType8(unsigned short X, unsigned short Y);
      void processTypeD(unsigned short X, unsigned short Y);
      void processTypeE(unsigned short ETypes, unsigned short X, Uint8* keys);
      void processTypeF(unsigned short FTypes, unsigned short X, Uint8* keys);
};

#endif /* CHIP_INC_CHIP8_H_ */
