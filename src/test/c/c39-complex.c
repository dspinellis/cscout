/* test program for complex.h support */

#include <complex.h>

int main() {
	complex c1;
	_Complex c2;
	float _Complex c3 = 2.0f + 2.0f*_Complex_I;

	imaginary c4;
	_Imaginary c5;
	float _Imaginary c7;

	float _Imaginary c8 = _Imaginary_I;
	float _Imaginary c9 = _Complex_I;
	float _Imaginary c10 = I;

	/*
	   Just an example; like this any function or macro function
	   declared in the standard complex.h when used, will be compiled by CScout
	*/
	double ca = cabs(c3);
	return 0;
}
