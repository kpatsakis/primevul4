static void init_once(void *foo)
{
	struct iso_inode_info *ei = foo;

	inode_init_once(&ei->vfs_inode);
}
