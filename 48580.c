static void pcrypt_aead_serial(struct padata_priv *padata)
{
	struct pcrypt_request *preq = pcrypt_padata_request(padata);
	struct aead_request *req = pcrypt_request_ctx(preq);

	aead_request_complete(req->base.data, padata->info);
}
