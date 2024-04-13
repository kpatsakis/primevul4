static void bdev_destroy_inode(struct inode *inode)
{
	call_rcu(&inode->i_rcu, bdev_i_callback);
}
