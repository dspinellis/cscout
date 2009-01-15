/*
 * (C) Copyright 2001-2009 Diomidis Spinellis.  All rights reserved.
 *
 * You may only use this code if you agree to the terms of the CScout
 * Source Code License agreement (see License.txt).
 * If you do not agree to the terms, do not use the code.
 *
 * Base-64 encoding and decoding.
 *
 */

#ifndef BASE64_
#define BASE64_

#include <stddef.h>

int base64_encode(unsigned char const *src, size_t srclength, char *target, size_t targsize);
int base64_decode(char const *src, unsigned char *target, size_t targsize);

#endif /* BASE64_ */
