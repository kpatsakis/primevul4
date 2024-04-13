int shash_ahash_digest(struct ahash_request *req, struct shash_desc *desc)
{
	unsigned int nbytes = req->nbytes;
	struct scatterlist *sg;
	unsigned int offset;
	int err;

	if (nbytes &&
	    (sg = req->src, offset = sg->offset,
	     nbytes < min(sg->length, ((unsigned int)(PAGE_SIZE)) - offset))) {
		void *data;

		data = kmap_atomic(sg_page(sg));
		err = crypto_shash_digest(desc, data + offset, nbytes,
					  req->result);
		kunmap_atomic(data);
		crypto_yield(desc->flags);
	} else
		err = crypto_shash_init(desc) ?:
		      shash_ahash_finup(req, desc);

	return err;
}
