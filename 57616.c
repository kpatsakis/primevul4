static int skcipher_walk_aead_common(struct skcipher_walk *walk,
				     struct aead_request *req, bool atomic)
{
	struct crypto_aead *tfm = crypto_aead_reqtfm(req);
	int err;

	walk->flags &= ~SKCIPHER_WALK_PHYS;

	scatterwalk_start(&walk->in, req->src);
	scatterwalk_start(&walk->out, req->dst);

	scatterwalk_copychunks(NULL, &walk->in, req->assoclen, 2);
	scatterwalk_copychunks(NULL, &walk->out, req->assoclen, 2);

	walk->iv = req->iv;
	walk->oiv = req->iv;

	if (req->base.flags & CRYPTO_TFM_REQ_MAY_SLEEP)
		walk->flags |= SKCIPHER_WALK_SLEEP;
	else
		walk->flags &= ~SKCIPHER_WALK_SLEEP;

	walk->blocksize = crypto_aead_blocksize(tfm);
	walk->stride = crypto_aead_chunksize(tfm);
	walk->ivsize = crypto_aead_ivsize(tfm);
	walk->alignmask = crypto_aead_alignmask(tfm);

	err = skcipher_walk_first(walk);

	if (atomic)
		walk->flags &= ~SKCIPHER_WALK_SLEEP;

	return err;
}
