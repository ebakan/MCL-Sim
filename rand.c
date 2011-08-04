/**
  * rand.c
  * Written by Eric Bakan
  */

#include "rand.h"
#include "gsl/gsl.h"
#include <time.h>
#include <stdio.h>

const gsl_rng_type* T;
gsl_rng* r;

float randf()
{
    return gsl_rng_uniform(r);
}

float randgauss(const float mu, const float sigma)
{
    return gsl_ran_gaussian_ziggurat(r,sigma)+mu;
}

void init_rng()
{
    gsl_rng_env_setup();
    T = gsl_rng_taus;
    r = gsl_rng_alloc(T);
    gsl_rng_set(r,time(NULL));
}

void free_rng()
{
    gsl_rng_free(r);
}

