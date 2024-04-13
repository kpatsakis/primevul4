static int ctr_aes_set_key(struct crypto_tfm *tfm, const u8 *in_key,
			   unsigned int key_len)
{
	struct s390_aes_ctx *sctx = crypto_tfm_ctx(tfm);

	switch (key_len) {
	case 16:
		sctx->enc = KMCTR_AES_128_ENCRYPT;
		sctx->dec = KMCTR_AES_128_DECRYPT;
		break;
	case 24:
		sctx->enc = KMCTR_AES_192_ENCRYPT;
		sctx->dec = KMCTR_AES_192_DECRYPT;
		break;
	case 32:
		sctx->enc = KMCTR_AES_256_ENCRYPT;
		sctx->dec = KMCTR_AES_256_DECRYPT;
		break;
	}

	return aes_set_key(tfm, in_key, key_len);
}
