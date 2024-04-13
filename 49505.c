static void crypto_exit_ops(struct crypto_tfm *tfm)
{
	const struct crypto_type *type = tfm->__crt_alg->cra_type;

	if (type) {
		if (tfm->exit)
			tfm->exit(tfm);
		return;
	}

	switch (crypto_tfm_alg_type(tfm)) {
	case CRYPTO_ALG_TYPE_CIPHER:
		crypto_exit_cipher_ops(tfm);
		break;

	case CRYPTO_ALG_TYPE_COMPRESS:
		crypto_exit_compress_ops(tfm);
		break;

	default:
		BUG();
	}
}
