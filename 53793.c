static int TSS_checkhmac1(unsigned char *buffer,
			  const uint32_t command,
			  const unsigned char *ononce,
			  const unsigned char *key,
			  unsigned int keylen, ...)
{
	uint32_t bufsize;
	uint16_t tag;
	uint32_t ordinal;
	uint32_t result;
	unsigned char *enonce;
	unsigned char *continueflag;
	unsigned char *authdata;
	unsigned char testhmac[SHA1_DIGEST_SIZE];
	unsigned char paramdigest[SHA1_DIGEST_SIZE];
	struct sdesc *sdesc;
	unsigned int dlen;
	unsigned int dpos;
	va_list argp;
	int ret;

	bufsize = LOAD32(buffer, TPM_SIZE_OFFSET);
	tag = LOAD16(buffer, 0);
	ordinal = command;
	result = LOAD32N(buffer, TPM_RETURN_OFFSET);
	if (tag == TPM_TAG_RSP_COMMAND)
		return 0;
	if (tag != TPM_TAG_RSP_AUTH1_COMMAND)
		return -EINVAL;
	authdata = buffer + bufsize - SHA1_DIGEST_SIZE;
	continueflag = authdata - 1;
	enonce = continueflag - TPM_NONCE_SIZE;

	sdesc = init_sdesc(hashalg);
	if (IS_ERR(sdesc)) {
		pr_info("trusted_key: can't alloc %s\n", hash_alg);
		return PTR_ERR(sdesc);
	}
	ret = crypto_shash_init(&sdesc->shash);
	if (ret < 0)
		goto out;
	ret = crypto_shash_update(&sdesc->shash, (const u8 *)&result,
				  sizeof result);
	if (ret < 0)
		goto out;
	ret = crypto_shash_update(&sdesc->shash, (const u8 *)&ordinal,
				  sizeof ordinal);
	if (ret < 0)
		goto out;
	va_start(argp, keylen);
	for (;;) {
		dlen = va_arg(argp, unsigned int);
		if (dlen == 0)
			break;
		dpos = va_arg(argp, unsigned int);
		ret = crypto_shash_update(&sdesc->shash, buffer + dpos, dlen);
		if (ret < 0)
			break;
	}
	va_end(argp);
	if (!ret)
		ret = crypto_shash_final(&sdesc->shash, paramdigest);
	if (ret < 0)
		goto out;

	ret = TSS_rawhmac(testhmac, key, keylen, SHA1_DIGEST_SIZE, paramdigest,
			  TPM_NONCE_SIZE, enonce, TPM_NONCE_SIZE, ononce,
			  1, continueflag, 0, 0);
	if (ret < 0)
		goto out;

	if (memcmp(testhmac, authdata, SHA1_DIGEST_SIZE))
		ret = -EINVAL;
out:
	kfree(sdesc);
	return ret;
}
