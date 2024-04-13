static int ecb_crypt(struct blkcipher_desc *desc, struct blkcipher_walk *walk,
		     bool enc)
{
	bool fpu_enabled = false;
	struct cast5_ctx *ctx = crypto_blkcipher_ctx(desc->tfm);
	const unsigned int bsize = CAST5_BLOCK_SIZE;
	unsigned int nbytes;
	void (*fn)(struct cast5_ctx *ctx, u8 *dst, const u8 *src);
	int err;

	fn = (enc) ? cast5_ecb_enc_16way : cast5_ecb_dec_16way;

	err = blkcipher_walk_virt(desc, walk);
	desc->flags &= ~CRYPTO_TFM_REQ_MAY_SLEEP;

	while ((nbytes = walk->nbytes)) {
		u8 *wsrc = walk->src.virt.addr;
		u8 *wdst = walk->dst.virt.addr;

		fpu_enabled = cast5_fpu_begin(fpu_enabled, nbytes);

		/* Process multi-block batch */
		if (nbytes >= bsize * CAST5_PARALLEL_BLOCKS) {
			do {
				fn(ctx, wdst, wsrc);

				wsrc += bsize * CAST5_PARALLEL_BLOCKS;
				wdst += bsize * CAST5_PARALLEL_BLOCKS;
				nbytes -= bsize * CAST5_PARALLEL_BLOCKS;
			} while (nbytes >= bsize * CAST5_PARALLEL_BLOCKS);

			if (nbytes < bsize)
				goto done;
		}

		fn = (enc) ? __cast5_encrypt : __cast5_decrypt;

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

	cast5_fpu_end(fpu_enabled);
	return err;
}
