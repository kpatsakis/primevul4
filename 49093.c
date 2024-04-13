static int xts_aes_crypt(struct blkcipher_desc *desc, long func,
			 struct s390_xts_ctx *xts_ctx,
			 struct blkcipher_walk *walk)
{
	unsigned int offset = (xts_ctx->key_len >> 1) & 0x10;
	int ret = blkcipher_walk_virt(desc, walk);
	unsigned int nbytes = walk->nbytes;
	unsigned int n;
	u8 *in, *out;
	struct pcc_param pcc_param;
	struct {
		u8 key[32];
		u8 init[16];
	} xts_param;

	if (!nbytes)
		goto out;

	memset(pcc_param.block, 0, sizeof(pcc_param.block));
	memset(pcc_param.bit, 0, sizeof(pcc_param.bit));
	memset(pcc_param.xts, 0, sizeof(pcc_param.xts));
	memcpy(pcc_param.tweak, walk->iv, sizeof(pcc_param.tweak));
	memcpy(pcc_param.key, xts_ctx->pcc_key, 32);
	ret = crypt_s390_pcc(func, &pcc_param.key[offset]);
	if (ret < 0)
		return -EIO;

	memcpy(xts_param.key, xts_ctx->key, 32);
	memcpy(xts_param.init, pcc_param.xts, 16);
	do {
		/* only use complete blocks */
		n = nbytes & ~(AES_BLOCK_SIZE - 1);
		out = walk->dst.virt.addr;
		in = walk->src.virt.addr;

		ret = crypt_s390_km(func, &xts_param.key[offset], out, in, n);
		if (ret < 0 || ret != n)
			return -EIO;

		nbytes &= AES_BLOCK_SIZE - 1;
		ret = blkcipher_walk_done(desc, walk, nbytes);
	} while ((nbytes = walk->nbytes));
out:
	return ret;
}
