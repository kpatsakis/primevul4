static struct nfs_page *nfs_page_find_request(struct page *page)
{
	struct inode *inode = page_file_mapping(page)->host;
	struct nfs_page *req = NULL;

	spin_lock(&inode->i_lock);
	req = nfs_page_find_request_locked(NFS_I(inode), page);
	spin_unlock(&inode->i_lock);
	return req;
}
