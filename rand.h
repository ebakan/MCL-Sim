/**
  * rand.h
  * Written by Eric Bakan
  * Provides an interface and
  * wrappers for random functions
  * from the GNU Scientific Library
  */

#ifndef RAND_H
#define RAND_H

float randf();
float randgauss(const float sigma, const float mu);
void init_rng();
void free_rng();

#endif//RAND_H

