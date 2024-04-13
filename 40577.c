static int pfkey_do_dump(struct pfkey_sock *pfk)
{
	struct sadb_msg *hdr;
	int rc;

	rc = pfk->dump.dump(pfk);
	if (rc == -ENOBUFS)
		return 0;

	if (pfk->dump.skb) {
		if (!pfkey_can_dump(&pfk->sk))
			return 0;

		hdr = (struct sadb_msg *) pfk->dump.skb->data;
		hdr->sadb_msg_seq = 0;
		hdr->sadb_msg_errno = rc;
		pfkey_broadcast(pfk->dump.skb, GFP_ATOMIC, BROADCAST_ONE,
				&pfk->sk, sock_net(&pfk->sk));
		pfk->dump.skb = NULL;
	}

	pfkey_terminate_dump(pfk);
	return rc;
}
