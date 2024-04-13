static int ecb_crypt(struct blkcipher_desc *desc, struct blkcipher_walk *walk,
		     void (*fn)(struct bf_ctx *, u8 *, const u8 *),
		     void (*fn_4way)(struct bf_ctx *, u8 *, const u8 *))
{
	struct bf_ctx *ctx = crypto_blkcipher_ctx(desc->tfm);
	unsigned int bsize = BF_BLOCK_SIZE;
	unsigned int nbytes;
	int err;

	err = blkcipher_walk_virt(desc, walk);

	while ((nbytes = walk->nbytes)) {
		u8 *wsrc = walk->src.virt.addr;
		u8 *wdst = walk->dst.virt.addr;

		/* Process four block batch */
		if (nbytes >= bsize * 4) {
			do {
				fn_4way(ctx, wdst, wsrc);

				wsrc += bsize * 4;
				wdst += bsize * 4;
				nbytes -= bsize * 4;
			} while (nbytes >= bsize * 4);

			if (nbytes < bsize)
				goto done;
		}

		/* Handle leftovers */
		do {
			fn(ctx, wdst, wsrc);

			wsrc += bsize;
			wdst += bsize;
			nbytes -= bsize;
		} while (nbytes >= bsize);

done:
		err = blkcipher_walk_done(desc, walk, nbytes);
	}

	return err;
}
