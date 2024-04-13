static struct bsg_device *__bsg_get_device(int minor, struct request_queue *q)
{
	struct bsg_device *bd;

	mutex_lock(&bsg_mutex);

	hlist_for_each_entry(bd, bsg_dev_idx_hash(minor), dev_list) {
		if (bd->queue == q) {
			atomic_inc(&bd->ref_count);
			goto found;
		}
	}
	bd = NULL;
found:
	mutex_unlock(&bsg_mutex);
	return bd;
}
