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

