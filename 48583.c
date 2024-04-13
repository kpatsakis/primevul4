static struct crypto_instance *pcrypt_alloc(struct rtattr **tb)
{
	struct crypto_attr_type *algt;

	algt = crypto_get_attr_type(tb);
	if (IS_ERR(algt))
		return ERR_CAST(algt);

	switch (algt->type & algt->mask & CRYPTO_ALG_TYPE_MASK) {
	case CRYPTO_ALG_TYPE_AEAD:
		return pcrypt_alloc_aead(tb, algt->type, algt->mask);
	}

	return ERR_PTR(-EINVAL);
}
