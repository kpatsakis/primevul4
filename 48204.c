static loff_t lower_offset_for_page(struct ecryptfs_crypt_stat *crypt_stat,
				    struct page *page)
{
	return ecryptfs_lower_header_size(crypt_stat) +
	       ((loff_t)page->index << PAGE_CACHE_SHIFT);
}
