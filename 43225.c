static void isofs_i_callback(struct rcu_head *head)
{
	struct inode *inode = container_of(head, struct inode, i_rcu);
	kmem_cache_free(isofs_inode_cachep, ISOFS_I(inode));
}
