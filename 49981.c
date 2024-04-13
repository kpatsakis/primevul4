static int bsg_put_device(struct bsg_device *bd)
{
	int ret = 0, do_free;
	struct request_queue *q = bd->queue;

	mutex_lock(&bsg_mutex);

	do_free = atomic_dec_and_test(&bd->ref_count);
	if (!do_free) {
		mutex_unlock(&bsg_mutex);
		goto out;
	}

	hlist_del(&bd->dev_list);
	mutex_unlock(&bsg_mutex);

	dprintk("%s: tearing down\n", bd->name);

	/*
	 * close can always block
	 */
	set_bit(BSG_F_BLOCK, &bd->flags);

	/*
	 * correct error detection baddies here again. it's the responsibility
	 * of the app to properly reap commands before close() if it wants
	 * fool-proof error detection
	 */
	ret = bsg_complete_all_commands(bd);

	kfree(bd);
out:
	kref_put(&q->bsg_dev.ref, bsg_kref_release_function);
	if (do_free)
		blk_put_queue(q);
	return ret;
}
