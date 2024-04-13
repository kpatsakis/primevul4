rfc4106_set_hash_subkey_done(struct crypto_async_request *req, int err)
{
	struct aesni_gcm_set_hash_subkey_result *result = req->data;

	if (err == -EINPROGRESS)
		return;
	result->err = err;
	complete(&result->completion);
}
