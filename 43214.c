static void isofs_destroy_inode(struct inode *inode)
{
	call_rcu(&inode->i_rcu, isofs_i_callback);
}
