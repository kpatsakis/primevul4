static void *shmem_follow_short_symlink(struct dentry *dentry, struct nameidata *nd)
{
	nd_set_link(nd, SHMEM_I(dentry->d_inode)->symlink);
	return NULL;
}
