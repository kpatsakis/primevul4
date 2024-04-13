static void __unqueue_futex(struct futex_q *q)
{
	struct futex_hash_bucket *hb;

	if (WARN_ON_SMP(!q->lock_ptr || !spin_is_locked(q->lock_ptr))
	    || WARN_ON(plist_node_empty(&q->list)))
		return;

	hb = container_of(q->lock_ptr, struct futex_hash_bucket, lock);
	plist_del(&q->list, &hb->chain);
}
