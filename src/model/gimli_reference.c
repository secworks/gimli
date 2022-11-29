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
  for (int i = 0; i < 12 ; i += 4) {
    printf("0x%08x 0x%08x 0x%08x 0x%08x\n",
	   state[(i + 0)], state[(i + 1)], state[(i + 2)], state[(i + 3)]);
  }
  printf("\n");
}


//------------------------------------------------------------------
//------------------------------------------------------------------
void check_state(uint32_t *state, uint32_t *expected) {
  int correct = 1;
  for (int i = 0; i < 12 ; i++) {
    if (state[i] != expected[i]) {
	correct = 0;
      }
  }

  if (correct) {
    printf("Correct result generated.\n");
  } else {
    printf("Error: Correct result NOT generated.\n");
    printf("Expected:\n");
    print_state(expected);

    printf("Got:\n");
    print_state(state);
  }
}


//------------------------------------------------------------------
// test1
// The input consists of all zero in the words.
//------------------------------------------------------------------
void test1() {
  uint32_t my_state[12];

  uint32_t my_expected[12] = {0x6467d8c4, 0x07dcf83b, 0x3b0bb0d4, 0x1b21364c,
                              0x083431dc, 0x0efbbe8e, 0x0054e884, 0x648bd955,
			      0x4a5db42e, 0xca0641cb, 0x8673d2c2, 0x2e30d809};

  printf("Test 1: An all zero input.\n");

  for (uint32_t i = 0 ; i < 12 ; i++) {
    my_state[i] = 0x00000000;
  }

  gimli(&my_state[0]);
  check_state(my_state, my_expected);
}


//------------------------------------------------------------------
// test2
// The input consists of all bits in the words set.
// in the LSB.
//------------------------------------------------------------------
void test2() {
  uint32_t my_state[12];

  uint32_t my_expected[12] = {0xb9d9fb03, 0x987f9e0e, 0xfeb97bac, 0x984a916c,
			      0x91c8c346, 0x734686ae, 0xbc982e4a, 0xafbfe0b7,
			      0xc85d43cb, 0x0724215a, 0x4f087199, 0x2d53ad4f};

  printf("Test 2: An all one input.\n");

  for (uint32_t i = 0 ; i < 12 ; i++) {
    my_state[i] = 0xffffffff;
  }

  gimli(&my_state[0]);
  check_state(my_state, my_expected);
}


//------------------------------------------------------------------
// test3
// The input consists of a monotonically increasing sequence
// in the LSB.
//------------------------------------------------------------------
void test3() {
  uint32_t my_state[12];

  uint32_t my_expected[12] = {0xa2ce911c, 0xc14a1a58, 0xc4cb5b8c, 0xbe72f0e0,
			      0x27a6c8ec, 0xa27a6d04, 0xf59334c0, 0x810c6a52,
			      0x3a9a427a, 0xd5c7af1a, 0x35457971, 0x0ea34284};

  printf("Test 3: a monotonically increasing sequence in the LSB.\n");

  for (uint32_t i = 0 ; i < 12 ; i++) {
    my_state[i] = i;
  }

  gimli(&my_state[0]);
  check_state(my_state, my_expected);
}


//------------------------------------------------------------------
// test4
//
// Input vector from gimi reference and paper.
//------------------------------------------------------------------
void test4() {
  uint32_t my_state[12] = {0x00000000, 0x9e3779ba, 0x3c6ef37a, 0xdaa66d46,
                           0x78dde724, 0x1715611a, 0xb54cdb2e, 0x53845566,
			   0xf1bbcfc8, 0x8ff34a5a, 0x2e2ac522, 0xcc624026};

  uint32_t my_expected[12] = {0xba11c85a, 0x91bad119, 0x380ce880, 0xd24c2c68,
			      0x3eceffea, 0x277a921c, 0x4f73a0bd, 0xda5a9cd8,
			      0x84b673f0, 0x34e52ff7, 0x9e2bef49, 0xf41bb8d6};

  printf("Test 3: Input vector from reference and paper.\n");

  gimli(&my_state[0]);
  check_state(my_state, my_expected);
}


//------------------------------------------------------------------
//------------------------------------------------------------------
int main(void) {
  printf("Reference model started.\n");

  test1();
  test2();
  test3();
  test4();

  printf("Reference model completed.\n");
}

//------------------------------------------------------------------
//------------------------------------------------------------------
