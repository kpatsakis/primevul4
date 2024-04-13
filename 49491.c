static void xor_vectors(unsigned char *in1, unsigned char *in2,
			unsigned char *out, unsigned int size)
{
	int i;

	for (i = 0; i < size; i++)
		out[i] = in1[i] ^ in2[i];

}
