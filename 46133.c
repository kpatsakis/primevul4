struct btrfs_delalloc_work *btrfs_alloc_delalloc_work(struct inode *inode,
						    int wait, int delay_iput)
{
	struct btrfs_delalloc_work *work;

	work = kmem_cache_zalloc(btrfs_delalloc_work_cachep, GFP_NOFS);
	if (!work)
		return NULL;

	init_completion(&work->completion);
	INIT_LIST_HEAD(&work->list);
	work->inode = inode;
	work->wait = wait;
	work->delay_iput = delay_iput;
	WARN_ON_ONCE(!inode);
	btrfs_init_work(&work->work, btrfs_flush_delalloc_helper,
			btrfs_run_delalloc_work, NULL, NULL);

	return work;
}
