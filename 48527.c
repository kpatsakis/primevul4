int lrw_crypt(struct blkcipher_desc *desc, struct scatterlist *sdst,
	      struct scatterlist *ssrc, unsigned int nbytes,
	      struct lrw_crypt_req *req)
{
	const unsigned int bsize = LRW_BLOCK_SIZE;
	const unsigned int max_blks = req->tbuflen / bsize;
	struct lrw_table_ctx *ctx = req->table_ctx;
	struct blkcipher_walk walk;
	unsigned int nblocks;
	be128 *iv, *src, *dst, *t;
	be128 *t_buf = req->tbuf;
	int err, i;

	BUG_ON(max_blks < 1);

	blkcipher_walk_init(&walk, sdst, ssrc, nbytes);

	err = blkcipher_walk_virt(desc, &walk);
	nbytes = walk.nbytes;
	if (!nbytes)
		return err;

	nblocks = min(walk.nbytes / bsize, max_blks);
	src = (be128 *)walk.src.virt.addr;
	dst = (be128 *)walk.dst.virt.addr;

	/* calculate first value of T */
	iv = (be128 *)walk.iv;
	t_buf[0] = *iv;

	/* T <- I*Key2 */
	gf128mul_64k_bbe(&t_buf[0], ctx->table);

	i = 0;
	goto first;

	for (;;) {
		do {
			for (i = 0; i < nblocks; i++) {
				/* T <- I*Key2, using the optimization
				 * discussed in the specification */
				be128_xor(&t_buf[i], t,
						&ctx->mulinc[get_index128(iv)]);
				inc(iv);
first:
				t = &t_buf[i];

				/* PP <- T xor P */
				be128_xor(dst + i, t, src + i);
			}

			/* CC <- E(Key2,PP) */
			req->crypt_fn(req->crypt_ctx, (u8 *)dst,
				      nblocks * bsize);

			/* C <- T xor CC */
			for (i = 0; i < nblocks; i++)
				be128_xor(dst + i, dst + i, &t_buf[i]);

			src += nblocks;
			dst += nblocks;
			nbytes -= nblocks * bsize;
			nblocks = min(nbytes / bsize, max_blks);
		} while (nblocks > 0);

		err = blkcipher_walk_done(desc, &walk, nbytes);
		nbytes = walk.nbytes;
		if (!nbytes)
			break;

		nblocks = min(nbytes / bsize, max_blks);
		src = (be128 *)walk.src.virt.addr;
		dst = (be128 *)walk.dst.virt.addr;
	}

	return err;
}
