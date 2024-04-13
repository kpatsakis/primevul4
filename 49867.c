static int hash_hw_final(struct ahash_request *req)
{
	int ret = 0;
	struct crypto_ahash *tfm = crypto_ahash_reqtfm(req);
	struct hash_ctx *ctx = crypto_ahash_ctx(tfm);
	struct hash_req_ctx *req_ctx = ahash_request_ctx(req);
	struct hash_device_data *device_data;
	u8 digest[SHA256_DIGEST_SIZE];

	ret = hash_get_device_data(ctx, &device_data);
	if (ret)
		return ret;

	dev_dbg(device_data->dev, "%s: (ctx=0x%x)!\n", __func__, (u32) ctx);

	if (req_ctx->updated) {
		ret = hash_resume_state(device_data, &device_data->state);

		if (ret) {
			dev_err(device_data->dev,
				"%s: hash_resume_state() failed!\n", __func__);
			goto out;
		}
	} else if (req->nbytes == 0 && ctx->keylen == 0) {
		u8 zero_hash[SHA256_DIGEST_SIZE];
		u32 zero_hash_size = 0;
		bool zero_digest = false;
		/**
		 * Use a pre-calculated empty message digest
		 * (workaround since hw return zeroes, hw bug!?)
		 */
		ret = get_empty_message_digest(device_data, &zero_hash[0],
				&zero_hash_size, &zero_digest);
		if (!ret && likely(zero_hash_size == ctx->digestsize) &&
		    zero_digest) {
			memcpy(req->result, &zero_hash[0], ctx->digestsize);
			goto out;
		} else if (!ret && !zero_digest) {
			dev_dbg(device_data->dev,
				"%s: HMAC zero msg with key, continue...\n",
				__func__);
		} else {
			dev_err(device_data->dev,
				"%s: ret=%d, or wrong digest size? %s\n",
				__func__, ret,
				zero_hash_size == ctx->digestsize ?
				"true" : "false");
			/* Return error */
			goto out;
		}
	} else if (req->nbytes == 0 && ctx->keylen > 0) {
		dev_err(device_data->dev, "%s: Empty message with keylength > 0, NOT supported\n",
			__func__);
		goto out;
	}

	if (!req_ctx->updated) {
		ret = init_hash_hw(device_data, ctx);
		if (ret) {
			dev_err(device_data->dev,
				"%s: init_hash_hw() failed!\n", __func__);
			goto out;
		}
	}

	if (req_ctx->state.index) {
		hash_messagepad(device_data, req_ctx->state.buffer,
				req_ctx->state.index);
	} else {
		HASH_SET_DCAL;
		while (readl(&device_data->base->str) & HASH_STR_DCAL_MASK)
			cpu_relax();
	}

	if (ctx->config.oper_mode == HASH_OPER_MODE_HMAC && ctx->key) {
		unsigned int keylen = ctx->keylen;
		u8 *key = ctx->key;

		dev_dbg(device_data->dev, "%s: keylen: %d\n",
			__func__, ctx->keylen);
		hash_hw_write_key(device_data, key, keylen);
	}

	hash_get_digest(device_data, digest, ctx->config.algorithm);
	memcpy(req->result, digest, ctx->digestsize);

out:
	release_hash_device(device_data);

	/**
	 * Allocated in setkey, and only used in HMAC.
	 */
	kfree(ctx->key);

	return ret;
}
