//======================================================================
//
// gimli_reference.c
// -----------------
// Reference model for the Gimli permutation.
// Retrieved from: https://gimli.cr.yp.to/spec.html
//
//======================================================================

#include <stdint.h>
#include <stdio.h>

uint32_t rotate(uint32_t x, int bits)
{
  if (bits == 0) return x;
  return (x << bits) | (x >> (32 - bits));
}

extern void gimli(uint32_t *state)
{
  int round;
  int column;
  uint32_t x;
  uint32_t y;
  uint32_t z;

  for (round = 24; round > 0; --round)
  {
    for (column = 0; column < 4; ++column)
    {
      x = rotate(state[    column], 24);
      y = rotate(state[4 + column],  9);
      z =        state[8 + column];

      state[8 + column] = x ^ (z << 1) ^ ((y&z) << 2);
      state[4 + column] = y ^ x        ^ ((x|z) << 1);
      state[column]     = z ^ y        ^ ((x&y) << 3);
    }

    if ((round & 3) == 0) { // small swap: pattern s...s...s... etc.
      x = state[0];
      state[0] = state[1];
      state[1] = x;
      x = state[2];
      state[2] = state[3];
      state[3] = x;
    }
    if ((round & 3) == 2) { // big swap: pattern ..S...S...S. etc.
      x = state[0];
      state[0] = state[2];
      state[2] = x;
      x = state[1];
      state[1] = state[3];
      state[3] = x;
    }

    if ((round & 3) == 0) { // add constant: pattern c...c...c... etc.
      state[0] ^= (0x9e377900 | round);
    }
  }
}

//------------------------------------------------------------------
//------------------------------------------------------------------
void print_state(uint32_t *state) {
  printf("State:\n");
  for (int i = 0; i < 4 ; i += 4) {
    printf("0x%08x 0x%08x 0x%08x 0x%08x\n",
	   state[(i + 0)], state[(i + 1)], state[(i + 2)], state[(i + 3)]);
  }
  printf("\n");
}


//------------------------------------------------------------------
//------------------------------------------------------------------
int main(void) {
  uint32_t my_state[12];

  for (uint32_t i = 0 ; i < 12 ; i++) {
    my_state[i] = i;
  }

  print_state(&my_state[0]);
  gimli(&my_state[0]);
  print_state(&my_state[0]);

}

//------------------------------------------------------------------
//------------------------------------------------------------------
