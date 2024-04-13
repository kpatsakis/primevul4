static void pcrypt_aead_giv_serial(struct padata_priv *padata)
{
	struct pcrypt_request *preq = pcrypt_padata_request(padata);
	struct aead_givcrypt_request *req = pcrypt_request_ctx(preq);

	aead_request_complete(req->areq.base.data, padata->info);
}
