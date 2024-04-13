static int fuse_release(struct inode *inode, struct file *file)
{
	struct fuse_conn *fc = get_fuse_conn(inode);

	/* see fuse_vma_close() for !writeback_cache case */
	if (fc->writeback_cache)
		write_inode_now(inode, 1);

	fuse_release_common(file, FUSE_RELEASE);

	/* return value is ignored by VFS */
	return 0;
}
