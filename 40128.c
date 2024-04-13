static void *skcipher_bind(const char *name, u32 type, u32 mask)
{
	return crypto_alloc_ablkcipher(name, type, mask);
}
