/*
 * Base-64 encoding and decoding.
 *
 */

int base64_encode(unsigned char const *src, size_t srclength, char *target, size_t targsize);
int base64_decode(char const *src, unsigned char *target, size_t targsize);

