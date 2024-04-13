static inline bool cast6_fpu_begin(bool fpu_enabled, unsigned int nbytes)
{
	return glue_fpu_begin(CAST6_BLOCK_SIZE, CAST6_PARALLEL_BLOCKS,
			      NULL, fpu_enabled, nbytes);
}
