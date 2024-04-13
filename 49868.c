int hash_hw_update(struct ahash_request *req)
{
	int ret = 0;
	u8 index = 0;
	u8 *buffer;
	struct hash_device_data *device_data;
	u8 *data_buffer;
	struct crypto_ahash *tfm = crypto_ahash_reqtfm(req);
	struct hash_ctx *ctx = crypto_ahash_ctx(tfm);
	struct hash_req_ctx *req_ctx = ahash_request_ctx(req);
	struct crypto_hash_walk walk;
	int msg_length = crypto_hash_walk_first(req, &walk);

	/* Empty message ("") is correct indata */
	if (msg_length == 0)
		return ret;

	index = req_ctx->state.index;
	buffer = (u8 *)req_ctx->state.buffer;

	/* Check if ctx->state.length + msg_length
	   overflows */
	if (msg_length > (req_ctx->state.length.low_word + msg_length) &&
	    HASH_HIGH_WORD_MAX_VAL == req_ctx->state.length.high_word) {
		pr_err("%s: HASH_MSG_LENGTH_OVERFLOW!\n", __func__);
		return -EPERM;
	}

	ret = hash_get_device_data(ctx, &device_data);
	if (ret)
		return ret;

	/* Main loop */
	while (0 != msg_length) {
		data_buffer = walk.data;
		ret = hash_process_data(device_data, ctx, req_ctx, msg_length,
				data_buffer, buffer, &index);

		if (ret) {
			dev_err(device_data->dev, "%s: hash_internal_hw_update() failed!\n",
				__func__);
			goto out;
		}

		msg_length = crypto_hash_walk_done(&walk, 0);
	}

	req_ctx->state.index = index;
	dev_dbg(device_data->dev, "%s: indata length=%d, bin=%d\n",
		__func__, req_ctx->state.index, req_ctx->state.bit_index);

out:
	release_hash_device(device_data);

	return ret;
}
