static unsigned int __cbc_encrypt(struct blkcipher_desc *desc,
				  struct blkcipher_walk *walk)
{
	struct cast5_ctx *ctx = crypto_blkcipher_ctx(desc->tfm);
	const unsigned int bsize = CAST5_BLOCK_SIZE;
	unsigned int nbytes = walk->nbytes;
	u64 *src = (u64 *)walk->src.virt.addr;
	u64 *dst = (u64 *)walk->dst.virt.addr;
	u64 *iv = (u64 *)walk->iv;

	do {
		*dst = *src ^ *iv;
		__cast5_encrypt(ctx, (u8 *)dst, (u8 *)dst);
		iv = dst;

		src += 1;
		dst += 1;
		nbytes -= bsize;
	} while (nbytes >= bsize);

	*(u64 *)walk->iv = *iv;
	return nbytes;
}
