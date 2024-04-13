int ablkcipher_walk_phys(struct ablkcipher_request *req,
			 struct ablkcipher_walk *walk)
{
	walk->blocksize = crypto_tfm_alg_blocksize(req->base.tfm);
	return ablkcipher_walk_first(req, walk);
}
