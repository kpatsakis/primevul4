static void ahash_op_unaligned_finish(struct ahash_request *req, int err)
{
	struct ahash_request_priv *priv = req->priv;

	if (err == -EINPROGRESS)
		return;

	if (!err)
		memcpy(priv->result, req->result,
		       crypto_ahash_digestsize(crypto_ahash_reqtfm(req)));

	kzfree(priv);
}
