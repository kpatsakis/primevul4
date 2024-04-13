xfs_iozero(
	struct xfs_inode	*ip,	/* inode			*/
	loff_t			pos,	/* offset in file		*/
	size_t			count)	/* size of data to zero		*/
{
	struct page		*page;
	struct address_space	*mapping;
	int			status;

	mapping = VFS_I(ip)->i_mapping;
	do {
		unsigned offset, bytes;
		void *fsdata;

		offset = (pos & (PAGE_CACHE_SIZE -1)); /* Within page */
		bytes = PAGE_CACHE_SIZE - offset;
		if (bytes > count)
			bytes = count;

		status = pagecache_write_begin(NULL, mapping, pos, bytes,
					AOP_FLAG_UNINTERRUPTIBLE,
					&page, &fsdata);
		if (status)
			break;

		zero_user(page, offset, bytes);

		status = pagecache_write_end(NULL, mapping, pos, bytes, bytes,
					page, fsdata);
		WARN_ON(status <= 0); /* can't return less than zero! */
		pos += bytes;
		count -= bytes;
		status = 0;
	} while (count);

	return (-status);
}
