/*
 * Chip8.h
 *
 *  Created on: Apr 29, 2017
 *      Author: fedcalderon
 */

#ifndef CHIP_INC_CHIP8_H_
#define CHIP_INC_CHIP8_H_

#define FONTSET_SIZE       80
#define MEMORY_SIZE        0x1000   // 4096
#define DISP_HOR           64
#define DISP_VER           32
#define NUM_KEYS           16
#define REG_SIZE           16
#define STACK_SIZE         16

#define APP_START_ADDR     0x200    // 512
#define INDEX_OF_0         0
#define INDEX_OF_1         1
#define INDEX_OF_2         2

#define READ_MODE          "r"
#define TEST_APP           "C:\\Users\\fedcalderon\\workspace\\Chip8\\Apps\\tetris.c8"

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
       * The display is 64 x 32 pixels
       */
      unsigned char display[DISP_HOR * DISP_VER];
      /*
       * Key array has 16 keys, 0 - F
       */
      unsigned char keys[NUM_KEYS];
      /*
       * The need to redraw flag
       */
      bool needRedraw;

      /*
       * Initialization method
       */
      void initializeChip8();
      /*
       * Load application program
       */
      bool loadApp(const char*);
      /*
       * Run method
       */
      void runEmulator();

   private:
      /*
       * Memory is 4KB
       * Internal use: 0x0000 - 0x0200
       * Fontset: 0x50
       * Programs: 0x200
       */
      char memory[MEMORY_SIZE];
      /*
       * There are 16 8-bit registers named from V0 - VF.
       * Reg 0xF used for carry, borrow and collision detection.
       */
      char V[REG_SIZE];
      /*
       * There are 16 levels of nesting for the subroutine callstack
       */
      int stack[STACK_SIZE];
      /*
       * Program counter is 16 bit (12 are used).
       * PC points to the current operation.
       */
      int pc;
      /*
       * The index register, 16-bit register for memory address.
       */
      int I;
      /*
       * The Opcode. There are 35 opcodes which are 2 bytes long and stored big-endian.
       */
      int opcode;
      /*
       * The delay timer
       */
      int delay_timer;
      /*
       * The sound timer
       */
      int sound_timer;
      /*
       * The Stack Pointer
       */
      int stack_pointer;
};

#endif /* CHIP_INC_CHIP8_H_ */
