static int pfkey_spdflush(struct sock *sk, struct sk_buff *skb, const struct sadb_msg *hdr, void * const *ext_hdrs)
{
	struct net *net = sock_net(sk);
	struct km_event c;
	struct xfrm_audit audit_info;
	int err, err2;

	audit_info.loginuid = audit_get_loginuid(current);
	audit_info.sessionid = audit_get_sessionid(current);
	audit_info.secid = 0;
	err = xfrm_policy_flush(net, XFRM_POLICY_TYPE_MAIN, &audit_info);
	err2 = unicast_flush_resp(sk, hdr);
	if (err || err2) {
		if (err == -ESRCH) /* empty table - old silent behavior */
			return 0;
		return err;
	}

	c.data.type = XFRM_POLICY_TYPE_MAIN;
	c.event = XFRM_MSG_FLUSHPOLICY;
	c.portid = hdr->sadb_msg_pid;
	c.seq = hdr->sadb_msg_seq;
	c.net = net;
	km_policy_notify(NULL, 0, &c);

	return 0;
}
