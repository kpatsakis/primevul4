static int skcipher_walk_skcipher(struct skcipher_walk *walk,
				  struct skcipher_request *req)
{
	struct crypto_skcipher *tfm = crypto_skcipher_reqtfm(req);

	scatterwalk_start(&walk->in, req->src);
	scatterwalk_start(&walk->out, req->dst);

	walk->total = req->cryptlen;
	walk->iv = req->iv;
	walk->oiv = req->iv;

	walk->flags &= ~SKCIPHER_WALK_SLEEP;
	walk->flags |= req->base.flags & CRYPTO_TFM_REQ_MAY_SLEEP ?
		       SKCIPHER_WALK_SLEEP : 0;

	walk->blocksize = crypto_skcipher_blocksize(tfm);
	walk->stride = crypto_skcipher_walksize(tfm);
	walk->ivsize = crypto_skcipher_ivsize(tfm);
	walk->alignmask = crypto_skcipher_alignmask(tfm);

	return skcipher_walk_first(walk);
}
