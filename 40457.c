static inline int blkcipher_copy_iv(struct blkcipher_walk *walk,
				    struct crypto_blkcipher *tfm,
				    unsigned int alignmask)
{
	unsigned bs = walk->blocksize;
	unsigned int ivsize = crypto_blkcipher_ivsize(tfm);
	unsigned aligned_bs = ALIGN(bs, alignmask + 1);
	unsigned int size = aligned_bs * 2 + ivsize + max(aligned_bs, ivsize) -
			    (alignmask + 1);
	u8 *iv;

	size += alignmask & ~(crypto_tfm_ctx_alignment() - 1);
	walk->buffer = kmalloc(size, GFP_ATOMIC);
	if (!walk->buffer)
		return -ENOMEM;

	iv = (u8 *)ALIGN((unsigned long)walk->buffer, alignmask + 1);
	iv = blkcipher_get_spot(iv, bs) + aligned_bs;
	iv = blkcipher_get_spot(iv, bs) + aligned_bs;
	iv = blkcipher_get_spot(iv, ivsize);

	walk->iv = memcpy(iv, walk->iv, ivsize);
	return 0;
}
