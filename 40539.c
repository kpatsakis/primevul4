static int shash_default_import(struct shash_desc *desc, const void *in)
{
	memcpy(shash_desc_ctx(desc), in, crypto_shash_descsize(desc->tfm));
	return 0;
}
