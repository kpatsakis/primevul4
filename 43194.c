static int udf_write_begin(struct file *file, struct address_space *mapping,
			loff_t pos, unsigned len, unsigned flags,
			struct page **pagep, void **fsdata)
{
	int ret;

	ret = block_write_begin(mapping, pos, len, flags, pagep, udf_get_block);
	if (unlikely(ret))
		udf_write_failed(mapping, pos + len);
	return ret;
}
