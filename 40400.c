static int setkey_unaligned(struct crypto_ablkcipher *tfm, const u8 *key,
			    unsigned int keylen)
{
	struct ablkcipher_alg *cipher = crypto_ablkcipher_alg(tfm);
	unsigned long alignmask = crypto_ablkcipher_alignmask(tfm);
	int ret;
	u8 *buffer, *alignbuffer;
	unsigned long absize;

	absize = keylen + alignmask;
	buffer = kmalloc(absize, GFP_ATOMIC);
	if (!buffer)
		return -ENOMEM;

	alignbuffer = (u8 *)ALIGN((unsigned long)buffer, alignmask + 1);
	memcpy(alignbuffer, key, keylen);
	ret = cipher->setkey(tfm, alignbuffer, keylen);
	memset(alignbuffer, 0, keylen);
	kfree(buffer);
	return ret;
}
