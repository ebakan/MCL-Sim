/*
 *  map.c
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

#include "map.h"

void byteArrayToBitArray(const uint8_t* bytes, const uint32_t numBytes, uint8_t* bits)
{
    uint32_t word=0;
    uint8_t bit=0;
    for(uint32_t i=0;i<numBytes;i++) {
	if(bytes[i])
	    bits[word]|=1<<bit;
	else
	    bits[word]&=~(1<<bit);
	bit++;
	word+=bit/8;
	bit%=8;
    }
}

void clearMap(Map* map)
{
    uint32_t numBytes=ceil(map->height*map->width/8.f);
    for(uint32_t i=0;i<numBytes;i++)
	map->data[i]=0;
}

void initMap(Map* map, const uint16_t height, const uint16_t width)
{
    map->height=height;
    map->width=width;
    uint32_t numBytes=ceil(height*width/8.f);
    map->data=malloc(sizeof(uint8_t)*numBytes);
    for(uint32_t i=0;i<numBytes;i++)
	map->data[i]=0;
}

void printMap(const Map* map)
{
    uint32_t word=0;
    uint8_t bit=0;
    for(uint32_t y=0;y<map->height;y++) {
	for(uint32_t x=0;x<map->width;x++) {
	    printf("%d",(map->data[word]>>bit)&1);
	    bit++;
	    word+=bit/8;
	    bit%=8;
	}
	printf("\n");
    }
}

uint8_t getVal(const uint16_t x, const uint16_t y, const Map* map)
{
    uint32_t pos=y*map->height+x;
    uint32_t words=pos/8;
    uint8_t bits=pos%8;
    return ((map->data[words])>>bits)&1;
}

void setVal(const uint16_t x, const uint16_t y, const uint8_t val, Map* map) {
    uint32_t pos=y*map->height+x;
    uint32_t words=pos/8;
    uint8_t bits=pos%8;
    if(val)
	map->data[words]|=1<<bits;
    else
	map->data[words]&=~(1<<bits);
}


void drawGrid(const Map* map)
{
    glBegin(GL_LINES);
    float cellwidth=2.f/map->width;
    float cellheight=2.f/map->height;
    for(int i=1;i<map->width;i++) {
	float xpos=-1+i*cellwidth;
	glVertex2f(-1,xpos);
	glVertex2f(1,xpos);
    }

    for(int i=1;i<map->height;i++) {
	float ypos=-1+i*cellheight;
	glVertex2f(ypos,-1);
	glVertex2f(ypos,1);
    }
    glEnd();
}

void drawMap(const Map* map)
{
    glBegin(GL_QUADS);
    float cellwidth=2.f/map->width;
    float cellheight=2.f/map->height;
    for(uint16_t y=0;y<map->height;y++) {
	for(uint16_t x=0;x<map->width;x++) {
	    if(getVal(x,y,map)) {
		float xl=-1+x*cellwidth;
		float xr=xl+cellwidth;
		float yt=1-y*cellheight;
		float yb=yt-cellheight;
		glVertex2f(xl,yt);
		glVertex2f(xr,yt);
		glVertex2f(xr,yb);
		glVertex2f(xl,yb);
	    }
	}
    }
    glEnd();
}

uint8_t getVal_float(const float x, const float y, const Map* map)
{
    uint16_t xval=(x+1)/2*map->width;
    uint16_t yval=(-y+1)/2*map->height;
    return getVal(xval,yval,map);
}


void setVal_float(const float x, const float y, const uint8_t val, Map* map)
{
    uint16_t xval=(x+1)/2*map->width;
    uint16_t yval=(-y+1)/2*map->height;
    setVal(xval,yval,val,map);
}

