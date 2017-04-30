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


int main(){
   cout << "Entering main() ... " << endl;

   chip8.initialize();

   return 0;
}

