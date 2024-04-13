static bool reqsk_queue_unlink(struct request_sock_queue *queue,
			       struct request_sock *req)
{
	struct inet_hashinfo *hashinfo = req_to_sk(req)->sk_prot->h.hashinfo;
	bool found = false;

	if (sk_hashed(req_to_sk(req))) {
		spinlock_t *lock = inet_ehash_lockp(hashinfo, req->rsk_hash);

		spin_lock(lock);
		found = __sk_nulls_del_node_init_rcu(req_to_sk(req));
		spin_unlock(lock);
	}
	if (timer_pending(&req->rsk_timer) && del_timer_sync(&req->rsk_timer))
		reqsk_put(req);
	return found;
}
