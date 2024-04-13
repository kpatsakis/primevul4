static int TSS_sha1(const unsigned char *data, unsigned int datalen,
		    unsigned char *digest)
{
	struct sdesc *sdesc;
	int ret;

	sdesc = init_sdesc(hashalg);
	if (IS_ERR(sdesc)) {
		pr_info("trusted_key: can't alloc %s\n", hash_alg);
		return PTR_ERR(sdesc);
	}

	ret = crypto_shash_digest(&sdesc->shash, data, datalen, digest);
	kfree(sdesc);
	return ret;
}
