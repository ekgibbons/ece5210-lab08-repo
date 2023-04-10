#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "test_utils/utest.h"

#include "ece5210.h"

#define NUM_SAMP 10000

typedef struct array
{
    uint32_t size;
    int16_t data[];
} array;

typedef struct array_f32
{
    uint32_t size;
    float data[];
} array_f32;


double mean_absolute_error(array *y, array *x);
double mean_absolute_error_f32(array_f32 *y, array_f32 *x);
array * initialize_array(uint32_t size);
array_f32 * initialize_array_f32(uint32_t size);
void skip_lines(FILE *fp, int n_lines);
array * read_1d_mtx(char filename[]);
array_f32 * read_1d_f32_mtx(char filename[]);
array_f32 * rand_uniform_f32(float start, float end,
                             uint32_t size);
int rand_uniform_int(int start, int end);
float fir_test(float x, float *h, float *state, uint16_t size);


UTEST(ece5210_lab08, fir_half)
{

    uint8_t num_taps = (uint8_t)rand_uniform_int(30,55);
    if (num_taps % 2 == 0)
    {
        num_taps++;
    }

    array_f32 *x = rand_uniform_f32(-2,2,NUM_SAMP);
    array_f32 *y_half = initialize_array_f32(NUM_SAMP);
    array_f32 *y_trad = initialize_array_f32(NUM_SAMP);

    array_f32 *h_half = rand_uniform_f32(-2,2,(num_taps + 1)/2);
    array_f32 *h_full = rand_uniform_f32(-2,2,num_taps);

    array_f32 *state = initialize_array_f32(num_taps);
    
    for (uint8_t n = 0; n < h_full->size; n++)
    {
        if (n < (num_taps + 1)/2)
        {
            h_full->data[n] = h_half->data[n];
        }
        else
        {
            h_full->data[n] = h_half->data[num_taps - n - 1];
        }
    }

    for (uint16_t n = 0; n < y_trad->size; n++)
    {
        y_trad->data[n] = fir_test(x->data[n], h_full->data,
                                   state->data,
                                   (uint16_t)h_full->size);
        y_half->data[n] = lpf(x->data[n], h_half->data,
                              (uint8_t)h_full->size-1);
    }

    double error = mean_absolute_error_f32(y_half, y_trad);

    ASSERT_LT(error, 1e-5);

    free(x);
    free(y_half);
    free(y_trad);
    free(h_half);
    free(h_full);
    free(state);

    x = NULL;
    y_half = NULL;
    y_trad = NULL;
    h_half = NULL;
    h_full = NULL;
    state = NULL;
    
}

UTEST(ece5210_lab08, fir_least_squares)
{

    int ret = system("python test_utils/ece5210_lab08_firls.py");
    ASSERT_FALSE(ret);
    
}

UTEST_MAIN();

/************************************* 
         SUPPORT FUNCTIONS 
*************************************/

double mean_absolute_error(array *y, array *x)
{
    double diff = 0;
    for (int n = 0; n < y->size; n++)
    {
        diff += fabs((double)y->data[n] - (double)x->data[n]);
    }

    return diff/y->size;
}

double mean_absolute_error_f32(array_f32 *y, array_f32 *x)
{
    double diff = 0;
    for (int n = 0; n < y->size; n++)
    {
        diff += fabs((double)y->data[n] - (double)x->data[n]);
    }

    return diff/y->size;
}


void skip_lines(FILE *fp, int n_lines)
{
    // Skip the first n lines
    for (int i = 0; i < n_lines; ++i)
    {
        if (fscanf(fp, "%*[^\n]\n") == -1)
        {
            printf("ERROR: fscanf() failed in %s on line %i\n",
                   __FILE__,__LINE__);
        }
    }
}

array * initialize_array(uint32_t size)
{
    array *out = malloc(sizeof(*out) +
                        sizeof(int16_t)*(size_t)size);

    out->size = size;
    return out;
}

array_f32 * initialize_array_f32(uint32_t size)
{
    array_f32 *out = malloc(sizeof(*out) +
                            sizeof(float)*(size_t)size);

    out->size = size;
    return out;
}



array * read_1d_mtx(char filename[])
{
    int temp;
    
    FILE *fp;
    fp = fopen(filename,"r");

    int n_lines = 2;
    skip_lines(fp, n_lines);


    // read in data and find max value
    if (fscanf(fp,"%i %*i", &temp) == -1)
    {
        printf("ERROR: fscanf in %s in line %i failed\n",
               __FILE__,__LINE__);
        exit(1);
    }

    array *out = initialize_array((uint32_t)temp);

    for (uint32_t m = 0; m < out->size; m++)
    {
        if ((fscanf(fp,"%i", &temp) == 1) &&
            !feof(fp))
        {
            out->data[m] = (int16_t)temp;		
        }
        else
        {
            printf("ERROR:  fscanf() failed\n");
        }
    }

    fclose(fp);
    
    return out;
}


array_f32 * read_1d_f32_mtx(char filename[])
{

    
    int size;
    float temp;
    
    FILE *fp;
    fp = fopen(filename,"r");

    int n_lines = 2;
    skip_lines(fp, n_lines);

    // read in the size
    if (fscanf(fp,"%i %*f", &size) == -1)
    {
        printf("ERROR: fscanf in %s in line %i failed\n",
               __FILE__,__LINE__);
        exit(1);
    }

    array_f32 *out = initialize_array_f32((uint32_t)size);

    for (uint32_t m = 0; m < out->size; m++)
    {
        if ((fscanf(fp,"%f", &temp) == 1) &&
            !feof(fp))
        {
            out->data[m] = temp;		
        }
        else
        {
            printf("ERROR:  fscanf() failed\n");
        }
    }

    fclose(fp);
    
    return out;
}

array_f32 * rand_uniform_f32(float start, float end, uint32_t size)
{
    
    array_f32 *out = initialize_array_f32((uint32_t)size);

    srand((unsigned int)time(NULL));
    
    float a = end - start;
    float offset = a/2;
    for (uint32_t i = 0; i < out->size; i++)
    {
        out->data[i] = (float)rand()/(float)(RAND_MAX/a) - offset;
    }

    return out;
}

int rand_uniform_int(int start, int end)
{
    srand((unsigned int)time(NULL));
    
    int out = (rand() %
               (end - start + 1)) + start;    

    return out;
}



float fir_test(float x, float *h, float *state, uint16_t size)
{
    float out = 0.f;

    for (uint16_t i = size - 1; i > 0; i--)
    {
        state[i] = state[i-1];
    }
    state[0] = x;

    
    for (uint16_t k = 0; k < size; k++)
    {
        out += h[k]*state[k];
    }

    return out;
}


