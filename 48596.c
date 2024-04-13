static void seqiv_aead_complete2(struct aead_givcrypt_request *req, int err)
{
	struct aead_request *subreq = aead_givcrypt_reqctx(req);
	struct crypto_aead *geniv;

	if (err == -EINPROGRESS)
		return;

	if (err)
		goto out;

	geniv = aead_givcrypt_reqtfm(req);
	memcpy(req->areq.iv, subreq->iv, crypto_aead_ivsize(geniv));

out:
	kfree(subreq->iv);
}
