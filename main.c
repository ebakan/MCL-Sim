/*
 *  main.c
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
  * Monte Carlo Localization Test
  */

#include <math.h>
#include <stdlib.h>

#include "gui.h"
#include "rand.h"

//basic structs
int main(int argc, char** argv)
{
    //lookup table test
    //testLookup(1000000,10000);

    unsigned int width=640;
    unsigned int height=640;
    char title[]="Monte Carlo Localization Simulator";

    //set map later, use simple box for now

    const unsigned int numParticles=10000;

    init_rng();
    atexit(free_rng);

#ifdef USE_LOOKUPS
    genTrigTables(100000);
    atexit(freeTrigTables);
#endif//USE_LOOKUPS

    //testing: start poses on robot and watch them spread
    initGlut(argc,argv,numParticles,title,width,height);
    //this function never returns, so the lines below are never
    //executed. need to solve by adding them to the 

    return 0;
}

