static void *shmem_follow_link(struct dentry *dentry, struct nameidata *nd)
{
	struct page *page = NULL;
	int error = shmem_getpage(dentry->d_inode, 0, &page, SGP_READ, NULL);
	nd_set_link(nd, error ? ERR_PTR(error) : kmap(page));
	if (page)
		unlock_page(page);
	return page;
}
