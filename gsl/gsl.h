/**
  * gsl.h
  * Written by Eric Bakan
  * Headers for select GSL functions
  */

#ifndef GSL_H
#define GSL_H

#include <stdlib.h>

#define GSL_SUCCESS 0

typedef struct
  {
    const char *name;
    unsigned long int max;
    unsigned long int min;
    size_t size;
    void (*set) (void *state, unsigned long int seed);
    unsigned long int (*get) (void *state);
    double (*get_double) (void *state);
  }
gsl_rng_type;

typedef struct
  {
    const gsl_rng_type * type;
    void *state;
  }
gsl_rng;

extern const gsl_rng_type *gsl_rng_taus;

const gsl_rng_type ** gsl_rng_types_setup(void);

double gsl_ran_gaussian_ziggurat (const gsl_rng * r, const double sigma);

void gsl_rng_free (gsl_rng * r);
const gsl_rng_type *gsl_rng_default;
unsigned long int gsl_rng_default_seed;
gsl_rng *gsl_rng_alloc (const gsl_rng_type * T);
int gsl_rng_memcpy (gsl_rng * dest, const gsl_rng * src);
gsl_rng *gsl_rng_clone (const gsl_rng * r);
void gsl_rng_free (gsl_rng * r);
void gsl_rng_set (const gsl_rng * r, unsigned long int seed);
unsigned long int gsl_rng_max (const gsl_rng * r);
unsigned long int gsl_rng_min (const gsl_rng * r);
const char *gsl_rng_name (const gsl_rng * r);
size_t gsl_rng_size (const gsl_rng * r);
void * gsl_rng_state (const gsl_rng * r);
void gsl_rng_print_state (const gsl_rng * r);
const gsl_rng_type * gsl_rng_env_setup (void);

unsigned long int gsl_rng_get (const gsl_rng * r);
double gsl_rng_uniform (const gsl_rng * r);
double gsl_rng_uniform_pos (const gsl_rng * r);
unsigned long int gsl_rng_uniform_int (const gsl_rng * r, unsigned long int n);

#endif//GSL_H

