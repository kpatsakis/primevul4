static inline void padlock_xcrypt_ecb(const u8 *input, u8 *output, void *key,
				      void *control_word, u32 count)
{
	u32 initial = count & (ecb_fetch_blocks - 1);

	if (count < ecb_fetch_blocks) {
		ecb_crypt(input, output, key, control_word, count);
		return;
	}

	if (initial)
		asm volatile (".byte 0xf3,0x0f,0xa7,0xc8"	/* rep xcryptecb */
			      : "+S"(input), "+D"(output)
			      : "d"(control_word), "b"(key), "c"(initial));

	asm volatile (".byte 0xf3,0x0f,0xa7,0xc8"	/* rep xcryptecb */
		      : "+S"(input), "+D"(output)
		      : "d"(control_word), "b"(key), "c"(count - initial));
}
