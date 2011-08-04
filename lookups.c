/**
  * lookups.h
  * Written by Eric Bakan
  * Provides lookup tables for
  * trig functions, dynamically-generated
  * lookup binary trees for other math functions
  */

#include "lookups.h"
#include "mcl.h"
#include <math.h>

typedef struct {
    float in;
    float out;
} lookup;

float* sintable;
float* costable;
float* tantable;
uint32_t tableSize;
double divisionSize;

void genTrigTables(const uint32_t numVals)
{
    tableSize=numVals;
    divisionSize=1.f/(tableSize-1);
    printf("divsize: %f\n",divisionSize);

    sintable=malloc(sizeof(float)*tableSize);
    costable=malloc(sizeof(float)*tableSize);
    tantable=malloc(sizeof(float)*tableSize);

    float incrSize=divisionSize*2*M_PI;
    float val=0;

    for(uint32_t i=0;i<numVals;i++,val+=incrSize) {
	//printf("i: %d val: %f\n",i,val);
	sintable[i]=sin(val);
	costable[i]=cos(val);
	tantable[i]=tan(val);
    }
}

void freeTrigTables()
{
    if(sintable) {
	free(sintable);
	sintable=NULL;
    }
    if(costable) {
	free(costable);
	costable=NULL;
    }
    if(tantable) {
	free(tantable);
	tantable=NULL;
    }
    tableSize=0;
    divisionSize=0;
}

//NOTE: though rounding the index would
//yield more accurate results, it is
//faster to just truncate the result
//and for sufficiently high tableSize
//the error is negligible

float sin_lookup(const float val)
{
    if(!sintable)
	return sinf(val);
    float modval=circlemod(val);
    uint32_t index=(modval*tableSize/(M_PI*2));
    return sintable[index];
}

float cos_lookup(const float val)
{
    if(!costable)
	return cosf(val);
    float modval=circlemod(val);
    uint32_t index=(modval*tableSize/(M_PI*2));
    return costable[index];
}

float tan_lookup(const float val)
{
    if(!tantable)
	return tanf(val);
    float modval=circlemod(val);
    uint32_t index=(modval*tableSize/(M_PI*2));
    return tantable[index];
}

void testLookup(const uint32_t tableVals, const uint32_t calcVals)
{
    freeTrigTables();
    genTrigTables(tableVals);

    printf("num table cells: %d\nnum test cells: %d\n",tableVals,calcVals);

    double calcincr=2*M_PI/(calcVals-1);
    double testVal=0;

    double sin_diff_avg=0;
    double cos_diff_avg=0;
    double tan_diff_avg=0;

    for(uint32_t i=0;i<calcVals;i++,testVal+=calcincr) {

	double sin_calc=sin(testVal);
	double cos_calc=cos(testVal);
	double tan_calc=tan(testVal);

	double sin_look=sin_lookup(testVal);
	double cos_look=cos_lookup(testVal);
	double tan_look=tan_lookup(testVal);

	double sin_diff=sin_calc-sin_look;
	double cos_diff=cos_calc-cos_look;
	double tan_diff=tan_calc-tan_look;

	sin_diff_avg+=sin_diff;
	cos_diff_avg+=cos_diff;
	tan_diff_avg+=tan_diff;

	printf("test val: %f\nsin_c: %f\nsin_l: %f\nsin_d: %f\ncos_c: %f\ncos_l: %f\ncos_d: %f\ntan_c: %f\ntan_l: %f\ntan_d: %f\n\n",
		testVal,      sin_calc,  sin_look,  sin_diff,  cos_calc,  cos_look,  cos_diff,  tan_calc,  tan_look,  tan_diff);  
    }

    sin_diff_avg/=calcVals;
    cos_diff_avg/=calcVals;
    tan_diff_avg/=calcVals;

    printf("averages:\nsin: %f\ncos: %f\ntan: %f\n",sin_diff_avg,cos_diff_avg,tan_diff_avg);

    freeTrigTables();
}

