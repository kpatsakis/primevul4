static int padlock_cra_init(struct crypto_tfm *tfm)
{
	struct crypto_shash *hash = __crypto_shash_cast(tfm);
	const char *fallback_driver_name = crypto_tfm_alg_name(tfm);
	struct padlock_sha_ctx *ctx = crypto_tfm_ctx(tfm);
	struct crypto_shash *fallback_tfm;
	int err = -ENOMEM;

	/* Allocate a fallback and abort if it failed. */
	fallback_tfm = crypto_alloc_shash(fallback_driver_name, 0,
					  CRYPTO_ALG_NEED_FALLBACK);
	if (IS_ERR(fallback_tfm)) {
		printk(KERN_WARNING PFX "Fallback driver '%s' could not be loaded!\n",
		       fallback_driver_name);
		err = PTR_ERR(fallback_tfm);
		goto out;
	}

	ctx->fallback = fallback_tfm;
	hash->descsize += crypto_shash_descsize(fallback_tfm);
	return 0;

out:
	return err;
}
