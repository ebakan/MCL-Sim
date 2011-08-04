
/**
  * gsl.c
  * Written by Eric Bakan
  * Definitions of functions
  */

#include "gsl.h"

unsigned long int
gsl_rng_get (const gsl_rng * r)
{
  return (r->type->get) (r->state);
}

double
gsl_rng_uniform (const gsl_rng * r)
{
  return (r->type->get_double) (r->state);
}

double
gsl_rng_uniform_pos (const gsl_rng * r)
{
  double x ;
  do
    {
      x = (r->type->get_double) (r->state) ;
    }
  while (x == 0) ;

  return x ;
}

/* Note: to avoid integer overflow in (range+1) we work with scale =
   range/n = (max-min)/n rather than scale=(max-min+1)/n, this reduces
   efficiency slightly but avoids having to check for the out of range
   value.  Note that range is typically O(2^32) so the addition of 1
   is negligible in most usage. */

unsigned long int
gsl_rng_uniform_int (const gsl_rng * r, unsigned long int n)
{
  unsigned long int offset = r->type->min;
  unsigned long int range = r->type->max - offset;
  unsigned long int scale;
  unsigned long int k;

  if (n > range || n == 0) 
    {
	abort();
    }

  scale = range / n;

  do
    {
      k = (((r->type->get) (r->state)) - offset) / scale;
    }
  while (k >= n);

  return k;
}

