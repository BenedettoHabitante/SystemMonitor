#include "ncurses_display.h"
#include "system.h"

//include "linux_parser.h"  	//for testing
//#include <iostream>       	//for testing

int main() {
  System system;
  
  //system.Processes(); 		//for testing
  NCursesDisplay::Display(system);
}