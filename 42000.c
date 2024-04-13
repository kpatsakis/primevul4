static void shmem_put_link(struct dentry *dentry, struct nameidata *nd, void *cookie)
{
	if (!IS_ERR(nd_get_link(nd))) {
		struct page *page = cookie;
		kunmap(page);
		mark_page_accessed(page);
		page_cache_release(page);
	}
}
