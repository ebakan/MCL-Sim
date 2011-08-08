/*
 *  map.h
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
  * Library for operations on bitmaps
  */

#include <stdint.h>
#include <OpenGL/gl.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct {
    uint16_t height;
    uint16_t width;
    uint8_t* data;
} Map;

void byteArrayToBitArray(const uint8_t* bytes, const uint32_t numBytes, uint8_t* bits);
void clearMap(Map* map);
void initMap(Map* map, const uint16_t height, const uint16_t width);
void printMap(const Map* map);
uint8_t getVal(const uint16_t x, const uint16_t y, const Map* map);
void setVal(const uint16_t x, const uint16_t y, const uint8_t val, Map* map);
void drawGrid(const Map* map);
void drawMap(const Map* map);
uint8_t getVal_float(const float x, const float y, const Map* map);
void setVal_float(const float x, const float y, const uint8_t val, Map* map);

