static void bdev_i_callback(struct rcu_head *head)
{
	struct inode *inode = container_of(head, struct inode, i_rcu);
	struct bdev_inode *bdi = BDEV_I(inode);

	kmem_cache_free(bdev_cachep, bdi);
}
