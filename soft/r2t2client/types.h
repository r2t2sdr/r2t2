#ifndef _TYPES_H_
#define _TYPES_H_

#include <stdint.h>
#include <complex>


typedef std::complex<double>  cdouble;
typedef std::complex<float>  cfloat;

typedef struct cfloat_s {
	float re;
	float im;
} cfloat_t;

typedef struct cdouble_s {
	double re;
	double im;
} cdouble_t;

#endif
