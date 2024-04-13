static void ahash_op_unaligned_done(struct crypto_async_request *req, int err)
{
	struct ahash_request *areq = req->data;
	struct ahash_request_priv *priv = areq->priv;
	crypto_completion_t complete = priv->complete;
	void *data = priv->data;

	ahash_op_unaligned_finish(areq, err);

	complete(data, err);
}
