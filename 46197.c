void btrfs_test_destroy_inode(struct inode *inode)
{
	btrfs_drop_extent_cache(inode, 0, (u64)-1, 0);
	kmem_cache_free(btrfs_inode_cachep, BTRFS_I(inode));
}
