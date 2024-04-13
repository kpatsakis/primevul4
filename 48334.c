static void get_data_to_compute(struct crypto_cipher *tfm,
			       struct crypto_ccm_req_priv_ctx *pctx,
			       struct scatterlist *sg, unsigned int len)
{
	struct scatter_walk walk;
	u8 *data_src;
	int n;

	scatterwalk_start(&walk, sg);

	while (len) {
		n = scatterwalk_clamp(&walk, len);
		if (!n) {
			scatterwalk_start(&walk, sg_next(walk.sg));
			n = scatterwalk_clamp(&walk, len);
		}
		data_src = scatterwalk_map(&walk);

		compute_mac(tfm, data_src, n, pctx);
		len -= n;

		scatterwalk_unmap(data_src);
		scatterwalk_advance(&walk, n);
		scatterwalk_done(&walk, 0, len);
		if (len)
			crypto_yield(pctx->flags);
	}

	/* any leftover needs padding and then encrypted */
	if (pctx->ilen) {
		int padlen;
		u8 *odata = pctx->odata;
		u8 *idata = pctx->idata;

		padlen = 16 - pctx->ilen;
		memset(idata + pctx->ilen, 0, padlen);
		crypto_xor(odata, idata, 16);
		crypto_cipher_encrypt_one(tfm, odata, odata);
		pctx->ilen = 0;
	}
}
