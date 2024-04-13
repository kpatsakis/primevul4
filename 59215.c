static bool perf_addr_filter_match(struct perf_addr_filter *filter,
				     struct file *file, unsigned long offset,
				     unsigned long size)
{
	if (filter->inode != file_inode(file))
		return false;

	if (filter->offset > offset + size)
		return false;

	if (filter->offset + filter->size < offset)
		return false;

	return true;
}
