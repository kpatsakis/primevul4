static inline unsigned fuse_wr_pages(loff_t pos, size_t len)
{
	return min_t(unsigned,
		     ((pos + len - 1) >> PAGE_CACHE_SHIFT) -
		     (pos >> PAGE_CACHE_SHIFT) + 1,
		     FUSE_MAX_PAGES_PER_REQ);
}
