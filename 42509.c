static void __sctp_hash_established(struct sctp_association *asoc)
{
	struct sctp_ep_common *epb;
	struct sctp_hashbucket *head;

	epb = &asoc->base;

	/* Calculate which chain this entry will belong to. */
	epb->hashent = sctp_assoc_hashfn(epb->bind_addr.port, asoc->peer.port);

	head = &sctp_assoc_hashtable[epb->hashent];

	sctp_write_lock(&head->lock);
	hlist_add_head(&epb->node, &head->chain);
	sctp_write_unlock(&head->lock);
}
