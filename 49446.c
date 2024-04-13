static inline bool twofish_fpu_begin(bool fpu_enabled, unsigned int nbytes)
{
	return glue_fpu_begin(TF_BLOCK_SIZE, TWOFISH_PARALLEL_BLOCKS, NULL,
			      fpu_enabled, nbytes);
}
