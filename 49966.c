static struct bsg_device *bsg_add_device(struct inode *inode,
					 struct request_queue *rq,
					 struct file *file)
{
	struct bsg_device *bd;
#ifdef BSG_DEBUG
	unsigned char buf[32];
#endif
	if (!blk_get_queue(rq))
		return ERR_PTR(-ENXIO);

	bd = bsg_alloc_device();
	if (!bd) {
		blk_put_queue(rq);
		return ERR_PTR(-ENOMEM);
	}

	bd->queue = rq;

	bsg_set_block(bd, file);

	atomic_set(&bd->ref_count, 1);
	mutex_lock(&bsg_mutex);
	hlist_add_head(&bd->dev_list, bsg_dev_idx_hash(iminor(inode)));

	strncpy(bd->name, dev_name(rq->bsg_dev.class_dev), sizeof(bd->name) - 1);
	dprintk("bound to <%s>, max queue %d\n",
		format_dev_t(buf, inode->i_rdev), bd->max_queue);

	mutex_unlock(&bsg_mutex);
	return bd;
}
