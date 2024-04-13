static int hmac_update(struct shash_desc *pdesc,
		       const u8 *data, unsigned int nbytes)
{
	struct shash_desc *desc = shash_desc_ctx(pdesc);

	desc->flags = pdesc->flags & CRYPTO_TFM_REQ_MAY_SLEEP;

	return crypto_shash_update(desc, data, nbytes);
}
