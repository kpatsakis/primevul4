static int crypto_init_ops(struct crypto_tfm *tfm, u32 type, u32 mask)
{
	const struct crypto_type *type_obj = tfm->__crt_alg->cra_type;

	if (type_obj)
		return type_obj->init(tfm, type, mask);

	switch (crypto_tfm_alg_type(tfm)) {
	case CRYPTO_ALG_TYPE_CIPHER:
		return crypto_init_cipher_ops(tfm);

	case CRYPTO_ALG_TYPE_COMPRESS:
		return crypto_init_compress_ops(tfm);

	default:
		break;
	}

	BUG();
	return -EINVAL;
}
