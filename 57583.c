int crypto_grab_skcipher(struct crypto_skcipher_spawn *spawn,
			  const char *name, u32 type, u32 mask)
{
	spawn->base.frontend = &crypto_skcipher_type2;
	return crypto_grab_spawn(&spawn->base, name, type, mask);
}
