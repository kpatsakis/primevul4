shmem_write_begin(struct file *file, struct address_space *mapping,
			loff_t pos, unsigned len, unsigned flags,
			struct page **pagep, void **fsdata)
{
	struct inode *inode = mapping->host;
	pgoff_t index = pos >> PAGE_CACHE_SHIFT;
	return shmem_getpage(inode, index, pagep, SGP_WRITE, NULL);
}
