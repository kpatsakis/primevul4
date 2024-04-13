static int calculate_nss_hash(
	struct crypto_instance *instance,
	const unsigned char *buf,
	const size_t buf_len,
	unsigned char *hash)
{
	PK11Context*	hash_context = NULL;
	SECItem		hash_param;
	unsigned int	hash_tmp_outlen = 0;
	unsigned char	hash_block[hash_block_len[instance->crypto_hash_type]];
	int		err = -1;

	/* Now do the digest */
	hash_param.type = siBuffer;
	hash_param.data = 0;
	hash_param.len = 0;

	hash_context = PK11_CreateContextBySymKey(hash_to_nss[instance->crypto_hash_type],
						 CKA_SIGN,
						 instance->nss_sym_key_sign,
						 &hash_param);

	if (!hash_context) {
		log_printf(instance->log_level_security,
			   "PK11_CreateContext failed (hash) hash_type=%d (err %d)",
			   (int)hash_to_nss[instance->crypto_hash_type],
			   PR_GetError());
		goto out;
	}

	if (PK11_DigestBegin(hash_context) != SECSuccess) {
		log_printf(instance->log_level_security,
			   "PK11_DigestBegin failed (hash) hash_type=%d (err %d)",
			   (int)hash_to_nss[instance->crypto_hash_type],
			   PR_GetError());
		goto out;
	}

	if (PK11_DigestOp(hash_context,
			  buf,
			  buf_len) != SECSuccess) {
		log_printf(instance->log_level_security,
			   "PK11_DigestOp failed (hash) hash_type=%d (err %d)",
			   (int)hash_to_nss[instance->crypto_hash_type],
			   PR_GetError());
		goto out;
	}

	if (PK11_DigestFinal(hash_context,
			     hash_block,
			     &hash_tmp_outlen,
			     hash_block_len[instance->crypto_hash_type]) != SECSuccess) {
		log_printf(instance->log_level_security,
			   "PK11_DigestFinale failed (hash) hash_type=%d (err %d)",
			   (int)hash_to_nss[instance->crypto_hash_type],
			   PR_GetError());
		goto out;
	}

	memcpy(hash, hash_block, hash_len[instance->crypto_hash_type]);
	err = 0;

out:
	if (hash_context) {
		PK11_DestroyContext(hash_context, PR_TRUE);
	}

	return err;
}
