static int crypto_pcbc_decrypt_segment(struct blkcipher_desc *desc,
				       struct blkcipher_walk *walk,
				       struct crypto_cipher *tfm)
{
	void (*fn)(struct crypto_tfm *, u8 *, const u8 *) =
		crypto_cipher_alg(tfm)->cia_decrypt;
	int bsize = crypto_cipher_blocksize(tfm);
	unsigned int nbytes = walk->nbytes;
	u8 *src = walk->src.virt.addr;
	u8 *dst = walk->dst.virt.addr;
	u8 *iv = walk->iv;

	do {
		fn(crypto_cipher_tfm(tfm), dst, src);
		crypto_xor(dst, iv, bsize);
		memcpy(iv, src, bsize);
		crypto_xor(iv, dst, bsize);

		src += bsize;
		dst += bsize;
	} while ((nbytes -= bsize) >= bsize);

	memcpy(walk->iv, iv, bsize);

	return nbytes;
}
