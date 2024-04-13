static void __sctp_unhash_endpoint(struct sctp_endpoint *ep)
{
	struct sctp_hashbucket *head;
	struct sctp_ep_common *epb;

	epb = &ep->base;

	if (hlist_unhashed(&epb->node))
		return;

	epb->hashent = sctp_ep_hashfn(epb->bind_addr.port);

	head = &sctp_ep_hashtable[epb->hashent];

	sctp_write_lock(&head->lock);
	__hlist_del(&epb->node);
	sctp_write_unlock(&head->lock);
}
