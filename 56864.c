static void mqueue_destroy_inode(struct inode *inode)
{
	call_rcu(&inode->i_rcu, mqueue_i_callback);
}
