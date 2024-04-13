static void ccm_calculate_auth_mac(struct aead_request *req, u8 mac[])
{
	struct crypto_aead *aead = crypto_aead_reqtfm(req);
	struct crypto_aes_ctx *ctx = crypto_aead_ctx(aead);
	struct __packed { __be16 l; __be32 h; u16 len; } ltag;
	struct scatter_walk walk;
	u32 len = req->assoclen;
	u32 macp = 0;

	/* prepend the AAD with a length tag */
	if (len < 0xff00) {
		ltag.l = cpu_to_be16(len);
		ltag.len = 2;
	} else  {
		ltag.l = cpu_to_be16(0xfffe);
		put_unaligned_be32(len, &ltag.h);
		ltag.len = 6;
	}

	ce_aes_ccm_auth_data(mac, (u8 *)&ltag, ltag.len, &macp, ctx->key_enc,
			     num_rounds(ctx));
	scatterwalk_start(&walk, req->assoc);

	do {
		u32 n = scatterwalk_clamp(&walk, len);
		u8 *p;

		if (!n) {
			scatterwalk_start(&walk, sg_next(walk.sg));
			n = scatterwalk_clamp(&walk, len);
		}
		p = scatterwalk_map(&walk);
		ce_aes_ccm_auth_data(mac, p, n, &macp, ctx->key_enc,
				     num_rounds(ctx));
		len -= n;

		scatterwalk_unmap(p);
		scatterwalk_advance(&walk, n);
		scatterwalk_done(&walk, 0, len);
	} while (len);
}
