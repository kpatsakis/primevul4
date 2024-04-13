static int crypto_authenc_esn_setkey(struct crypto_aead *authenc_esn, const u8 *key,
				     unsigned int keylen)
{
	struct crypto_authenc_esn_ctx *ctx = crypto_aead_ctx(authenc_esn);
	struct crypto_ahash *auth = ctx->auth;
	struct crypto_ablkcipher *enc = ctx->enc;
	struct crypto_authenc_keys keys;
	int err = -EINVAL;

	if (crypto_authenc_extractkeys(&keys, key, keylen) != 0)
		goto badkey;

	crypto_ahash_clear_flags(auth, CRYPTO_TFM_REQ_MASK);
	crypto_ahash_set_flags(auth, crypto_aead_get_flags(authenc_esn) &
				     CRYPTO_TFM_REQ_MASK);
	err = crypto_ahash_setkey(auth, keys.authkey, keys.authkeylen);
	crypto_aead_set_flags(authenc_esn, crypto_ahash_get_flags(auth) &
					   CRYPTO_TFM_RES_MASK);

	if (err)
		goto out;

	crypto_ablkcipher_clear_flags(enc, CRYPTO_TFM_REQ_MASK);
	crypto_ablkcipher_set_flags(enc, crypto_aead_get_flags(authenc_esn) &
					 CRYPTO_TFM_REQ_MASK);
	err = crypto_ablkcipher_setkey(enc, keys.enckey, keys.enckeylen);
	crypto_aead_set_flags(authenc_esn, crypto_ablkcipher_get_flags(enc) &
					   CRYPTO_TFM_RES_MASK);

out:
	return err;

badkey:
	crypto_aead_set_flags(authenc_esn, CRYPTO_TFM_RES_BAD_KEY_LEN);
	goto out;
}
