static int crypt(struct blkcipher_desc *d,
		 struct blkcipher_walk *w, struct priv *ctx,
		 void (*tw)(struct crypto_tfm *, u8 *, const u8 *),
		 void (*fn)(struct crypto_tfm *, u8 *, const u8 *))
{
	int err;
	unsigned int avail;
	const int bs = XTS_BLOCK_SIZE;
	struct sinfo s = {
		.tfm = crypto_cipher_tfm(ctx->child),
		.fn = fn
	};
	u8 *wsrc;
	u8 *wdst;

	err = blkcipher_walk_virt(d, w);
	if (!w->nbytes)
		return err;

	s.t = (be128 *)w->iv;
	avail = w->nbytes;

	wsrc = w->src.virt.addr;
	wdst = w->dst.virt.addr;

	/* calculate first value of T */
	tw(crypto_cipher_tfm(ctx->tweak), w->iv, w->iv);

	goto first;

	for (;;) {
		do {
			gf128mul_x_ble(s.t, s.t);

first:
			xts_round(&s, wdst, wsrc);

			wsrc += bs;
			wdst += bs;
		} while ((avail -= bs) >= bs);

		err = blkcipher_walk_done(d, w, avail);
		if (!w->nbytes)
			break;

		avail = w->nbytes;

		wsrc = w->src.virt.addr;
		wdst = w->dst.virt.addr;
	}

	return err;
}
