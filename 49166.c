static int crc32c_sparc64_update(struct shash_desc *desc, const u8 *data,
				 unsigned int len)
{
	u32 *crcp = shash_desc_ctx(desc);

	crc32c_compute(crcp, (const u64 *) data, len);

	return 0;
}
