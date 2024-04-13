static void __sctp_hash_endpoint(struct sctp_endpoint *ep)
{
	struct sctp_ep_common *epb;
	struct sctp_hashbucket *head;

	epb = &ep->base;

	epb->hashent = sctp_ep_hashfn(epb->bind_addr.port);
	head = &sctp_ep_hashtable[epb->hashent];

	sctp_write_lock(&head->lock);
	hlist_add_head(&epb->node, &head->chain);
	sctp_write_unlock(&head->lock);
}
