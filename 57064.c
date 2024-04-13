static int __init init_inodecache(void)
{
	f2fs_inode_cachep = kmem_cache_create("f2fs_inode_cache",
			sizeof(struct f2fs_inode_info), 0,
			SLAB_RECLAIM_ACCOUNT|SLAB_ACCOUNT, NULL);
	if (!f2fs_inode_cachep)
		return -ENOMEM;
	return 0;
}
