static void crypto_gcm_setkey_done(struct crypto_async_request *req, int err)
{
	struct crypto_gcm_setkey_result *result = req->data;

	if (err == -EINPROGRESS)
		return;

	result->err = err;
	complete(&result->completion);
}
