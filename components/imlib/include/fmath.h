/**
 * Fast approximate math functions.
 */
#ifndef __FMATH_H__
#define __FMATH_H__

#include <stdlib.h>
#include <stdint.h>
#include <float.h>

#include <math.h>

float fast_atanf(float x);
float fast_atan2f(float y, float x);
float fast_expf(float x);
float fast_cbrtf(float d);
float fast_log(float x);
float fast_log2(float x);
float fast_powf(float a, float b);
void fast_get_min_max(float *data, size_t data_len, float *p_min, float *p_max);

// Fast square root function
static inline float fast_sqrtf(float x)
{
    return sqrtf(x);
}

// Fast floor function
static inline int fast_floorf(float x)
{
    int i = (int) x;
    return (x < 0 && x != i) ? i - 1 : i;
}

// Fast ceil function
static inline int fast_ceilf(float x)
{
    int i = (int) x;
    return (x > 0 && x != i) ? i + 1 : i;
}

// Fast round function
static inline int fast_roundf(float x)
{
    return (int) (x + (x >= 0 ? 0.5f : -0.5f));
}

// Fast fabs function
static inline float fast_fabsf(float x)
{
    return (x >= 0) ? x : -x;
}

#endif // __FMATH_H__