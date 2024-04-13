static int cbc_aes_crypt(struct blkcipher_desc *desc, long func,
			 struct blkcipher_walk *walk)
{
	struct s390_aes_ctx *sctx = crypto_blkcipher_ctx(desc->tfm);
	int ret = blkcipher_walk_virt(desc, walk);
	unsigned int nbytes = walk->nbytes;
	struct {
		u8 iv[AES_BLOCK_SIZE];
		u8 key[AES_MAX_KEY_SIZE];
	} param;

	if (!nbytes)
		goto out;

	memcpy(param.iv, walk->iv, AES_BLOCK_SIZE);
	memcpy(param.key, sctx->key, sctx->key_len);
	do {
		/* only use complete blocks */
		unsigned int n = nbytes & ~(AES_BLOCK_SIZE - 1);
		u8 *out = walk->dst.virt.addr;
		u8 *in = walk->src.virt.addr;

		ret = crypt_s390_kmc(func, &param, out, in, n);
		if (ret < 0 || ret != n)
			return -EIO;

		nbytes &= AES_BLOCK_SIZE - 1;
		ret = blkcipher_walk_done(desc, walk, nbytes);
	} while ((nbytes = walk->nbytes));
	memcpy(walk->iv, param.iv, AES_BLOCK_SIZE);

out:
	return ret;
}
