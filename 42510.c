static struct sctp_association *__sctp_lookup_association(
					const union sctp_addr *local,
					const union sctp_addr *peer,
					struct sctp_transport **pt)
{
	struct sctp_hashbucket *head;
	struct sctp_ep_common *epb;
	struct sctp_association *asoc;
	struct sctp_transport *transport;
	struct hlist_node *node;
	int hash;

	/* Optimize here for direct hit, only listening connections can
	 * have wildcards anyways.
	 */
	hash = sctp_assoc_hashfn(ntohs(local->v4.sin_port), ntohs(peer->v4.sin_port));
	head = &sctp_assoc_hashtable[hash];
	read_lock(&head->lock);
	sctp_for_each_hentry(epb, node, &head->chain) {
		asoc = sctp_assoc(epb);
		transport = sctp_assoc_is_match(asoc, local, peer);
		if (transport)
			goto hit;
	}

	read_unlock(&head->lock);

	return NULL;

hit:
	*pt = transport;
	sctp_association_hold(asoc);
	read_unlock(&head->lock);
	return asoc;
}
