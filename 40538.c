static int shash_default_export(struct shash_desc *desc, void *out)
{
	memcpy(out, shash_desc_ctx(desc), crypto_shash_descsize(desc->tfm));
	return 0;
}
