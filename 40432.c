static int ahash_op_unaligned(struct ahash_request *req,
			      int (*op)(struct ahash_request *))
{
	struct crypto_ahash *tfm = crypto_ahash_reqtfm(req);
	unsigned long alignmask = crypto_ahash_alignmask(tfm);
	unsigned int ds = crypto_ahash_digestsize(tfm);
	struct ahash_request_priv *priv;
	int err;

	priv = kmalloc(sizeof(*priv) + ahash_align_buffer_size(ds, alignmask),
		       (req->base.flags & CRYPTO_TFM_REQ_MAY_SLEEP) ?
		       GFP_KERNEL : GFP_ATOMIC);
	if (!priv)
		return -ENOMEM;

	priv->result = req->result;
	priv->complete = req->base.complete;
	priv->data = req->base.data;

	req->result = PTR_ALIGN((u8 *)priv->ubuf, alignmask + 1);
	req->base.complete = ahash_op_unaligned_done;
	req->base.data = req;
	req->priv = priv;

	err = op(req);
	ahash_op_unaligned_finish(req, err);

	return err;
}
