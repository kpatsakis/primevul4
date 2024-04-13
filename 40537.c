static int shash_compat_update(struct hash_desc *hdesc, struct scatterlist *sg,
			       unsigned int len)
{
	struct shash_desc **descp = crypto_hash_ctx(hdesc->tfm);
	struct shash_desc *desc = *descp;
	struct crypto_hash_walk walk;
	int nbytes;

	for (nbytes = crypto_hash_walk_first_compat(hdesc, &walk, sg, len);
	     nbytes > 0; nbytes = crypto_hash_walk_done(&walk, nbytes))
		nbytes = crypto_shash_update(desc, walk.data, nbytes);

	return nbytes;
}
