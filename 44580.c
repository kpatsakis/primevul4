static bool nfs_write_pageuptodate(struct page *page, struct inode *inode)
{
	if (nfs_have_delegated_attributes(inode))
		goto out;
	if (NFS_I(inode)->cache_validity & (NFS_INO_INVALID_DATA|NFS_INO_REVAL_PAGECACHE))
		return false;
out:
	return PageUptodate(page) != 0;
}
