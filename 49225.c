static int __driver_rfc4106_encrypt(struct aead_request *req)
{
	u8 one_entry_in_sg = 0;
	u8 *src, *dst, *assoc;
	__be32 counter = cpu_to_be32(1);
	struct crypto_aead *tfm = crypto_aead_reqtfm(req);
	struct aesni_rfc4106_gcm_ctx *ctx = aesni_rfc4106_gcm_ctx_get(tfm);
	void *aes_ctx = &(ctx->aes_key_expanded);
	unsigned long auth_tag_len = crypto_aead_authsize(tfm);
	u8 iv_tab[16+AESNI_ALIGN];
	u8* iv = (u8 *) PTR_ALIGN((u8 *)iv_tab, AESNI_ALIGN);
	struct scatter_walk src_sg_walk;
	struct scatter_walk assoc_sg_walk;
	struct scatter_walk dst_sg_walk;
	unsigned int i;

	/* Assuming we are supporting rfc4106 64-bit extended */
	/* sequence numbers We need to have the AAD length equal */
	/* to 8 or 12 bytes */
	if (unlikely(req->assoclen != 8 && req->assoclen != 12))
		return -EINVAL;
	/* IV below built */
	for (i = 0; i < 4; i++)
		*(iv+i) = ctx->nonce[i];
	for (i = 0; i < 8; i++)
		*(iv+4+i) = req->iv[i];
	*((__be32 *)(iv+12)) = counter;

	if ((sg_is_last(req->src)) && (sg_is_last(req->assoc))) {
		one_entry_in_sg = 1;
		scatterwalk_start(&src_sg_walk, req->src);
		scatterwalk_start(&assoc_sg_walk, req->assoc);
		src = scatterwalk_map(&src_sg_walk);
		assoc = scatterwalk_map(&assoc_sg_walk);
		dst = src;
		if (unlikely(req->src != req->dst)) {
			scatterwalk_start(&dst_sg_walk, req->dst);
			dst = scatterwalk_map(&dst_sg_walk);
		}

	} else {
		/* Allocate memory for src, dst, assoc */
		src = kmalloc(req->cryptlen + auth_tag_len + req->assoclen,
			GFP_ATOMIC);
		if (unlikely(!src))
			return -ENOMEM;
		assoc = (src + req->cryptlen + auth_tag_len);
		scatterwalk_map_and_copy(src, req->src, 0, req->cryptlen, 0);
		scatterwalk_map_and_copy(assoc, req->assoc, 0,
					req->assoclen, 0);
		dst = src;
	}

	aesni_gcm_enc_tfm(aes_ctx, dst, src, (unsigned long)req->cryptlen, iv,
		ctx->hash_subkey, assoc, (unsigned long)req->assoclen, dst
		+ ((unsigned long)req->cryptlen), auth_tag_len);

	/* The authTag (aka the Integrity Check Value) needs to be written
	 * back to the packet. */
	if (one_entry_in_sg) {
		if (unlikely(req->src != req->dst)) {
			scatterwalk_unmap(dst);
			scatterwalk_done(&dst_sg_walk, 0, 0);
		}
		scatterwalk_unmap(src);
		scatterwalk_unmap(assoc);
		scatterwalk_done(&src_sg_walk, 0, 0);
		scatterwalk_done(&assoc_sg_walk, 0, 0);
	} else {
		scatterwalk_map_and_copy(dst, req->dst, 0,
			req->cryptlen + auth_tag_len, 1);
		kfree(src);
	}
	return 0;
}
