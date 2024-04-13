static void mqueue_i_callback(struct rcu_head *head)
{
	struct inode *inode = container_of(head, struct inode, i_rcu);
	kmem_cache_free(mqueue_inode_cachep, MQUEUE_I(inode));
}
