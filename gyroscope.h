#ifndef GYROSCOPE_h
#define GYROSCOPE_h

#include "L3G4200D.h"

void setup_L3G4200D();
void setup_gyroscope();
struct Vector_Float update_gyroscope();

#endif
