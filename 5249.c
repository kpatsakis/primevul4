int digest_generic_digest(struct digest *d, const void *data,
			  unsigned int len, u8 *md)

{
	int ret;

	if (!data || len == 0 || !md)
		return -EINVAL;

	ret = digest_init(d);
	if (ret)
		return ret;
	ret = digest_update(d, data, len);
	if (ret)
		return ret;
	return digest_final(d, md);
}