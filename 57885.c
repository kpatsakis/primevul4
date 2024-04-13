static void reqsk_queue_hash_req(struct request_sock *req,
				 unsigned long timeout)
{
	req->num_retrans = 0;
	req->num_timeout = 0;
	req->sk = NULL;

	setup_pinned_timer(&req->rsk_timer, reqsk_timer_handler,
			    (unsigned long)req);
	mod_timer(&req->rsk_timer, jiffies + timeout);

	inet_ehash_insert(req_to_sk(req), NULL);
	/* before letting lookups find us, make sure all req fields
	 * are committed to memory and refcnt initialized.
	 */
	smp_wmb();
	atomic_set(&req->rsk_refcnt, 2 + 1);
}
