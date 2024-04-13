static inline struct aes_ctx *aes_ctx_common(void *ctx)
{
	unsigned long addr = (unsigned long)ctx;
	unsigned long align = PADLOCK_ALIGNMENT;

	if (align <= crypto_tfm_ctx_alignment())
		align = 1;
	return (struct aes_ctx *)ALIGN(addr, align);
}
