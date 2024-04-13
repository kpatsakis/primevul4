static void *hash_bind(const char *name, u32 type, u32 mask)
{
	return crypto_alloc_ahash(name, type, mask);
}
