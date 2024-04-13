static inline void rep_xcrypt_ecb(const u8 *input, u8 *output, void *key,
				  struct cword *control_word, int count)
{
	asm volatile (".byte 0xf3,0x0f,0xa7,0xc8"	/* rep xcryptecb */
		      : "+S"(input), "+D"(output)
		      : "d"(control_word), "b"(key), "c"(count));
}
