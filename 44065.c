void requeue_futex(struct futex_q *q, struct futex_hash_bucket *hb1,
		   struct futex_hash_bucket *hb2, union futex_key *key2)
{

	/*
	 * If key1 and key2 hash to the same bucket, no need to
	 * requeue.
	 */
	if (likely(&hb1->chain != &hb2->chain)) {
		plist_del(&q->list, &hb1->chain);
		hb_waiters_dec(hb1);
		plist_add(&q->list, &hb2->chain);
		hb_waiters_inc(hb2);
		q->lock_ptr = &hb2->lock;
	}
	get_futex_key_refs(key2);
	q->key = *key2;
}
