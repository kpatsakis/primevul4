static inline unsigned int blkcipher_done_slow(struct crypto_blkcipher *tfm,
					       struct blkcipher_walk *walk,
					       unsigned int bsize)
{
	u8 *addr;
	unsigned int alignmask = crypto_blkcipher_alignmask(tfm);

	addr = (u8 *)ALIGN((unsigned long)walk->buffer, alignmask + 1);
	addr = blkcipher_get_spot(addr, bsize);
	scatterwalk_copychunks(addr, &walk->out, bsize, 1);
	return bsize;
}
