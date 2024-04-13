static inline bool camellia_fpu_begin(bool fpu_enabled, unsigned int nbytes)
{
	return glue_fpu_begin(CAMELLIA_BLOCK_SIZE,
			      CAMELLIA_AESNI_PARALLEL_BLOCKS, NULL, fpu_enabled,
			      nbytes);
}
