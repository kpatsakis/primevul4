static int key_notify_sa(struct xfrm_state *x, const struct km_event *c)
{
	struct sk_buff *skb;
	struct sadb_msg *hdr;

	skb = pfkey_xfrm_state2msg(x);

	if (IS_ERR(skb))
		return PTR_ERR(skb);

	hdr = (struct sadb_msg *) skb->data;
	hdr->sadb_msg_version = PF_KEY_V2;
	hdr->sadb_msg_type = event2keytype(c->event);
	hdr->sadb_msg_satype = pfkey_proto2satype(x->id.proto);
	hdr->sadb_msg_errno = 0;
	hdr->sadb_msg_reserved = 0;
	hdr->sadb_msg_seq = c->seq;
	hdr->sadb_msg_pid = c->portid;

	pfkey_broadcast(skb, GFP_ATOMIC, BROADCAST_ALL, NULL, xs_net(x));

	return 0;
}
