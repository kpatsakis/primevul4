int page_readlink(struct dentry *dentry, char __user *buffer, int buflen)
{
	struct page *page = NULL;
	int res = readlink_copy(buffer, buflen, page_getlink(dentry, &page));
	if (page) {
		kunmap(page);
		page_cache_release(page);
	}
	return res;
}
