static int padlock_sha256_final(struct shash_desc *desc, u8 *out)
{
	u8 buf[4];

	return padlock_sha256_finup(desc, buf, 0, out);
}
