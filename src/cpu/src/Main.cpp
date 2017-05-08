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

void testOpcodes();
void testType0();
void testType1();
void testType2();
void testType3();
void testType4();
void testType5();
void testType6();
void testType7();
void testType8();
void testType9();
void testTypeA();
void testTypeB();
void testTypeC();
void testTypeD();
void testTypeE();
void testTypeF();


int main(int argc, char **argv){
   cout << "Entering Chip8 main() ... " << endl;

   int rtn = COEFF_OF_0;

   if(argc < COEFF_OF_2){
      cout << "Specify the name of the application you want to load. For ex, >Chip8.exe tetris.c8" << endl;
   }

   // Load the app
   chip8.loadApp(TEST_APP);
   chip8.runDisplay();

//   if(!chip8.loadApp(argv[INDEX_OF_1])){
//      cout << "Error opening the application!" << endl;
//      rtn = INDEX_OF_1;
//   }

   return rtn;
}

void testOpcodes(){
   testType0();
   testType1();
   testType2();
   testType3();
   testType4();
   testType5();
   testType6();
   testType7();
   testType8();
   testType9();
   testTypeA();
   testTypeB();
   testTypeC();
   testTypeD();
   testTypeE();
   testTypeF();
}
void testType0(){

}
void testType1(){

}
void testType2(){

}
void testType3(){

}
void testType4(){

}
void testType5(){

}
void testType6(){

}
void testType7(){

}
void testType8(){

}
void testType9(){

}
void testTypeA(){

}
void testTypeB(){

}
void testTypeC(){

}
void testTypeD(){

}
void testTypeE(){

}
void testTypeF(){

}

