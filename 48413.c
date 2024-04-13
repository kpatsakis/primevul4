static int crypto_ctr_crypt_inplace(struct blkcipher_walk *walk,
				    struct crypto_cipher *tfm)
{
	void (*fn)(struct crypto_tfm *, u8 *, const u8 *) =
		   crypto_cipher_alg(tfm)->cia_encrypt;
	unsigned int bsize = crypto_cipher_blocksize(tfm);
	unsigned long alignmask = crypto_cipher_alignmask(tfm);
	unsigned int nbytes = walk->nbytes;
	u8 *ctrblk = walk->iv;
	u8 *src = walk->src.virt.addr;
	u8 tmp[bsize + alignmask];
	u8 *keystream = PTR_ALIGN(tmp + 0, alignmask + 1);

	do {
		/* create keystream */
		fn(crypto_cipher_tfm(tfm), keystream, ctrblk);
		crypto_xor(src, keystream, bsize);

		/* increment counter in counterblock */
		crypto_inc(ctrblk, bsize);

		src += bsize;
	} while ((nbytes -= bsize) >= bsize);

	return nbytes;
}
