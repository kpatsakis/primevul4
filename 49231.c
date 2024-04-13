static unsigned int __ctr_crypt(struct blkcipher_desc *desc,
				struct blkcipher_walk *walk)
{
	struct bf_ctx *ctx = crypto_blkcipher_ctx(desc->tfm);
	unsigned int bsize = BF_BLOCK_SIZE;
	unsigned int nbytes = walk->nbytes;
	u64 *src = (u64 *)walk->src.virt.addr;
	u64 *dst = (u64 *)walk->dst.virt.addr;
	u64 ctrblk = be64_to_cpu(*(__be64 *)walk->iv);
	__be64 ctrblocks[4];

	/* Process four block batch */
	if (nbytes >= bsize * 4) {
		do {
			if (dst != src) {
				dst[0] = src[0];
				dst[1] = src[1];
				dst[2] = src[2];
				dst[3] = src[3];
			}

			/* create ctrblks for parallel encrypt */
			ctrblocks[0] = cpu_to_be64(ctrblk++);
			ctrblocks[1] = cpu_to_be64(ctrblk++);
			ctrblocks[2] = cpu_to_be64(ctrblk++);
			ctrblocks[3] = cpu_to_be64(ctrblk++);

			blowfish_enc_blk_xor_4way(ctx, (u8 *)dst,
						  (u8 *)ctrblocks);

			src += 4;
			dst += 4;
		} while ((nbytes -= bsize * 4) >= bsize * 4);

		if (nbytes < bsize)
			goto done;
	}

	/* Handle leftovers */
	do {
		if (dst != src)
			*dst = *src;

		ctrblocks[0] = cpu_to_be64(ctrblk++);

		blowfish_enc_blk_xor(ctx, (u8 *)dst, (u8 *)ctrblocks);

		src += 1;
		dst += 1;
	} while ((nbytes -= bsize) >= bsize);

done:
	*(__be64 *)walk->iv = cpu_to_be64(ctrblk);
	return nbytes;
}
