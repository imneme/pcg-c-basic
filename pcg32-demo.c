/*
 * PCG Random Number Generation for C.
 *
 * Copyright 2014 Melissa O'Neill <oneill@pcg-random.org>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 * For additional information about the PCG random number generation scheme,
 * including its license and other licensing options, visit
 *
 *     http://www.pcg-random.org
 */

/*
 * This file was mechanically generated from tests/check-pcg32.c
 */

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>

#include "pcg_basic.h"

int main(int argc, char** argv)
{
    // Read command-line options

    int rounds = 5;
    bool nondeterministic_seed = false;
    int round, i;

    ++argv;
    --argc;
    if (argc > 0 && strcmp(argv[0], "-r") == 0) {
        nondeterministic_seed = true;
        ++argv;
        --argc;
    }
    if (argc > 0) {
        rounds = atoi(argv[0]);
    }

    // In this version of the code, we'll use a local rng, rather than the
    // global one.

    pcg32_random_t rng;

    // You should *always* seed the RNG.  The usual time to do it is the
    // point in time when you create RNG (typically at the beginning of the
    // program).
    //
    // pcg32_srandom_r takes two 64-bit constants (the initial state, and the
    // rng sequence selector; rngs with different sequence selectors will
    // *never* have random sequences that coincide, at all) - the code below
    // shows three possible ways to do so.

    if (nondeterministic_seed) {
        // Seed with external entropy -- the time and some program addresses
        // (which will actually be somewhat random on most modern systems).
        // A better solution, entropy_getbytes, using /dev/random, is provided
        // in the full library.
        
        pcg32_srandom_r(&rng, time(NULL) ^ (intptr_t)&printf, 
			      (intptr_t)&rounds);
    } else {
        // Seed with a fixed constant

        pcg32_srandom_r(&rng, 42u, 54u);
    }

    printf("pcg32_random_r:\n"
           "      -  result:      32-bit unsigned int (uint32_t)\n"
           "      -  period:      2^64   (* 2^63 streams)\n"
           "      -  state type:  pcg32_random_t (%zu bytes)\n"
           "      -  output func: XSH-RR\n"
           "\n",
           sizeof(pcg32_random_t));

    for (round = 1; round <= rounds; ++round) {
        printf("Round %d:\n", round);
        /* Make some 32-bit numbers */
        printf("  32bit:");
        for (i = 0; i < 6; ++i)
            printf(" 0x%08x", pcg32_random_r(&rng));
        printf("\n");

        /* Toss some coins */
        printf("  Coins: ");
        for (i = 0; i < 65; ++i)
            printf("%c", pcg32_boundedrand_r(&rng, 2) ? 'H' : 'T');
        printf("\n");

        /* Roll some dice */
        printf("  Rolls:");
        for (i = 0; i < 33; ++i) {
            printf(" %d", (int)pcg32_boundedrand_r(&rng, 6) + 1);
        }
        printf("\n");

        /* Deal some cards */
        enum { SUITS = 4, NUMBERS = 13, CARDS = 52 };
        char cards[CARDS];

        for (i = 0; i < CARDS; ++i)
            cards[i] = i;

        for (i = CARDS; i > 1; --i) {
            int chosen = pcg32_boundedrand_r(&rng, i);
            char card = cards[chosen];
            cards[chosen] = cards[i - 1];
            cards[i - 1] = card;
        }

        printf("  Cards:");
        static const char number[] = {'A', '2', '3', '4', '5', '6', '7',
                                      '8', '9', 'T', 'J', 'Q', 'K'};
        static const char suit[] = {'h', 'c', 'd', 's'};
        for (i = 0; i < CARDS; ++i) {
            printf(" %c%c", number[cards[i] / SUITS], suit[cards[i] % SUITS]);
            if ((i + 1) % 22 == 0)
                printf("\n\t");
        }
        printf("\n");

        printf("\n");
    }

    return 0;
}
