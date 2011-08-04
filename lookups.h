/**
  * lookups.h
  * Written by Eric Bakan
  * Provides lookup tables for
  * trig functions, dynamically-generated
  * lookup binary trees for other math functions
  */

#ifndef LOOKUPS_H
#define LOOKUPS_H

#include <stdint.h>

void genTrigTables(const uint32_t numVals);
void freeTrigTables();
float sin_lookup(const float val);
float cos_lookup(const float val);
float tan_lookup(const float val);
void testLookup(const uint32_t tableVals, const uint32_t calcVals);

/*
float exp_lookup(const float val);
float sqrt_lookup(const float val);
*/


#endif//LOOKUPS_H

