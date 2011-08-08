/*
 *  structs.h
 *  Copyright (C) 2011 Eric Bakan
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
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

