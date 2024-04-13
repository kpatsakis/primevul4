static void shmem_destroy_callback(struct rcu_head *head)
{
	struct inode *inode = container_of(head, struct inode, i_rcu);
	kmem_cache_free(shmem_inode_cachep, SHMEM_I(inode));
}
