int sctp_for_each_endpoint(int (*cb)(struct sctp_endpoint *, void *),
			   void *p) {
	int err = 0;
	int hash = 0;
	struct sctp_ep_common *epb;
	struct sctp_hashbucket *head;

	for (head = sctp_ep_hashtable; hash < sctp_ep_hashsize;
	     hash++, head++) {
		read_lock_bh(&head->lock);
		sctp_for_each_hentry(epb, &head->chain) {
			err = cb(sctp_ep(epb), p);
			if (err)
				break;
		}
		read_unlock_bh(&head->lock);
	}

	return err;
}
