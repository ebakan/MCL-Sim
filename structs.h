/**
  * structures.h
  * Written by Eric Bakan
  * A bunch of useful structures
  * for Monte-Carlo localization
  */

#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <stdint.h>

//comment this line out to use standard trig functions
#define USE_LOOKUPS 1

#ifdef USE_LOOKUPS
#include "lookups.h"
#define sinf(x) sin_lookup(x)
#define cosf(x) cos_lookup(x)
#define tanf(x) tan_lookup(x)
#endif//USE_LOOKUPS



typedef struct
{
    float x;
    float y;
    float theta;
} Pose;

typedef struct
{
    Pose pose;
    float weight;
} Particle;

typedef struct
{
    float delta_angle;
    float delta_length;
} Command;

typedef struct
{
    float laserrange;
} SensorData;

typedef struct
{
    float top;
    float bottom;
    float left;
    float right;
} BoundingBox;

#endif//STRUCTURES_H

