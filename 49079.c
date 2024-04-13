static int ctr_aes_crypt(struct blkcipher_desc *desc, long func,
			 struct s390_aes_ctx *sctx, struct blkcipher_walk *walk)
{
	int ret = blkcipher_walk_virt_block(desc, walk, AES_BLOCK_SIZE);
	unsigned int n, nbytes;
	u8 buf[AES_BLOCK_SIZE], ctrbuf[AES_BLOCK_SIZE];
	u8 *out, *in, *ctrptr = ctrbuf;

	if (!walk->nbytes)
		return ret;

	if (spin_trylock(&ctrblk_lock))
		ctrptr = ctrblk;

	memcpy(ctrptr, walk->iv, AES_BLOCK_SIZE);
	while ((nbytes = walk->nbytes) >= AES_BLOCK_SIZE) {
		out = walk->dst.virt.addr;
		in = walk->src.virt.addr;
		while (nbytes >= AES_BLOCK_SIZE) {
			if (ctrptr == ctrblk)
				n = __ctrblk_init(ctrptr, nbytes);
			else
				n = AES_BLOCK_SIZE;
			ret = crypt_s390_kmctr(func, sctx->key, out, in,
					       n, ctrptr);
			if (ret < 0 || ret != n) {
				if (ctrptr == ctrblk)
					spin_unlock(&ctrblk_lock);
				return -EIO;
			}
			if (n > AES_BLOCK_SIZE)
				memcpy(ctrptr, ctrptr + n - AES_BLOCK_SIZE,
				       AES_BLOCK_SIZE);
			crypto_inc(ctrptr, AES_BLOCK_SIZE);
			out += n;
			in += n;
			nbytes -= n;
		}
		ret = blkcipher_walk_done(desc, walk, nbytes);
	}
	if (ctrptr == ctrblk) {
		if (nbytes)
			memcpy(ctrbuf, ctrptr, AES_BLOCK_SIZE);
		else
			memcpy(walk->iv, ctrptr, AES_BLOCK_SIZE);
		spin_unlock(&ctrblk_lock);
	} else {
		if (!nbytes)
			memcpy(walk->iv, ctrptr, AES_BLOCK_SIZE);
	}
	/*
	 * final block may be < AES_BLOCK_SIZE, copy only nbytes
	 */
	if (nbytes) {
		out = walk->dst.virt.addr;
		in = walk->src.virt.addr;
		ret = crypt_s390_kmctr(func, sctx->key, buf, in,
				       AES_BLOCK_SIZE, ctrbuf);
		if (ret < 0 || ret != AES_BLOCK_SIZE)
			return -EIO;
		memcpy(out, buf, nbytes);
		crypto_inc(ctrbuf, AES_BLOCK_SIZE);
		ret = blkcipher_walk_done(desc, walk, 0);
		memcpy(walk->iv, ctrbuf, AES_BLOCK_SIZE);
	}

	return ret;
}
