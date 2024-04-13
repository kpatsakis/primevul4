static int padlock_sha1_final(struct shash_desc *desc, u8 *out)
{
	u8 buf[4];

	return padlock_sha1_finup(desc, buf, 0, out);
}
