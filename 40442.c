static int crypto_ahash_op(struct ahash_request *req,
			   int (*op)(struct ahash_request *))
{
	struct crypto_ahash *tfm = crypto_ahash_reqtfm(req);
	unsigned long alignmask = crypto_ahash_alignmask(tfm);

	if ((unsigned long)req->result & alignmask)
		return ahash_op_unaligned(req, op);

	return op(req);
}
