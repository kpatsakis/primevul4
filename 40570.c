static int pfkey_add(struct sock *sk, struct sk_buff *skb, const struct sadb_msg *hdr, void * const *ext_hdrs)
{
	struct net *net = sock_net(sk);
	struct xfrm_state *x;
	int err;
	struct km_event c;

	x = pfkey_msg2xfrm_state(net, hdr, ext_hdrs);
	if (IS_ERR(x))
		return PTR_ERR(x);

	xfrm_state_hold(x);
	if (hdr->sadb_msg_type == SADB_ADD)
		err = xfrm_state_add(x);
	else
		err = xfrm_state_update(x);

	xfrm_audit_state_add(x, err ? 0 : 1,
			     audit_get_loginuid(current),
			     audit_get_sessionid(current), 0);

	if (err < 0) {
		x->km.state = XFRM_STATE_DEAD;
		__xfrm_state_put(x);
		goto out;
	}

	if (hdr->sadb_msg_type == SADB_ADD)
		c.event = XFRM_MSG_NEWSA;
	else
		c.event = XFRM_MSG_UPDSA;
	c.seq = hdr->sadb_msg_seq;
	c.portid = hdr->sadb_msg_pid;
	km_state_notify(x, &c);
out:
	xfrm_state_put(x);
	return err;
}
