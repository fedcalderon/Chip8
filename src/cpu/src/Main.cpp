/*
 * Main.cpp
 *
 *  Created on: Apr 29, 2017
 *      Author: fedcalderon
 */


#include "../../cpu/inc/Chip8.h"
#include <iostream>
#include <stdio.h>

using namespace std;

Chip8 chip8;


int main(int argc, char **argv){
   cout << "Entering Chip8 main() ... " << endl;

   int rtn = INDEX_OF_0;

   if(argc < INDEX_OF_2){
      cout << "Specify the name of the application you want to load. For ex, >Chip8.exe tetris.c8" << endl;
   }

   // Load the app
   chip8.loadApp(TEST_APP);
//   if(!chip8.loadApp(argv[INDEX_OF_1])){
//      cout << "Error opening the application!" << endl;
//      rtn = INDEX_OF_1;
//   }

   return rtn;
}

