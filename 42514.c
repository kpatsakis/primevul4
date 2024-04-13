static struct sctp_endpoint *__sctp_rcv_lookup_endpoint(const union sctp_addr *laddr)
{
	struct sctp_hashbucket *head;
	struct sctp_ep_common *epb;
	struct sctp_endpoint *ep;
	struct hlist_node *node;
	int hash;

	hash = sctp_ep_hashfn(ntohs(laddr->v4.sin_port));
	head = &sctp_ep_hashtable[hash];
	read_lock(&head->lock);
	sctp_for_each_hentry(epb, node, &head->chain) {
		ep = sctp_ep(epb);
		if (sctp_endpoint_is_match(ep, laddr))
			goto hit;
	}

	ep = sctp_sk((sctp_get_ctl_sock()))->ep;

hit:
	sctp_endpoint_hold(ep);
	read_unlock(&head->lock);
	return ep;
}
