static struct shash_desc *init_desc(char type)
{
	long rc;
	char *algo;
	struct crypto_shash **tfm;
	struct shash_desc *desc;

	if (type == EVM_XATTR_HMAC) {
		tfm = &hmac_tfm;
		algo = evm_hmac;
	} else {
		tfm = &hash_tfm;
		algo = evm_hash;
	}

	if (*tfm == NULL) {
		mutex_lock(&mutex);
		if (*tfm)
			goto out;
		*tfm = crypto_alloc_shash(algo, 0, CRYPTO_ALG_ASYNC);
		if (IS_ERR(*tfm)) {
			rc = PTR_ERR(*tfm);
			pr_err("Can not allocate %s (reason: %ld)\n", algo, rc);
			*tfm = NULL;
			mutex_unlock(&mutex);
			return ERR_PTR(rc);
		}
		if (type == EVM_XATTR_HMAC) {
			rc = crypto_shash_setkey(*tfm, evmkey, evmkey_len);
			if (rc) {
				crypto_free_shash(*tfm);
				*tfm = NULL;
				mutex_unlock(&mutex);
				return ERR_PTR(rc);
			}
		}
out:
		mutex_unlock(&mutex);
	}

	desc = kmalloc(sizeof(*desc) + crypto_shash_descsize(*tfm),
			GFP_KERNEL);
	if (!desc)
		return ERR_PTR(-ENOMEM);

	desc->tfm = *tfm;
	desc->flags = CRYPTO_TFM_REQ_MAY_SLEEP;

	rc = crypto_shash_init(desc);
	if (rc) {
		kfree(desc);
		return ERR_PTR(rc);
	}
	return desc;
}
