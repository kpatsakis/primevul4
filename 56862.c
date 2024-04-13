static struct inode *mqueue_alloc_inode(struct super_block *sb)
{
	struct mqueue_inode_info *ei;

	ei = kmem_cache_alloc(mqueue_inode_cachep, GFP_KERNEL);
	if (!ei)
		return NULL;
	return &ei->vfs_inode;
}
