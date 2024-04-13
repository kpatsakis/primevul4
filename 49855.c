static int get_empty_message_digest(
		struct hash_device_data *device_data,
		u8 *zero_hash, u32 *zero_hash_size, bool *zero_digest)
{
	int ret = 0;
	struct hash_ctx *ctx = device_data->current_ctx;
	*zero_digest = false;

	/**
	 * Caller responsible for ctx != NULL.
	 */

	if (HASH_OPER_MODE_HASH == ctx->config.oper_mode) {
		if (HASH_ALGO_SHA1 == ctx->config.algorithm) {
			memcpy(zero_hash, &zero_message_hash_sha1[0],
			       SHA1_DIGEST_SIZE);
			*zero_hash_size = SHA1_DIGEST_SIZE;
			*zero_digest = true;
		} else if (HASH_ALGO_SHA256 ==
				ctx->config.algorithm) {
			memcpy(zero_hash, &zero_message_hash_sha256[0],
			       SHA256_DIGEST_SIZE);
			*zero_hash_size = SHA256_DIGEST_SIZE;
			*zero_digest = true;
		} else {
			dev_err(device_data->dev, "%s: Incorrect algorithm!\n",
				__func__);
			ret = -EINVAL;
			goto out;
		}
	} else if (HASH_OPER_MODE_HMAC == ctx->config.oper_mode) {
		if (!ctx->keylen) {
			if (HASH_ALGO_SHA1 == ctx->config.algorithm) {
				memcpy(zero_hash, &zero_message_hmac_sha1[0],
				       SHA1_DIGEST_SIZE);
				*zero_hash_size = SHA1_DIGEST_SIZE;
				*zero_digest = true;
			} else if (HASH_ALGO_SHA256 == ctx->config.algorithm) {
				memcpy(zero_hash, &zero_message_hmac_sha256[0],
				       SHA256_DIGEST_SIZE);
				*zero_hash_size = SHA256_DIGEST_SIZE;
				*zero_digest = true;
			} else {
				dev_err(device_data->dev, "%s: Incorrect algorithm!\n",
					__func__);
				ret = -EINVAL;
				goto out;
			}
		} else {
			dev_dbg(device_data->dev,
				"%s: Continue hash calculation, since hmac key available\n",
				__func__);
		}
	}
out:

	return ret;
}
