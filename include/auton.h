#pragma once
#include "robodash/views/console.hpp"
#include "robodash/views/selector.hpp"

//These are the names of the autons we are using
void RightAWP();
void LeftAWP();
void BlueRight();
void Score();
void Forwards();
void Skills();

//these are the names next to the actual loop using the name
static rd::Selector selector({
   {"Right AWP", &RightAWP},
   {"Left AWP", &LeftAWP},
   {"Blue Right Elim", &BlueRight},
   {"Blue Left Elim", &BlueLeft},
   {"Red Right Elim", &RedRight},
   {"Red Left Elim", &RedLeft},
   {"Forwards", &Forwards}, 
   {"Skills", &Skills},
   
   
}); 

static rd::Console console;