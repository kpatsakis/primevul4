static int hash_dma_final(struct ahash_request *req)
{
	int ret = 0;
	struct crypto_ahash *tfm = crypto_ahash_reqtfm(req);
	struct hash_ctx *ctx = crypto_ahash_ctx(tfm);
	struct hash_req_ctx *req_ctx = ahash_request_ctx(req);
	struct hash_device_data *device_data;
	u8 digest[SHA256_DIGEST_SIZE];
	int bytes_written = 0;

	ret = hash_get_device_data(ctx, &device_data);
	if (ret)
		return ret;

	dev_dbg(device_data->dev, "%s: (ctx=0x%x)!\n", __func__, (u32) ctx);

	if (req_ctx->updated) {
		ret = hash_resume_state(device_data, &device_data->state);

		if (ret) {
			dev_err(device_data->dev, "%s: hash_resume_state() failed!\n",
				__func__);
			goto out;
		}
	}

	if (!req_ctx->updated) {
		ret = hash_setconfiguration(device_data, &ctx->config);
		if (ret) {
			dev_err(device_data->dev,
				"%s: hash_setconfiguration() failed!\n",
				__func__);
			goto out;
		}

		/* Enable DMA input */
		if (hash_mode != HASH_MODE_DMA || !req_ctx->dma_mode) {
			HASH_CLEAR_BITS(&device_data->base->cr,
					HASH_CR_DMAE_MASK);
		} else {
			HASH_SET_BITS(&device_data->base->cr,
				      HASH_CR_DMAE_MASK);
			HASH_SET_BITS(&device_data->base->cr,
				      HASH_CR_PRIVN_MASK);
		}

		HASH_INITIALIZE;

		if (ctx->config.oper_mode == HASH_OPER_MODE_HMAC)
			hash_hw_write_key(device_data, ctx->key, ctx->keylen);

		/* Number of bits in last word = (nbytes * 8) % 32 */
		HASH_SET_NBLW((req->nbytes * 8) % 32);
		req_ctx->updated = 1;
	}

	/* Store the nents in the dma struct. */
	ctx->device->dma.nents = hash_get_nents(req->src, req->nbytes, NULL);
	if (!ctx->device->dma.nents) {
		dev_err(device_data->dev, "%s: ctx->device->dma.nents = 0\n",
			__func__);
		ret = ctx->device->dma.nents;
		goto out;
	}

	bytes_written = hash_dma_write(ctx, req->src, req->nbytes);
	if (bytes_written != req->nbytes) {
		dev_err(device_data->dev, "%s: hash_dma_write() failed!\n",
			__func__);
		ret = bytes_written;
		goto out;
	}

	wait_for_completion(&ctx->device->dma.complete);
	hash_dma_done(ctx);

	while (readl(&device_data->base->str) & HASH_STR_DCAL_MASK)
		cpu_relax();

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
