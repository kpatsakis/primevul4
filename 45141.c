static void cifs_invalidate_page(struct page *page, unsigned int offset,
				 unsigned int length)
{
	struct cifsInodeInfo *cifsi = CIFS_I(page->mapping->host);

	if (offset == 0 && length == PAGE_CACHE_SIZE)
		cifs_fscache_invalidate_page(page, &cifsi->vfs_inode);
}
