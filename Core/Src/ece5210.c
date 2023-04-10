#include "ece5210.h"

#include <stdio.h>
#include <string.h>
#include <math.h>

#define PI 3.14159265358979323846264338327950288f

#define NUM_TAPS 5 // need to adjust
#define W_C (PI / 2)

float h_lp[NUM_TAPS] = {0.f};
float h_hp[NUM_TAPS] = {0.f};


/**
   Generates a windowed sinc lowpass filter with cutoff
   frequency defined as a constant W_C with the number of
   taps also defined as a constant (NUM_TAPS).  This filter
   uses a Hamming window.

   * @param void
   * @return void
   */
void init_firwin(void)
{
    uint16_t M = NUM_TAPS - 1;
    float sum = 0.f;

    for (uint16_t n = 0; n < NUM_TAPS; n++)
    {
        if ((n == M/2) && (NUM_TAPS % 2))
        {
            h_lp[n] = W_C/PI;
        }
        else
        {
            h_lp[n] = sinf(W_C*(n - (float)M/2)) /
                (PI*(n - (float)M/2));
        }

        h_lp[n] *= 0.54f - 0.46f*cosf(2*PI*n/M);
        
        sum += h_lp[n];
    }

    for (int n = 0; n < NUM_TAPS; n++)
    {
        h_lp[n] /= sum;
        if (n % 2 == 1)
        {
            h_hp[n] = -h_lp[n];
        }
        else
        {
            h_hp[n] = h_lp[n];
        }
    }
}



int16_t process_sample_left(int16_t sample_in)
{
    int16_t sample_out = 0;
    float sample_in_f = (float)sample_in;


    float sample_out_f = sample_in_f;

    // Convert back to int16_t
    sample_out = (int16_t)sample_out_f;
    return sample_out;
}


int16_t process_sample_right(int16_t sample_in)
{
    int16_t sample_out = 0;
    float sample_in_f = (float)sample_in;

    float sample_out_f = sample_in_f;

    // Convert back to int16_t
    sample_out = (int16_t)sample_out_f;
    return sample_out;
}
