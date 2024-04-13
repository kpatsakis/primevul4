static int crypto_cbc_encrypt_segment(struct blkcipher_desc *desc,
				      struct blkcipher_walk *walk,
				      struct crypto_cipher *tfm)
{
	void (*fn)(struct crypto_tfm *, u8 *, const u8 *) =
		crypto_cipher_alg(tfm)->cia_encrypt;
	int bsize = crypto_cipher_blocksize(tfm);
	unsigned int nbytes = walk->nbytes;
	u8 *src = walk->src.virt.addr;
	u8 *dst = walk->dst.virt.addr;
	u8 *iv = walk->iv;

	do {
		crypto_xor(iv, src, bsize);
		fn(crypto_cipher_tfm(tfm), dst, iv);
		memcpy(iv, dst, bsize);

		src += bsize;
		dst += bsize;
	} while ((nbytes -= bsize) >= bsize);

	return nbytes;
}
