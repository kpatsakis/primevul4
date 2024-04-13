static inline void swap_words_in_key_and_bits_in_byte(const u8 *in,
						      u8 *out, u32 len)
{
	unsigned int i = 0;
	int j;
	int index = 0;

	j = len - BYTES_PER_WORD;
	while (j >= 0) {
		for (i = 0; i < BYTES_PER_WORD; i++) {
			index = len - j - BYTES_PER_WORD + i;
			out[j + i] =
				swap_bits_in_byte(in[index]);
		}
		j -= BYTES_PER_WORD;
	}
}
