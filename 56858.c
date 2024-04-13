static void init_once(void *foo)
{
	struct mqueue_inode_info *p = (struct mqueue_inode_info *) foo;

	inode_init_once(&p->vfs_inode);
}
