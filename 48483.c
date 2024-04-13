static int crypto_rfc4543_decrypt(struct aead_request *req)
{
	int err;

	if (req->src != req->dst) {
		err = crypto_rfc4543_copy_src_to_dst(req, false);
		if (err)
			return err;
	}

	req = crypto_rfc4543_crypt(req, false);

	return crypto_aead_decrypt(req);
}
