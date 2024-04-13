ssize_t generic_file_splice_read(struct file *in, loff_t *ppos,
				 struct pipe_inode_info *pipe, size_t len,
				 unsigned int flags)
{
	loff_t isize, left;
	int ret;

	isize = i_size_read(in->f_mapping->host);
	if (unlikely(*ppos >= isize))
		return 0;

	left = isize - *ppos;
	if (unlikely(left < len))
		len = left;

	ret = __generic_file_splice_read(in, ppos, pipe, len, flags);
	if (ret > 0) {
		*ppos += ret;
		file_accessed(in);
	}

	return ret;
}
