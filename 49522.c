static int ecb_arc4_crypt(struct blkcipher_desc *desc, struct scatterlist *dst,
			  struct scatterlist *src, unsigned int nbytes)
{
	struct arc4_ctx *ctx = crypto_blkcipher_ctx(desc->tfm);
	struct blkcipher_walk walk;
	int err;

	blkcipher_walk_init(&walk, dst, src, nbytes);

	err = blkcipher_walk_virt(desc, &walk);

	while (walk.nbytes > 0) {
		u8 *wsrc = walk.src.virt.addr;
		u8 *wdst = walk.dst.virt.addr;

		arc4_crypt(ctx, wdst, wsrc, walk.nbytes);

		err = blkcipher_walk_done(desc, &walk, 0);
	}

	return err;
}
