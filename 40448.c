int crypto_hash_walk_first(struct ahash_request *req,
			   struct crypto_hash_walk *walk)
{
	walk->total = req->nbytes;

	if (!walk->total)
		return 0;

	walk->alignmask = crypto_ahash_alignmask(crypto_ahash_reqtfm(req));
	walk->sg = req->src;
	walk->flags = req->base.flags;

	return hash_walk_new_entry(walk);
}
