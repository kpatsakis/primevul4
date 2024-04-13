static void ahash_def_finup_done2(struct crypto_async_request *req, int err)
{
	struct ahash_request *areq = req->data;
	struct ahash_request_priv *priv = areq->priv;
	crypto_completion_t complete = priv->complete;
	void *data = priv->data;

	ahash_def_finup_finish2(areq, err);

	complete(data, err);
}
