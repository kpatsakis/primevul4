static void encrypt_to_decrypt(u64 *d, const u64 *e)
{
	const u64 *s = e + (DES_EXPKEY_WORDS / 2) - 1;
	int i;

	for (i = 0; i < DES_EXPKEY_WORDS / 2; i++)
		*d++ = *s--;
}
