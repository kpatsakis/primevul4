static void pcrypt_aead_dec(struct padata_priv *padata)
{
	struct pcrypt_request *preq = pcrypt_padata_request(padata);
	struct aead_request *req = pcrypt_request_ctx(preq);

	padata->info = crypto_aead_decrypt(req);

	if (padata->info == -EINPROGRESS)
		return;

	padata_do_serial(padata);
}
