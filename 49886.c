static int init_hash_hw(struct hash_device_data *device_data,
			struct hash_ctx *ctx)
{
	int ret = 0;

	ret = hash_setconfiguration(device_data, &ctx->config);
	if (ret) {
		dev_err(device_data->dev, "%s: hash_setconfiguration() failed!\n",
			__func__);
		return ret;
	}

	hash_begin(device_data, ctx);

	if (ctx->config.oper_mode == HASH_OPER_MODE_HMAC)
		hash_hw_write_key(device_data, ctx->key, ctx->keylen);

	return ret;
}
