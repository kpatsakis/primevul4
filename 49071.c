static unsigned int __ctrblk_init(u8 *ctrptr, unsigned int nbytes)
{
	unsigned int i, n;

	/* only use complete blocks, max. PAGE_SIZE */
	n = (nbytes > PAGE_SIZE) ? PAGE_SIZE : nbytes & ~(AES_BLOCK_SIZE - 1);
	for (i = AES_BLOCK_SIZE; i < n; i += AES_BLOCK_SIZE) {
		memcpy(ctrptr + i, ctrptr + i - AES_BLOCK_SIZE,
		       AES_BLOCK_SIZE);
		crypto_inc(ctrptr + i, AES_BLOCK_SIZE);
	}
	return n;
}
