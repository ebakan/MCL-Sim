/**
  * Monte Carlo Localization Test
  * Written by Eric Bakan
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

