static unsigned int __ctr_crypt(struct blkcipher_desc *desc,
				struct blkcipher_walk *walk)
{
	struct cast5_ctx *ctx = crypto_blkcipher_ctx(desc->tfm);
	const unsigned int bsize = CAST5_BLOCK_SIZE;
	unsigned int nbytes = walk->nbytes;
	u64 *src = (u64 *)walk->src.virt.addr;
	u64 *dst = (u64 *)walk->dst.virt.addr;

	/* Process multi-block batch */
	if (nbytes >= bsize * CAST5_PARALLEL_BLOCKS) {
		do {
			cast5_ctr_16way(ctx, (u8 *)dst, (u8 *)src,
					(__be64 *)walk->iv);

			src += CAST5_PARALLEL_BLOCKS;
			dst += CAST5_PARALLEL_BLOCKS;
			nbytes -= bsize * CAST5_PARALLEL_BLOCKS;
		} while (nbytes >= bsize * CAST5_PARALLEL_BLOCKS);

		if (nbytes < bsize)
			goto done;
	}

	/* Handle leftovers */
	do {
		u64 ctrblk;

		if (dst != src)
			*dst = *src;

		ctrblk = *(u64 *)walk->iv;
		be64_add_cpu((__be64 *)walk->iv, 1);

		__cast5_encrypt(ctx, (u8 *)&ctrblk, (u8 *)&ctrblk);
		*dst ^= ctrblk;

		src += 1;
		dst += 1;
		nbytes -= bsize;
	} while (nbytes >= bsize);

done:
	return nbytes;
}
