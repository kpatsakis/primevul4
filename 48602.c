static void seqiv_complete2(struct skcipher_givcrypt_request *req, int err)
{
	struct ablkcipher_request *subreq = skcipher_givcrypt_reqctx(req);
	struct crypto_ablkcipher *geniv;

	if (err == -EINPROGRESS)
		return;

	if (err)
		goto out;

	geniv = skcipher_givcrypt_reqtfm(req);
	memcpy(req->creq.info, subreq->info, crypto_ablkcipher_ivsize(geniv));

out:
	kfree(subreq->info);
}
