static unsigned int __cbc_decrypt(struct blkcipher_desc *desc,
				  struct blkcipher_walk *walk)
{
	struct cast5_ctx *ctx = crypto_blkcipher_ctx(desc->tfm);
	const unsigned int bsize = CAST5_BLOCK_SIZE;
	unsigned int nbytes = walk->nbytes;
	u64 *src = (u64 *)walk->src.virt.addr;
	u64 *dst = (u64 *)walk->dst.virt.addr;
	u64 last_iv;

	/* Start of the last block. */
	src += nbytes / bsize - 1;
	dst += nbytes / bsize - 1;

	last_iv = *src;

	/* Process multi-block batch */
	if (nbytes >= bsize * CAST5_PARALLEL_BLOCKS) {
		do {
			nbytes -= bsize * (CAST5_PARALLEL_BLOCKS - 1);
			src -= CAST5_PARALLEL_BLOCKS - 1;
			dst -= CAST5_PARALLEL_BLOCKS - 1;

			cast5_cbc_dec_16way(ctx, (u8 *)dst, (u8 *)src);

			nbytes -= bsize;
			if (nbytes < bsize)
				goto done;

			*dst ^= *(src - 1);
			src -= 1;
			dst -= 1;
		} while (nbytes >= bsize * CAST5_PARALLEL_BLOCKS);
	}

	/* Handle leftovers */
	for (;;) {
		__cast5_decrypt(ctx, (u8 *)dst, (u8 *)src);

		nbytes -= bsize;
		if (nbytes < bsize)
			break;

		*dst ^= *(src - 1);
		src -= 1;
		dst -= 1;
	}

done:
	*dst ^= *(u64 *)walk->iv;
	*(u64 *)walk->iv = last_iv;

	return nbytes;
}
