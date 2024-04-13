static int crypt(struct blkcipher_desc *d,
		 struct blkcipher_walk *w, struct priv *ctx,
		 void (*fn)(struct crypto_tfm *, u8 *, const u8 *))
{
	int err;
	unsigned int avail;
	const int bs = LRW_BLOCK_SIZE;
	struct sinfo s = {
		.tfm = crypto_cipher_tfm(ctx->child),
		.fn = fn
	};
	be128 *iv;
	u8 *wsrc;
	u8 *wdst;

	err = blkcipher_walk_virt(d, w);
	if (!(avail = w->nbytes))
		return err;

	wsrc = w->src.virt.addr;
	wdst = w->dst.virt.addr;

	/* calculate first value of T */
	iv = (be128 *)w->iv;
	s.t = *iv;

	/* T <- I*Key2 */
	gf128mul_64k_bbe(&s.t, ctx->table.table);

	goto first;

	for (;;) {
		do {
			/* T <- I*Key2, using the optimization
			 * discussed in the specification */
			be128_xor(&s.t, &s.t,
				  &ctx->table.mulinc[get_index128(iv)]);
			inc(iv);

first:
			lrw_round(&s, wdst, wsrc);

			wsrc += bs;
			wdst += bs;
		} while ((avail -= bs) >= bs);

		err = blkcipher_walk_done(d, w, avail);
		if (!(avail = w->nbytes))
			break;

		wsrc = w->src.virt.addr;
		wdst = w->dst.virt.addr;
	}

	return err;
}
